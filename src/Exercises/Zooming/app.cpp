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

    std::vector<GLfloat> vertices{
            // front
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

            // left
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,

            // right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,

            // back
            -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,

            // bottom
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f
    };

    unsigned int indices[] = {
            0,1,2,
            3,4,5,
            6,7,8,
            9,10,11,
            12,13,14,
            13,14,15
    };

    GLuint vbo_handle;
    glGenBuffers(1, &vbo_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vao_);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));


    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // uniforms
    float strength = 0.3f;
    float light[3] = {0.6, 0.4, 0.6};

    GLuint ubo_handle(0u);
    glGenBuffers(1, &ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 8 * sizeof(float), &strength);
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
    preparePVM(program);
}

void SimpleShapeApplication::preparePVM(GLuint program) {
    glm::vec3 eye = glm::vec3(-1.0, 1.0, -1.0); // pos of camera
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0); // where camera looks at
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0); // what is 'up' axis for camera

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    aspect_ = (float)w/h;
    fov_ = glm::pi<float>()/4.0;
    near_ = 0.1f;
    far_ = 100.0f;

    P_ = glm::perspective(fov_, aspect_, near_, far_);
    V_ = glm::lookAt(eye, center, up);
    glm::mat4 M(1.0f); // model


    u_pvm_buffer_ = glGetUniformLocation(program, "proj_matrix");
    if (u_pvm_buffer_ != 0) {
        std::cerr << "Error occurred while trying to find uniform proj_matrix" << std::endl;
    }
}

void SimpleShapeApplication::frame() {
    glm::mat4 PVM = P_ * V_;
    glUniformMatrix4fv(u_pvm_buffer_, 1, false, glm::value_ptr(PVM));

    glBindVertexArray(vao_);
    glEnable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (void*)nullptr);
    glBindVertexArray(0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0,0,w,h);
    aspect_ = (float) w / h;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
}
