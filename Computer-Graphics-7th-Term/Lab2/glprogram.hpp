#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

class GlProgram
{
  private:
    GLuint _program;

  public:
    GlProgram(std::vector<std::pair<std::string, GLenum>> shader_paths);
    void set_uniform(const char* name, const glm::vec3& value) const;
    void set_uniform(const char* name, const glm::mat4& value) const;
    void use() const
    {
        glUseProgram(_program);
    }
};
