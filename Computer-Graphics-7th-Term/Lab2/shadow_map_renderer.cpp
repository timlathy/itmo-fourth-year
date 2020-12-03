#include "shadow_map_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

const float MAP_WIDTH = 1024;
const float MAP_HEIGHT = 1024;

ShadowMapRenderer::ShadowMapRenderer(std::vector<LightSource> light_sources)
    : _depth_textures(light_sources.size()),
      _program({{"../shader/shadow_map.vert", GL_VERTEX_SHADER}, {"../shader/shadow_map.frag", GL_FRAGMENT_SHADER}})
{
    glGenFramebuffers(1, &_fbo);
    glGenTextures(_depth_textures.size(), _depth_textures.data());
    for (const GLint texture : _depth_textures)
    {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, MAP_WIDTH, MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    }
    for (const auto& source : light_sources)
    {
        const float near_plane = 1.0f;
        const float far_plane = 100.0f;

        const glm::mat4 view = glm::lookAt(source.position, glm::vec3(0.0f, 0.0f, 0.0f), CAMERA_UP);
        const glm::mat4 vp = source.projection * view;

        _shadow_vp_matrices.push_back(vp);

        // Divide the coordinates by two ([-1;1] -> [-0.5;0.5]) and add 0.5 to get texture VP matrix
        const glm::mat4 transform(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
        const glm::mat4 tex_vp = transform * vp;

        _shadow_tex_vp_matrices.push_back(tex_vp);
    }
}

void ShadowMapRenderer::draw(const Scene& scene)
{
    glViewport(0, 0, MAP_WIDTH, MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    _program.use();

    glCullFace(GL_FRONT);

    for (int i = 0; i < _depth_textures.size(); ++i)
    {
        glBindTexture(GL_TEXTURE_2D, _depth_textures[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth_textures[i], 0);
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
