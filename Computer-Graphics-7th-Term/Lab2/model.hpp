#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <optional>
#include <string>
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

struct AnimationFrame
{
    glm::mat4 transformation;
    glm::mat4 normal_transformation;
};

class Model
{
  private:
    std::string _name;
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::optional<GLuint> _texture;
    glm::mat4 _transformation;
    glm::mat4 _normal_transformation;

    std::optional<std::pair<int, AnimationFrame>> _animation_frame;

    union {
        struct
        {
            GLuint _vbo, _ebo;
        };
        GLuint _vertex_buffers[2];
    };
    GLuint _vao;

  public:
    Model(
        std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices,
        std::optional<GLuint> texture, glm::mat4 transform)
        : _name(name), _vertices(vertices), _indices(indices), _texture(texture), _transformation(transform)
    {
        _normal_transformation = normal_transformation(transform);
    }
    void instantiate();
    void draw() const;

    const std::string& name() const
    {
        return _name;
    }
    bool empty() const
    {
        return _vertices.empty();
    }
    const glm::mat4& transform() const
    {
        if (_animation_frame)
            return _animation_frame->second.transformation;

        return _transformation;
    }
    const glm::mat4& normal_transformation() const
    {
        if (_animation_frame)
            return _animation_frame->second.normal_transformation;

        return _normal_transformation;
    }
    glm::vec3 position() const
    {
        return transform()[3];
    }
    const std::vector<Vertex>& vertices() const
    {
        return _vertices;
    }

    bool play_animation(const std::vector<AnimationFrame>& frames)
    {
        int frame = _animation_frame ? _animation_frame->first : -1;
        if (++frame >= frames.size())
        {
            return false;
        }
        _animation_frame = {frame, frames[frame]};
        return true;
    }

    static glm::mat4 normal_transformation(glm::mat4 transformation)
    {
        return glm::transpose(glm::inverse(transformation));
    }
};
