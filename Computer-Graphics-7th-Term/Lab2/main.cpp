#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "glutils.hpp"

const int width = 1600;
const int height = 1200;

static const char* vertex_source = R"glsl(
    #version 450 core

    in vec3 position;
    uniform mat4 mvp;

    out VS_FS_INTERFACE { vec3 in_pos; } vertex;

    void main()
    {
        gl_Position = mvp * vec4(position, 1.0);
        vertex.in_pos = position;
    }
)glsl";

static const char* fragment_source = R"glsl(
    #version 450 core

    in VS_FS_INTERFACE { vec3 in_pos; } vertex;

    out layout (location = 0) vec4 out_color;

    void main()
    {
        out_color = vec4(clamp(vertex.in_pos, 0.0, 1.0), 1.0);
    }
)glsl";

// 3 vertices per triangle, 12 triangles
static const GLfloat cube_vertex_buffer_data[] = {
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
    1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,  -1.0f, 1.0f};

int main()
{
    if (glfwInit() != GLFW_TRUE)
        return -1;

    // Use OpenGL 4.5 (forward compatibility removes deprecated functionality)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Computer Graphics Lab 2", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; // use GL 3.2+ core context
    if (auto err{glewInit()}; err != GLEW_OK)
    {
        std::cerr << "glewInit() error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    if (!compile_shader(vertex_shader, vertex_source))
        return 1;

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compile_shader(fragment_shader, fragment_source))
        return 1;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex_buffer_data), cube_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLint pos_attr = glGetAttribLocation(program, "position");
    glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attr);

    glm::mat4 model = glm::mat4(1.0f); // model to world coordinates
    glm::mat4 view = glm::lookAt(
        glm::vec3(4.0f, 3.0f, 3.0f), // camera position in world coordinates
        glm::vec3(0.0f, 0.0f, 0.0f), // camera is centered at this point
        glm::vec3(0.0f, 0.0f, 1.0f)  // Z is the vertical axis (X and Y are horizontal)
    );
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),          // FOV
        (float)width / (float)height, // aspect ratio
        0.1f,                         // near plane (a vertex that is closer to camera is clipped)
        100.0f                        // far plane (a vertex that is further from camera is clipped)
    );
    glm::mat4 mvp = projection * view * model;

    GLint mvp_uniform = glGetUniformLocation(program, "mvp");

    while (!glfwWindowShouldClose(window))
    {
        glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
