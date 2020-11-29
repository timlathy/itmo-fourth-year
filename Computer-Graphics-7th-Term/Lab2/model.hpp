#pragma once

#include <string>

#include "mesh.hpp"
#include "texloader.hpp"

class Model
{
  private:
    std::vector<Mesh> _meshes;

  public:
    Model(const std::string& file, TextureLoader& tex_loader);
    void instantiate_meshes();
    void draw_meshes() const;
};
