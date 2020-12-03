#pragma once

#include <string>

#include "mesh.hpp"
#include "texloader.hpp"

#include <assimp/scene.h>

class Scene
{
  private:
    std::vector<Mesh> _meshes;
    glm::vec3 _camera_position;
    glm::vec3 _light_source;

    void import_node(const aiNode* node, const aiScene* scene, TextureLoader& tex_loader, glm::mat4 acc_transform);

  public:
    Scene(const std::string& file, TextureLoader& tex_loader);
    void instantiate_meshes();
    const std::vector<Mesh> meshes() const
    {
        return _meshes;
    }
    const glm::vec3& camera_position() const
    {
        return _camera_position;
    }
    const glm::vec3& light_source() const
    {
        return _light_source;
    }
};
