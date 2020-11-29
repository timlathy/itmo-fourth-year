#include "glutils.hpp"

#include <iostream>
#include <vector>

void compile_shader(GLuint handle, const char* source)
{
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);

    GLint log_size;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 0)
    {
        std::vector<GLchar> message(log_size);
        glGetShaderInfoLog(handle, log_size, nullptr, message.data());

        std::cerr << "compile_shader: " << message.data() << std::endl;
    }

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
        throw std::runtime_error("compile_shader failed");
}

void link_program(GLuint program)
{
    glLinkProgram(program);

    GLint log_size;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 0)
    {
        std::vector<GLchar> message(log_size);
        glGetProgramInfoLog(program, log_size, nullptr, message.data());

        std::cerr << "link_program: " << message.data() << std::endl;
    }

    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (is_linked == GL_FALSE)
        throw std::runtime_error("link_program failed");
}
