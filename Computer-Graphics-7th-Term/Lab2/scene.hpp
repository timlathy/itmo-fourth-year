#pragma once

#include <string>

#include "model.hpp"
#include "texloader.hpp"

#include <assimp/scene.h>

class Scene
{
  private:
    std::vector<Model> _models;
    std::vector<Animation> _animations;

    void import_node(const aiNode* node, const aiScene* scene, TextureLoader& tex_loader, glm::mat4 acc_transform);

  public:
    Scene(const std::string& file, TextureLoader& tex_loader);
    void instantiate_meshes();

    const Model& operator[](const std::string& name) const;
    const std::vector<Model> models() const
    {
        return _models;
    }
};
