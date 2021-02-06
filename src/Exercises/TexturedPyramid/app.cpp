//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>


#include "Application/utils.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void SimpleShapeApplication::init() {


    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");


    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

    program_ = program;
    set_camera(new Camera());
    set_controler(new CameraControler(camera()));
    pyramid_ = std::make_shared<Pyramid>();

    // uniforms
    float strength = 0.3f;
    float light[3] = {0.6, 0.4, 0.6};

    GLuint ubo_handle(0u);
    glGenBuffers(1, &ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_handle);


    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);

    auto u_modifiers_index = glGetUniformBlockIndex(program, "Modifiers");
    if (u_modifiers_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Modifiers uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program, u_modifiers_index, 0);
    }

    preparePVM();

    auto  u_diffuse_map_location = glGetUniformLocation(program,"diffuse_map");
    if(u_diffuse_map_location==-1) {
        std::cerr<<"Cannot find uniform diffuse_map\n";
    } else {
        glUniform1ui(u_diffuse_map_location,0);
    }

}

void SimpleShapeApplication::preparePVM() {
    glm::vec3 eye = glm::vec3(-1.0, 1.0, -1.0); // pos of camera
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // where camera looks at
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0); // what is 'up' axis for camera
    camera()->look_at(eye, center, up);

    int w, h;
    std::tie(w, h) = frame_buffer_size();

    float aspect = (float) w / h;
    float fov = glm::pi<float>() / 4.0;
    float near = 0.1f;
    float far = 100.0f;
    camera()->perspective(fov, aspect, near, far);

    u_pvm_buffer_ = glGetUniformBlockIndex(program_, "PVM");
    if (u_pvm_buffer_ == GL_INVALID_INDEX) {
        std::cout << "Cannot find PVM uniform block in program" << std::endl;
    }

    glGenBuffers(1, &pvm_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, pvm_handle);
    glUniformBlockBinding(program_, u_pvm_buffer_, 1);
}

void SimpleShapeApplication::frame() {
    glm::mat4 PVM = camera()->projection() * camera()->view();
    setPVMUniformBufferData(PVM);
    pyramid_->draw();
}

void SimpleShapeApplication::setPVMUniformBufferData(const glm::mat4 &PVM) const {
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    float aspect = (float) w / h;
    camera()->set_aspect(aspect);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}
