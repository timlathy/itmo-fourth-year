#include "scene.hpp"

#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

void Scene::import_node(const aiNode* node, const aiScene* scene, TextureLoader& tex_loader, glm::mat4 acc_transform)
{
    /* IMPORTANT: when exporting an FBX model from Blender, use the following settings:
     * Apply Scalings: FBX All, Forward: -Z, Up: Y, Apply Unit, Apply Transformations */
    const aiMatrix4x4 t = node->mTransformation;
    const glm::mat4 transform =
        glm::mat4(t.a1, t.b1, t.c1, t.d1, t.a2, t.b2, t.c2, t.d2, t.a3, t.b3, t.c3, t.d3, t.a4, t.b4, t.c4, t.d4);
    acc_transform *= transform;

    const std::string name(node->mName.C_Str());

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::optional<GLuint> texture;

    if (node->mNumMeshes > 1)
        throw std::runtime_error("Unable to import node " + name + ": objects with multiple meshes are not supported.");

    if (node->mNumMeshes == 1)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
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
    }

    _models.emplace_back(name, vertices, indices, texture, acc_transform);

    for (int i = 0; i < node->mNumChildren; ++i)
    {
        import_node(node->mChildren[i], scene, tex_loader, acc_transform);
    }
}

Scene::Scene(const std::string& file, TextureLoader& tex_loader)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode)
        throw std::runtime_error("Unable to import scene from " + file + ": " + std::string(importer.GetErrorString()));

    import_node(scene->mRootNode, scene, tex_loader, glm::mat4(1.0f));

    /* IMPORTANT: when exporting an FBX model from Blender, use the following settings:
     * Bake Animation is On, NLA Strips is Off, Force S/E Keying is On, Sampling Rate is 1, Simplify is 0 */
    /* If you have parent-child relationships with animations, we don't handle that.
     * 3D View -> Object -> Animation -> Bake Action (check Visual Keying and Clear Parents) */
    for (int i = 0; i < scene->mNumAnimations; ++i)
    {
        const aiAnimation* a = scene->mAnimations[i];
        std::string animation_name(a->mName.C_Str());

        if (a->mNumChannels != 1)
            throw std::runtime_error("Unable to import animation " + animation_name + ": a single channel is required");

        const aiNodeAnim* channel = a->mChannels[0];

        // channel->mNodeName is useless in FBX models (at least the ones exported from Blender),
        // see https://blender.stackexchange.com/q/119664

        if (channel->mNumPositionKeys != channel->mNumRotationKeys ||
            channel->mNumPositionKeys != channel->mNumScalingKeys)
            throw std::runtime_error("Unable to import animation " + animation_name + ": T/R/S keys do not match");

        std::vector<AnimationFrame> frames;
        for (int i = 0; i < channel->mNumPositionKeys; ++i)
        {
            const auto p = channel->mPositionKeys[i].mValue;
            const auto r = channel->mRotationKeys[i].mValue.GetMatrix();
            const auto s = channel->mScalingKeys[i].mValue;

            const glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
            const glm::mat3 rot = glm::mat3(r.a1, r.b1, r.c1, r.a2, r.b2, r.c2, r.a3, r.b3, r.c3);
            const glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);

            glm::mat4 translation = glm::translate(pos);

            glm::mat4 rotation = glm::mat4(1.0f);
            rotation[0] = glm::vec4(rot[0], 0.0f);
            rotation[1] = glm::vec4(rot[1], 0.0f);
            rotation[2] = glm::vec4(rot[2], 0.0f);

            glm::mat4 scaling = glm::scale(scale);

            glm::mat4 trs = translation * rotation * scaling;
            glm::mat4 normal = Model::normal_transformation(trs);

            frames.push_back({trs, normal});
        }

        _animations.emplace(animation_name, frames);
    }
}

void Scene::instantiate_meshes()
{
    for (auto& m : _models)
        m.instantiate();
}

const Model& Scene::operator[](const std::string& name) const
{
    auto model = std::find_if(_models.begin(), _models.end(), [name](const auto& m) { return m.name() == name; });
    if (model == _models.end())
        throw std::runtime_error("Unable to find " + name + " in the scene");
    return *model;
}
