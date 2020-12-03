#include "glprogram.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <streambuf>

GlProgram::GlProgram(std::vector<std::pair<std::string, GLenum>> shader_paths)
{
    _program = glCreateProgram();

    for (const auto& [path, type] : shader_paths)
    {
        std::ifstream src_file(path);
        if (!src_file)
            throw std::runtime_error("Unable to open shader source file " + path);

        std::string src((std::istreambuf_iterator<char>(src_file)), std::istreambuf_iterator<char>());

        GLuint shader = glCreateShader(type);

        const GLchar* gl_src = src.data();
        glShaderSource(shader, 1, &gl_src, nullptr);
        glCompileShader(shader);

        GLint log_size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
        if (log_size > 0)
        {
            std::vector<GLchar> message(log_size);
            glGetShaderInfoLog(shader, log_size, nullptr, message.data());
            std::cerr << "Warning for glCompileShader(" << path << "): " << message.data() << std::endl;
        }

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
            throw std::runtime_error("Compilation failed for shader " + path);

        glAttachShader(_program, shader);
    }

    glLinkProgram(_program);

    GLint log_size;
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 0)
    {
        std::vector<GLchar> message(log_size);
        glGetProgramInfoLog(_program, log_size, nullptr, message.data());

        std::cerr << "Warning for glLinkProgram: " << message.data() << std::endl;
    }

    GLint is_linked = 0;
    glGetProgramiv(_program, GL_LINK_STATUS, &is_linked);
    if (is_linked == GL_FALSE)
        throw std::runtime_error("glLinkProgram failed");
}

void GlProgram::set_uniform(const char* name, const glm::vec3& value) const
{
    const GLint location = glGetUniformLocation(_program, name);
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void GlProgram::set_uniform(const char* name, const glm::mat4& value) const
{
    const GLint location = glGetUniformLocation(_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
