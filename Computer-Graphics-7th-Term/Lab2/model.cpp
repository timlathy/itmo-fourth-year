#include "model.hpp"

#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

void import_node(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, TextureLoader& tex_loader)
{
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
                for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
                {
                    aiString path;
                    material->GetTexture(aiTextureType_DIFFUSE, i, &path);
                    texture = tex_loader.load_texture(path.C_Str());
                }
            }

            meshes.emplace_back(vertices, indices, texture);
        }
    }
    for (int i = 0; i < node->mNumChildren; ++i)
    {
        import_node(node->mChildren[i], scene, meshes, tex_loader);
    }
}

Model::Model(const std::string& file, TextureLoader& tex_loader)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode)
        throw std::runtime_error("Unable to import model from " + file + ": " + std::string(importer.GetErrorString()));

    import_node(scene->mRootNode, scene, _meshes, tex_loader);
}

void Model::instantiate_meshes()
{
    for (auto& m : _meshes)
        m.instantiate();
}

void Model::draw_meshes() const
{
    for (const auto& m : _meshes)
        m.draw();
}
