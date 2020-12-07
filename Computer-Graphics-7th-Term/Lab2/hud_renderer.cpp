#include "hud_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

struct HudVertex
{
    glm::vec3 position;
    glm::vec2 uv;
};

static const HudVertex _2d_plane_vertices[] = {
    {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // top left
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // top right
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // bottom left
    {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}  // bottom right
};

HUDRenderer::HUDRenderer()
    : _program({{"../shader/hud.vert", GL_VERTEX_SHADER}, {"../shader/hud.frag", GL_FRAGMENT_SHADER}})
{
    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_2d_plane_vertices), _2d_plane_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); /* position */
    glVertexAttribPointer(
        0, 3 /* vec3 */, GL_FLOAT, GL_FALSE /* not normalized */, sizeof(HudVertex) /* stride */,
        (void*)offsetof(HudVertex, position) /* offset */);

    glEnableVertexAttribArray(1); /* uv */
    glVertexAttribPointer(
        1, 2 /* vec2 */, GL_FLOAT, GL_FALSE /* not normalized */, sizeof(HudVertex) /* stride */,
        (void*)offsetof(HudVertex, uv) /* offset */);

    glBindVertexArray(0);
}

void HUDRenderer::draw(GLint texture)
{
    auto proj = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

    glm::mat4 model = glm::scale(glm::vec3(1, 1.0f / 16.0f, 1)); /* hardcoded texture aspect ratio */
    model = glm::scale(model, glm::vec3(0.5f));
    model = glm::translate(model, glm::vec3(0.5f, 1.0f, 0));

    _program.use();
    _program.set_uniform("mvp", proj * model);

    glBindVertexArray(_vao);

    glBindTexture(GL_TEXTURE_2D, texture);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);

    glBindVertexArray(0);
}
