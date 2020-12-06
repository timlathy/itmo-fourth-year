#pragma once

#include "model.hpp"
#include "texloader.hpp"

#include <assimp/scene.h>

#include <string>
#include <unordered_map>
#include <stdexcept>

class Scene
{
  private:
    std::vector<Model> _models;
    std::unordered_map<std::string, std::vector<AnimationFrame>> _animations;

    void import_node(const aiNode* node, const aiScene* scene, TextureLoader& tex_loader, glm::mat4 acc_transform);

  public:
    Scene(const std::string& file, TextureLoader& tex_loader);
    void instantiate_meshes();

    const Model& operator[](const std::string& name) const;
    const std::vector<Model>& models() const
    {
        return _models;
    }
    std::vector<Model>& models()
    {
        return _models;
    }
    const std::vector<AnimationFrame>& animation_keys(const std::string& animation_name) const
    {
        if (auto it{_animations.find(animation_name)}; it != _animations.end())
            return it->second;
        throw std::runtime_error("Animation" + animation_name + " does not exist in the scene");
    }
};
