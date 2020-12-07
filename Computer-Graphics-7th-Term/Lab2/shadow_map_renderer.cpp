#include "shadow_map_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

ShadowMapRenderer::ShadowMapRenderer(std::vector<LightSource> light_sources)
    : _depth_textures(light_sources.size()), _texture_resolution(light_sources.size()),
      _shadow_vp_matrices(light_sources.size()), _shadow_tex_vp_matrices(light_sources.size()),
      _program({{"../shader/shadow_map.vert", GL_VERTEX_SHADER}, {"../shader/shadow_map.frag", GL_FRAGMENT_SHADER}})
{
    glGenFramebuffers(1, &_fbo);

    glCreateTextures(GL_TEXTURE_2D, _depth_textures.size(), _depth_textures.data());
    for (int i = 0; i < light_sources.size(); ++i)
    {
        const auto& source = light_sources[i];
        const GLint texture = _depth_textures[i];

        glTextureStorage2D(texture, 1, GL_DEPTH_COMPONENT16, source.depth_map_resolution, source.depth_map_resolution);

        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTextureParameteri(texture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

        _texture_resolution[i] = source.depth_map_resolution;
        _shadow_vp_matrices[i] = source.vp;

        // Divide the coordinates by two ([-1;1] -> [-0.5;0.5]) and add 0.5 to get texture VP matrix
        const glm::mat4 transform(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
        const glm::mat4 tex_vp = transform * source.vp;

        _shadow_tex_vp_matrices[i] = tex_vp;
    }
}

void ShadowMapRenderer::draw(const Scene& scene)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    _program.use();

    glCullFace(GL_FRONT);

    for (int i = 0; i < _depth_textures.size(); ++i)
    {
        glViewport(0, 0, _texture_resolution[i], _texture_resolution[i]);

        glBindTextureUnit(0, _depth_textures[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth_textures[i], 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawBuffer(GL_NONE); // no color buffer

        for (const auto& m : scene.models())
        {
            const glm::mat4 mvp = _shadow_vp_matrices[i] * m.transform();
            _program.set_uniform("mvp", mvp);

            m.draw();
        }
    }

    glCullFace(GL_BACK);
}
