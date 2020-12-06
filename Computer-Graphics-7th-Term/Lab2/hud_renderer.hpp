#pragma once

#include <GL/glew.h>

#include "glprogram.hpp"
#include "texloader.hpp"

class HUDRenderer
{
  private:
    GlProgram _program;
    GLuint _vao, _vbo;

  public:
    HUDRenderer();
    void draw(GLint texture);
};
