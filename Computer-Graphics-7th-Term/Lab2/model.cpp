#include "model.hpp"

#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <iostream>

void Model::import_node(const aiNode* node, const aiScene* scene, TextureLoader& tex_loader, glm::mat4 acc_transform)
{
    /* IMPORTANT: when exporting an FBX model from Blender, use the following settings:
     * Apply Scalings: FBX All, Forward: -Z, Up: Y, Apply Unit, Apply Transformations */
    const aiMatrix4x4 t = node->mTransformation;
    const glm::mat4 transform =
        glm::mat4(t.a1, t.b1, t.c1, t.d1, t.a2, t.b2, t.c2, t.d2, t.a3, t.b3, t.c3, t.d3, t.a4, t.b4, t.c4, t.d4);
    acc_transform *= transform;

    std::cout << "Importing node " << node->mName.C_Str() << std::endl;

    // Hack: assume that a node with no meshes and no children is the light source
    // (the proper way would be to use custom properties I guess)
    if (node->mNumMeshes == 0 && node->mNumChildren == 0)
    {
        _light_source = acc_transform[3]; // extract translation vector from transformation matrix
    }

    for (int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::optional<GLuint> texture;

            for (int i = 0; i < mesh->mNumVertices; ++i)
            {
                const aiVector3D ai_pos = mesh->mVertices[i];
                glm::vec3 position(ai_pos.x, ai_pos.y, ai_pos.z);
                glm::vec3 normal{0, 0, 0};
                glm::vec2 uv{0, 0};
                if (mesh->HasNormals())
                {
                    const aiVector3D ai_norm = mesh->mNormals[i];
                    normal = glm::vec3(ai_norm.x, ai_norm.y, ai_norm.z);
                }
                if (auto first_texture{mesh->mTextureCoords[0]})
                {
                    uv = glm::vec2(first_texture[i].x, first_texture[i].y);
                }
                vertices.emplace_back(position, normal, uv);
            }

            for (int i = 0; i < mesh->mNumFaces; ++i)
            {
                const aiFace face = mesh->mFaces[i];
                for (int j = 0; j < face.mNumIndices; ++j)
                    indices.push_back(face.mIndices[j]);
            }

            if (mesh->mMaterialIndex >= 0)
            {
                const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                const auto tex_count = material->GetTextureCount(aiTextureType_DIFFUSE);
                for (int i = 0; i < tex_count; ++i)
                {
                    aiString path;
                    material->GetTexture(aiTextureType_DIFFUSE, i, &path);
                    texture = tex_loader.load_texture(path.C_Str());
                }
            }
            _meshes.emplace_back(vertices, indices, texture, acc_transform);
        }
    }
    for (int i = 0; i < node->mNumChildren; ++i)
    {
        import_node(node->mChildren[i], scene, tex_loader, acc_transform);
    }
}

Model::Model(const std::string& file, TextureLoader& tex_loader)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode)
        throw std::runtime_error("Unable to import model from " + file + ": " + std::string(importer.GetErrorString()));

    import_node(scene->mRootNode, scene, tex_loader, glm::mat4(1.0f));
}

void Model::instantiate_meshes()
{
    for (auto& m : _meshes)
        m.instantiate();
}
