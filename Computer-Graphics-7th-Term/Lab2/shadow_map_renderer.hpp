#pragma once

#include <GL/glew.h>

#include "glprogram.hpp"
#include "scene.hpp"

struct LightSource
{
    glm::vec3 position;
    glm::mat4 projection;
};

class ShadowMapRenderer
{
  private:
    GlProgram _program;
    GLuint _fbo;
    std::vector<GLuint> _depth_textures;
    std::vector<glm::mat4> _shadow_vp_matrices;
    std::vector<glm::mat4> _shadow_tex_vp_matrices;

  public:
    ShadowMapRenderer(std::vector<LightSource> light_sources);
    void draw(const Scene& scene);
    const std::vector<glm::mat4>& shadow_tex_vp_matrices()
    {
        return _shadow_tex_vp_matrices;
    }
    const std::vector<GLuint>& textures() const
    {
        return _depth_textures;
    }
};
