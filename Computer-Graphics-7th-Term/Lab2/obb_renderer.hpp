#pragma once

#include <GL/glew.h>

#include "glprogram.hpp"
#include "obbcd.hpp"
#include "scene.hpp"

class OBBRenderer
{
  private:
    GlProgram _program;
    GLuint _vao, _vbo;

  public:
    OBBRenderer();
    void draw(const OBBCollisionDetection& obbcd, const glm::mat4& camera_vp);
};
