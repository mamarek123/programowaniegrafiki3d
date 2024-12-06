//
// Created by pbialas on 25.09.2020.
//


#include "app.h"
#include "camera.h"
#include <vector>
#include "spdlog/spdlog.h"
#include "glad/gl.h"
#include "Application/utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, glm::lookAt, glm::perspective
#include <glm/gtc/type_ptr.hpp>         // For glm::value_ptr

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    //
    OGL_CALL(glViewport(0, 0, w, h));
}

void SimpleShapeApplication::init() {
    /*
     * A utility function that reads the shaders' source files, compiles them and creates the program object,
     * as everything in OpenGL we reference the program by an integer "handle".
     * The input to this function is a map that associates a shader type (GL_VERTEX_SHADER and GL_FRAGMENT_SHADER) with
     * its source file. The source files are located in the PROJECT_DIR/shaders directory, where  PROJECT_DIR is the
     * current assignment directory (e.g., src/Assignments/Triangle).
     */
    set_camera(new Camera);
    set_controler(new CameraController(camera()));

    auto program = xe::utils::create_program(
        {
            {GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
            {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
        });

    if (!program) {
        SPDLOG_CRITICAL("Invalid program");
        exit(-1);
    }


    std::vector<GLfloat> vertices = {

        //base
        -0.5f, 0.5f, 0.0f, 0.5, 0.5, 0.5,  //0
        -0.5f, -0.5f, 0.0f, 0.5, 0.5, 0.5, //1
        0.5f, -0.5f, 0.0f, 0.5, 0.5, 0.5,  //2
        0.5f, 0.5f, 0.0f, 0.5, 0.5, 0.5,   //3
        //red wall
        0.5f, -0.5f, 0.0f, 1, 0, 0,        //4
        0.0f, 0.0f, 1.0f, 1, 0, 0,         //5
        0.5f, 0.5f, 0.0f, 1, 0, 0,         //6
        // green wall
        -0.5f, -0.5f, 0.0f, 0, 1, 0,        //7
        0.0f, 0.0f, 1.0f, 0, 1, 0,          //8
        -0.5f, 0.5f, 0.0f, 0, 1, 0,         //9
        //blue wall
        0.5f, 0.5f, 0.0f, 0, 0, 1,         //10
        0.0f, 0.0f, 1.0f, 0, 0, 1,         //11
        -0.5f, 0.5f, 0.0f, 0, 0, 1,        //12
        //yellow wall
        0.5f, -0.5f, 0.0f, 1, 1, 0,        //13
        0.0f, 0.0f, 1.0f, 1, 1, 0,         //14
        -0.5f, -0.5f, 0.0f, 1, 1, 0,       //15

        };

    std::vector<GLubyte> indices = {
        0, 2, 1,
        0, 3, 2,
        4, 6, 5,
        7, 8, 9,
        10, 12, 11,
        13, 14, 15
    };


    /*
     * All the calls to the OpenGL API are "encapsulated" in the OGL_CALL macro for debugging purposes as explained in
     * Assignments/DEBUGGING.md. The macro is defined in src/Application/utils.h. If the call to the OpenGL API returns an
     * error code, the macro will print the name of the function that failed, the file and line number where the error
     * occurred.
     */

    // Generate buffers for vertices and indices
    GLuint v_buffer_handle, i_buffer_handle;
    OGL_CALL(glCreateBuffers(1, &v_buffer_handle));
    OGL_CALL(glNamedBufferData(v_buffer_handle, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW));

    OGL_CALL(glCreateBuffers(1, &i_buffer_handle));
    OGL_CALL(glNamedBufferData(i_buffer_handle, indices.size() * sizeof(GLubyte), indices.data(), GL_STATIC_DRAW));

    // Setup VAO
    OGL_CALL(glGenVertexArrays(1, &vao_));
    OGL_CALL(glBindVertexArray(vao_));
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle));

    /*
     * The following lines bound the vertex attribute 0 to the currently bound vertex buffer (the one we just created).
     * Attribute 0 is specified in the vertex shader with the
     * layout (location = 0) in vec4 a_vertex_position;
     * directive.
     */

    // This specifies that the data for attribute 0 should be read from a vertex buffer
    OGL_CALL(glEnableVertexAttribArray(0));
    // and this specifies the data layout in the buffer.
    OGL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                               reinterpret_cast<GLvoid *>(0)));

    OGL_CALL(glEnableVertexAttribArray(1));
    OGL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                                reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat))));
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    OGL_CALL(glBindVertexArray(0));

 
    OGL_CALL(glClearColor(0.1f, 0.75f, 0.75f, 1.0f));




    M_ = glm::mat4(1.0);



    auto [w, h] = frame_buffer_size(); // Get width and height of the frame buffer
    camera()->look_at(glm::vec3(2.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    camera()->perspective(glm::radians(45.0f), static_cast<float>(w) / h, 0.1f, 100.0f);

    OGL_CALL(glCreateBuffers(1, &u_trans_buffer_handle_));
    OGL_CALL(glNamedBufferData(u_trans_buffer_handle_, 16 * sizeof(float), nullptr, GL_STATIC_DRAW));
    //OGL_CALL(glNamedBufferSubData(u_trans_buffer_handle_, 0, 16 * sizeof(float), &PVM[0]));


    OGL_CALL(glViewport(0, 0, w, h));
    OGL_CALL(glUseProgram(program));
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

}


void SimpleShapeApplication::frame() {
    auto [w, h] = frame_buffer_size();

    glm::mat4 V = camera()->view();
    glm::mat4 P = camera()->projection();


    glm::mat4 PVM = P * V * M_;
    OGL_CALL(glNamedBufferSubData(u_trans_buffer_handle_, 0, 16 * sizeof(float), &PVM[0]));
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1,u_trans_buffer_handle_));

    OGL_CALL(glBindVertexArray(vao_));
    OGL_CALL(glDrawElements(GL_TRIANGLES, 18,GL_UNSIGNED_BYTE,nullptr));
    OGL_CALL(glBindVertexArray(0));

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);  
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(yoffset / 20.0f);
}

