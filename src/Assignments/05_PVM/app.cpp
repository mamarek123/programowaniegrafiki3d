//
// Created by pbialas on 25.09.2020.
//


#include "app.h"

#include <vector>
#include "spdlog/spdlog.h"
#include "glad/gl.h"
#include "Application/utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, glm::lookAt, glm::perspective
#include <glm/gtc/type_ptr.hpp>         // For glm::value_ptr

void SimpleShapeApplication::init() {
    /*
     * A utility function that reads the shaders' source files, compiles them and creates the program object,
     * as everything in OpenGL we reference the program by an integer "handle".
     * The input to this function is a map that associates a shader type (GL_VERTEX_SHADER and GL_FRAGMENT_SHADER) with
     * its source file. The source files are located in the PROJECT_DIR/shaders directory, where  PROJECT_DIR is the
     * current assignment directory (e.g., src/Assignments/Triangle).
     */
    auto program = xe::utils::create_program(
        {
            {GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
            {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
        });

    if (!program) {
        SPDLOG_CRITICAL("Invalid program");
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the triangle.
    std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, //0
         0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, //1
        -0.5f,  0.0f, 0.0f,    0.0f, 1.0f, 0.0f, //2
         0.5f,  0.0f, 0.0f,    0.0f, 1.0f, 0.0f, //3
        -0.5f,  0.0f, 0.0f,    1.0f, 0.0f, 0.0f, //4
         0.5f,  0.0f, 0.0f,    1.0f, 0.0f, 0.0f, //5
         0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f, //6
    };

    std::vector<GLubyte> indices = {
        0, 1, 2,
        1, 3, 2,
        4, 5, 6,
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

    float strength = 0.5;
    float mix_color[3] = {0.0, 0.0, 1.0}; 


    GLuint mixerBuffer;
    OGL_CALL(glCreateBuffers(1, &mixerBuffer));
    OGL_CALL(glNamedBufferData(mixerBuffer, 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW)); 

    OGL_CALL(glNamedBufferSubData(mixerBuffer, 0, sizeof(float), &strength));
    OGL_CALL(glNamedBufferSubData(mixerBuffer, 16, 3 * sizeof(float), mix_color)); // mix_color


    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, mixerBuffer));


    glm::mat4 M(1.0);
    M = glm::translate(M, glm::vec3(-1.0f, 1.0f, 0.0f));
    glm::mat4 V = glm::lookAt(
        glm::vec3(0.0f, -2.0f, 2.0f), 
        glm::vec3(0.0f, 0.0f, 0.0f),  
        glm::vec3(0.0f, 0.0f, 1.0f)   
    );

    auto [w, h] = frame_buffer_size();
    float aspect = static_cast<float>(w) / static_cast<float>(h);

    glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 20.0f);

    glm::mat4 PVM = P * V * M;

    GLuint PVMBuffer;
    OGL_CALL(glCreateBuffers(1, &PVMBuffer));
    OGL_CALL(glNamedBufferData(PVMBuffer, 16 * sizeof(float), nullptr, GL_STATIC_DRAW));
    OGL_CALL(glNamedBufferSubData(PVMBuffer, 0, 16 * sizeof(float), &PVM[0]));
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1,PVMBuffer));


    OGL_CALL(glViewport(0, 0, w, h));
    OGL_CALL(glUseProgram(program));

}


void SimpleShapeApplication::frame() {
    
    OGL_CALL(glBindVertexArray(vao_));
    OGL_CALL(glDrawElements(GL_TRIANGLES, 9,GL_UNSIGNED_BYTE,nullptr));
    OGL_CALL(glBindVertexArray(0));
}
