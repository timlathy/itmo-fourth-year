#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <optional>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) : position(position), normal(normal), uv(uv)
    {
    }
};

class Mesh
{
  private:
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::optional<GLuint> _texture;
    glm::mat4 _transform;
    glm::mat4 _normal_transform;

    union {
        struct
        {
            GLuint _vbo, _ebo;
        };
        GLuint _vertex_buffers[2];
    };
    GLuint _vao;

  public:
    Mesh(
        std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::optional<GLuint> texture,
        glm::mat4 transform)
        : _vertices(vertices), _indices(indices), _texture(texture), _transform(transform)
    {
        _normal_transform = glm::transpose(glm::inverse(transform));
    }
    void instantiate();
    void draw() const;
    const glm::mat4& transform() const
    {
        return _transform;
    }
    const glm::mat4& normal_transform() const
    {
        return _normal_transform;
    }
};
