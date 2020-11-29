#pragma once

#include <GL/glew.h>

#include <unordered_map>
#include <string>

class TextureLoader
{
  private:
    std::unordered_map<std::string, GLuint> _textures;

  public:
    GLuint load_texture(std::string path);
};
