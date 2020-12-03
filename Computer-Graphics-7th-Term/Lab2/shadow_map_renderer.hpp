#pragma once

#include <GL/glew.h>

#include "glprogram.hpp"
#include "scene.hpp"

class ShadowMapRenderer
{
  private:
    GlProgram _program;
    GLuint _fbo, _depth_texture;

  public:
    ShadowMapRenderer();
    void draw(const Scene& scene, const glm::mat4& light_vp);
    const GLuint texture() const
    {
        return _depth_texture;
    }

    static glm::mat4 shadow_vp_matrix(const glm::vec3& light_position);
    static glm::mat4 vp_to_texcoords(const glm::mat4& light_vp);
};
