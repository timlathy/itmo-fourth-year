#pragma once

#include <string>

#include "mesh.hpp"

class Model
{
  private:
    std::vector<Mesh> _meshes;

  public:
    Model(const std::string& file);
    void instantiate_meshes();
    void draw_meshes() const;
};
