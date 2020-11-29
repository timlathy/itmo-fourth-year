#pragma once

#include <GL/glew.h>

#include <string>
#include <unordered_map>

class TextureLoader
{
  private:
    std::string _data_dir;
    std::unordered_map<std::string, GLuint> _textures;

  public:
    TextureLoader(std::string data_dir) : _data_dir(data_dir)
    {
    }
    GLuint load_texture(std::string path);
};
