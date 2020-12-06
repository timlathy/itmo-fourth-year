#include "obb_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

static const GLfloat _unit_cube_vertices[] = {
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
    1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,  -1.0f, 1.0f};

OBBRenderer::OBBRenderer()
    : _program({{"../shader/obb.vert", GL_VERTEX_SHADER}, {"../shader/obb.frag", GL_FRAGMENT_SHADER}})
{
    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_unit_cube_vertices), _unit_cube_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); /* position */
    glVertexAttribPointer(0, 3 /* vec3 */, GL_FLOAT, GL_FALSE /* not normalized */, 0 /* stride */, 0 /* offset */);

    glBindVertexArray(0);
}

glm::mat4 bounding_box_transformation_matrix(const OrientedBoundingBox& bbox)
{
    glm::mat4 translation = glm::translate(bbox.position);

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4(bbox.rotation_x, 0.0f);
    rotation[1] = glm::vec4(bbox.rotation_y, 0.0f);
    rotation[2] = glm::vec4(bbox.rotation_z, 0.0f);

    glm::mat4 scaling = glm::scale(bbox.half_size);

    glm::mat4 model = translation * rotation * scaling;
    return model;
}

void OBBRenderer::draw(const OBBCollisionDetection& obbcd, const glm::mat4& camera_vp)
{
    _program.use();
    _program.set_uniform("vp", camera_vp);

    glBindVertexArray(_vao);

    for (const auto& bbox : obbcd.bounding_boxes())
    {
        _program.set_uniform("model", bounding_box_transformation_matrix(bbox));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    }

    _program.set_uniform("model", bounding_box_transformation_matrix(obbcd.observer_box()));
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    glBindVertexArray(0);
}
