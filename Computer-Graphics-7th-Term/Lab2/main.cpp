#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#include "camera.hpp"
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

std::unique_ptr<Camera> camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->on_mouse_movement(xpos, ypos);
}

// Process keys that don't depend on the press duration
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }
    }
}

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

    camera = std::make_unique<Camera>(width, height);

    glm::mat4 model = glm::mat4(1.0f); // model to world coordinates

    GLint mvp_uniform = glGetUniformLocation(program, "mvp");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = camera->vp_matrix() * model;
        glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glm::vec3 movement{0,0,0};
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            movement += CAMERA_FWD;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            movement -= CAMERA_FWD;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            movement += CAMERA_RIGHT;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            movement -= CAMERA_RIGHT;
        if (glm::length(movement) > 0.0f)
            camera->on_key_movement(movement);
    }

    glfwTerminate();
}
