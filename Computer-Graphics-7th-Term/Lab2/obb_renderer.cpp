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

static const glm::vec3 _filter_red = {0.8, 0.1, 0.1};
static const glm::vec3 _filter_green = {0.1, 0.8, 0.1};
static const glm::vec3 _filter_blue = {0.1, 0.1, 0.8};
static const glm::vec3 _filter_orange = {0.8, 0.4, 0.1};

void OBBRenderer::draw(const OBBCollisionDetection& obbcd, const glm::mat4& camera_vp)
{
    _program.use();
    _program.set_uniform("vp", camera_vp);

    glBindVertexArray(_vao);

    OrientedBoundingBox observer_collider = obbcd.observer_box();
    observer_collider.half_size += glm::vec3(0.1, 0.1, 0.1);

    for (const auto& bbox : obbcd.bounding_boxes())
    {
        glm::vec3 filter;
        if (OBBCollisionDetection::check_collision(bbox, observer_collider))
            filter = _filter_red;
        else
            filter = _filter_green;

        _program.set_uniform("color_filter", filter);
        _program.set_uniform("model", bounding_box_transformation_matrix(bbox));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    }

    for (const auto& bbox : obbcd.interaction_boxes())
    {
        glm::vec3 filter;
        if (OBBCollisionDetection::check_collision(bbox, observer_collider))
            filter = _filter_orange;
        else
            filter = _filter_blue;

        _program.set_uniform("color_filter", filter);
        _program.set_uniform("model", bounding_box_transformation_matrix(bbox));
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    }

    _program.set_uniform("model", bounding_box_transformation_matrix(obbcd.observer_box()));
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    glBindVertexArray(0);
}
