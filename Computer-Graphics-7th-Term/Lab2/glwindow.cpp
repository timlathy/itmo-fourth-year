#include "glwindow.hpp"

#include <stdexcept>

GlWindow::GlWindow(int width, int height)
{
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("glfwInit() error");

    // Use OpenGL 4.5 (forward compatibility removes deprecated functionality)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _window = glfwCreateWindow(width, height, "Computer Graphics Lab 2", nullptr, nullptr);
    glfwMakeContextCurrent(_window);

    glewExperimental = GL_TRUE; // use GL 3.2+ core context
    if (auto err{glewInit()}; err != GLEW_OK)
        throw std::runtime_error(std::string("glewInit() error: ").append((const char*)glewGetErrorString(err)));
}

void GlWindow::setup_event_loop()
{
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GlWindow::submit_frame()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
    if (key_pressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(_window, GL_TRUE);
}
