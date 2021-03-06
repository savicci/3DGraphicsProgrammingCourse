//
// Created by pbialas on 05.08.2020.
//


#pragma once

#include <vector>
#include <glm/mat4x4.hpp>


#include "Application/application.h"
#include "Application/utils.h"

#include "glad/glad.h"
#include "camera.h"
#include "camera_controller.h"
#include "pyramid.h"

class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor) {}

    void init() override;;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void cursor_position_callback(double x, double y) override;

    void scroll_callback(double xoffset, double yoffset) override {
        Application::scroll_callback(xoffset, yoffset);
        camera()->zoom(yoffset / 30.0f);
    }

    void set_camera(Camera *camera) { camera_ = camera; }

    void set_controler(CameraControler *controler) { controler_ = controler; }
    Camera *camera() { return camera_; }

    ~SimpleShapeApplication() {
        delete camera_;
    }

    std::shared_ptr<Pyramid> pyramid_;

private:
    GLuint vao_;
    GLuint u_pvm_buffer_;
    GLuint pvm_handle;
    Camera *camera_;
    CameraControler *controler_;
    unsigned int program_;

    void preparePVM();
    void setPVMUniformBufferData(const glm::mat4 &PVM) const;
};