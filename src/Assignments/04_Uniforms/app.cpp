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
                    {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
                    {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
            });


    if (!program) {
        SPDLOG_CRITICAL("Invalid program");
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the triangle.
        std::vector<GLfloat> vertices = {
    
        -0.5f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 
        0.5f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 
        0.0f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  
        
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   //3
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   //4 
        -0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,   //5

        //duplicate
        //0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //4
        //duplicate
        //-0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, //5
        0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f   //6
    };
// 1. First, we will not remove the vertices, we will only add an index buffer. To do this, you need to declare a new
//    vector `vector<GLubyte>` which will contain the indexes. Since we are not changing anything in the vertex
//    buffer for now, we fill it with integers from `0` to `number of vertices-1`.
    std::vector<GLubyte> indices = {
        0,1,2,3,4,5,4,5,6
    };


    /*
     * All the calls to the OpenGL API are "encapsulated" in the OGL_CALL macro for debugging purposes as explained in
     * Assignments/DEBUGGING.md. The macro is defined in src/Application/utils.h. If the call to the OpenGL API returns an
     * error code, the macro will print the name of the function that failed, the file and line number where the error
     * occurred.
     */

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    OGL_CALL(glCreateBuffers(1, &v_buffer_handle));
    OGL_CALL(glNamedBufferData(v_buffer_handle, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW));

    //indices
    GLuint i_buffer_handle;
    OGL_CALL(glCreateBuffers(1, &i_buffer_handle));
    OGL_CALL(glNamedBufferData(i_buffer_handle, indices.size() * sizeof(GLubyte), indices.data(), GL_STATIC_DRAW));
        
    // This sets up a Vertex Array Object (VAO) that encapsulates
    // the state of all vertex buffers needed for rendering.
    // The vao_ variable is a member of the SimpleShapeApplication class and is defined in src/Application/app.h.
    OGL_CALL(glGenVertexArrays(1, &vao_));
    OGL_CALL(glBindVertexArray(vao_));
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));

    //indices
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
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not using white or black for better debugging.
    OGL_CALL(glClearColor(0.1f, 0.75f, 0.75f, 1.0f));

    // This set up an OpenGL viewport of the size of the whole rendering window.
    auto [w, h] = frame_buffer_size();
    OGL_CALL(glViewport(0, 0, w, h));

    OGL_CALL(glUseProgram(program));

    GLuint mixerBuffer;

    glGenBuffers(1, &mixerBuffer);

    glBindBuffer(GL_UNIFORM_BUFFER, mixerBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);

    float strength = 0.5;
    float mix_color[3] = {0.0, 0.0, 1.0}; 

  
    float data[8] = { strength, 0.0, 0.0, 0.0, 
                    mix_color[0], mix_color[1], mix_color[2], 0.0 }; 
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), data);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mixerBuffer);

    float theta = glm::pi<float>() / 6.0f; // 30 degrees
    float cs = std::cos(theta);
    float ss = std::sin(theta);
    glm::mat2 rot{cs, ss, -ss, cs};
    glm::vec2 trans{0.0, -0.25};
    glm::vec2 scale{1, 0.5};


    GLuint transformBuffer;
    glGenBuffers(1, &transformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, transformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 64, nullptr, GL_DYNAMIC_DRAW);

    float transformData[16] = {
        scale.x, scale.y, 0.0f, 0.0f,            
        trans.x, trans.y, 0.0f, 0.0f,           
        rot[0][0], rot[1][1], 0.0f, 0.0f,        
        rot[1][0], rot[0][0], 0.0f, 0.0f       
    };

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(transformData), transformData);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformBuffer);


}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will set up all the required vertex attribute arrays.
    OGL_CALL(glBindVertexArray(vao_));
        //replece this
    //OGL_CALL(glDrawArrays(GL_TRIANGLES, 0, 9));  
        //to this
    OGL_CALL(glDrawElements(GL_TRIANGLES, 9,GL_UNSIGNED_BYTE,nullptr));
    OGL_CALL(glBindVertexArray(0));
}
