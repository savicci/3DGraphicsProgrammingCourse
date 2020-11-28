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
    rotation_period = 4.0;

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);

    preparePVM();
    start_ = std::chrono::steady_clock::now();
}

void SimpleShapeApplication::preparePVM() {
    glm::vec3 eye = glm::vec3(3.0, 18.0, -5.0); // pos of camera
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // where camera looks at
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0); // what is 'up' axis for camera
    camera()->look_at(eye, center, up);

    int w, h;
    std::tie(w, h) = frame_buffer_size();

    float aspect = (float) w / h;
    float fov = glm::pi<float>() / 2.0;
    float near = 0.1f;
    float far = 100.0f;
    camera()->perspective(fov, aspect, near, far);

    u_pvm_buffer_ = glGetUniformBlockIndex(program_, "PVM");
    if (u_pvm_buffer_ == GL_INVALID_INDEX) {
        std::cout << "Cannot find PVM uniform block in program" << std::endl;
    }
}

void SimpleShapeApplication::frame() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(now - start_).count();

    glm::mat4 PVM = prepareMatrix(elapsed_time);
    setPVMUniformBufferData(PVM);
    pyramid_->draw();
}

glm::mat4 SimpleShapeApplication::prepareMatrix(float elapsed_time) {
    auto rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / rotation_period;

    auto axis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::rotate(glm::mat4(1.0f), rotation_angle, axis);

    float a = 20;
    float b = 8;
    float orbital_rotation_period = 20.0f;
    float orbital_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / orbital_rotation_period;
    float x = a * cos(orbital_rotation_angle);
    float z = b * sin(orbital_rotation_angle);
    auto O = glm::translate(glm::mat4(1.0f), glm::vec3{x, 0.0, z});

    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(R, glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(R, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 M = O * R;

    return camera()->projection() * camera()->view() * M;
}

void SimpleShapeApplication::setPVMUniformBufferData(const glm::mat4 &PVM) const {
    unsigned int ubo_handle(1u);
    glGenBuffers(1, &ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PVM), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_handle);

    glUniformBlockBinding(program_, u_pvm_buffer_, 1);
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
