#include "texloader.hpp"

#include <spng.h>

#include <stdexcept>
#include <cmath>
#include <vector>

GLuint TextureLoader::load_texture(std::string path)
{
    if (auto loaded{_textures.find(path)}; loaded != _textures.end())
        return loaded->second;

    FILE* png_file = fopen((_data_dir + "/" + path).c_str(), "rb");
    if (!png_file)
        throw std::runtime_error(path + ": unable to open texture file");

    spng_ctx* ctx = spng_ctx_new(0);
    if (int error{spng_set_png_file(ctx, png_file)}; error != 0)
        throw std::runtime_error(path + ": spng_set_png_file failed, " + std::string(spng_strerror(error)));

    spng_ihdr header;
    if (int error{spng_get_ihdr(ctx, &header)}; error != 0)
        throw std::runtime_error(path + ": spng_get_ihdr failed, " + std::string(spng_strerror(error)));

    size_t size;
    if (int error{spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &size)}; error != 0)
        throw std::runtime_error(path + ": spng_decoded_image_size failed, " + std::string(spng_strerror(error)));

    std::vector<char> tex_data(size);
    if (int error{spng_decode_image(ctx, tex_data.data(), size, SPNG_FMT_RGBA8, 0)}; error != 0)
        throw std::runtime_error(path + ": spng_decode_image failed, " + std::string(spng_strerror(error)));

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto mipmap_levels = std::max(1, (int)std::floor(std::log2(std::max(header.width, header.height))));

    glTextureStorage2D(texture, mipmap_levels, GL_RGBA8, header.width, header.height);
    glTextureSubImage2D(texture, 0, 0, 0, header.width,header.height, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data());
    glGenerateTextureMipmap(texture);

    spng_ctx_free(ctx);

    _textures.emplace(path, texture);

    return texture;
}
