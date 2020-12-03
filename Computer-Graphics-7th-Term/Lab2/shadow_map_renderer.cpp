#include "shadow_map_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

const float MAP_WIDTH = 1024;
const float MAP_HEIGHT = 1024;

ShadowMapRenderer::ShadowMapRenderer()
    : _program({{"../shader/shadow_map.vert", GL_VERTEX_SHADER}, {"../shader/shadow_map.frag", GL_FRAGMENT_SHADER}})
{
    glGenFramebuffers(1, &_fbo);
    glGenTextures(1, &_depth_texture);
    glBindTexture(GL_TEXTURE_2D, _depth_texture);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, MAP_WIDTH, MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
}

void ShadowMapRenderer::draw(const Scene& scene, const glm::mat4& light_vp)
{
    glViewport(0, 0, MAP_WIDTH, MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, _depth_texture);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth_texture, 0);
    glDrawBuffer(GL_NONE); // no color buffer

    glCullFace(GL_FRONT);

    _program.use();

    for (const auto& m : scene.meshes())
    {
        const glm::mat4 mvp = light_vp * m.transform();
        _program.set_uniform("mvp", mvp);

        m.draw();
    }

    glCullFace(GL_BACK);
}

glm::mat4 ShadowMapRenderer::shadow_vp_matrix(const glm::vec3& light_position)
{
    const float near_plane = 1.0f;
    const float far_plane = 100.0f;

    const glm::mat4 proj = glm::ortho(-16.5f, 32.0f, -11.0f, 34.5f, near_plane, far_plane);
    const glm::mat4 view = glm::lookAt(light_position, glm::vec3(0.0f, 0.0f, 0.0f), CAMERA_UP);
    return proj * view;
}

glm::mat4 ShadowMapRenderer::vp_to_texcoords(const glm::mat4& light_vp)
{
    // Divide the coordinates by two ([-1;1] -> [-0.5;0.5]) and add 0.5
    const glm::mat4 transform(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
    return transform * light_vp;
}
