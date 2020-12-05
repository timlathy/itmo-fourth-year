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
    void set_uniform(const char* name, GLint value) const;

    void set_uniform_array(const char* name, const glm::mat4* values, size_t count) const;
    void set_uniform_array(const char* name, const glm::vec3* values, size_t count) const;
    void set_uniform_array(const char* name, const glm::vec2* values, size_t count) const;
    void set_uniform_array(const char* name, const GLint* values, size_t count) const;

    void use() const
    {
        glUseProgram(_program);
    }
};
