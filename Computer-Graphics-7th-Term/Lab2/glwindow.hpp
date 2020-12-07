#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GlWindow
{
  private:
    GLFWwindow* _window;

  public:
    GlWindow(int width, int height);
    ~GlWindow()
    {
        glfwTerminate();
    }
    void setup_event_loop();
    void submit_frame();
    bool key_pressed(int key) const
    {
        return glfwGetKey(_window, key) == GLFW_PRESS;
    }
    bool closed() const
    {
        return glfwWindowShouldClose(_window);
    }
    void cursor_position(double* mouse_x, double* mouse_y) const
    {
        glfwGetCursorPos(_window, mouse_x, mouse_y);
    }
};
