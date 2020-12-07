#include "glwindow.hpp"

#include <chrono>
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
    _frame_start_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void GlWindow::begin_frame()
{
    const uint64_t current_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    _frame_time_acc += current_time - _frame_start_time;
    _frame_start_time = current_time;
}

bool GlWindow::animation_tick()
{
    const uint64_t t_per_frame = 1000 / 60; // 60 fps
    bool tick = _frame_time_acc > t_per_frame;
    if (tick)
        _frame_time_acc -= t_per_frame;
    return tick;
}

void GlWindow::submit_frame()
{
    if (key_pressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(_window, GL_TRUE);

    glfwSwapBuffers(_window);
    glfwPollEvents();
}
