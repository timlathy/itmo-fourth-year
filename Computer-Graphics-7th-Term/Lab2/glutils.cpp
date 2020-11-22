#include "glutils.hpp"
#include <iostream>

bool compile_shader(GLuint handle, const char* source)
{
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);

    GLint log_size;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_size);
    if (log_size > 0)
    {
        GLchar* message = new char[log_size];
        glGetShaderInfoLog(handle, log_size, NULL, message);

        std::cerr << "compile_shader log: " << message << std::endl;
        delete[] message;
    }

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
}
