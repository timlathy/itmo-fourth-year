#pragma once

#include <GL/glew.h>

#include <string>
#include <vector>

class GlProgram
{
  private:
    GLuint _program;

  public:
    GlProgram(std::vector<std::pair<std::string, GLenum>> shader_paths);
    GLint uniform_location(const char* name) const
    {
        return glGetUniformLocation(_program, name);
    }
    void use() const
    {
        glUseProgram(_program);
    }
};
