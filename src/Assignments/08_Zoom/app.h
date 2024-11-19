//
// Created by pbialas on 05.08.2020.
//

#pragma once


#include <vector>

#include "camera.h"

#include "glad/gl.h"

#include <glm/glm.hpp>

#include "Application/application.h"


class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) :
    Application(width, height, title, debug), camera_(nullptr) {}

    void init() override;

    Camera *camera_;

    void set_camera(Camera *camera) { camera_ = camera; }
   
    Camera *camera() const {
        assert(camera_);
        return camera_;
    }
    

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

    glm::mat4 M_;

    GLuint u_trans_buffer_handle_;

    void scroll_callback(double xoffset, double yoffset) override;

private:
    GLuint vao_;
};