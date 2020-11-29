#include "texloader.hpp"

#include <spng.h>

#include <stdexcept>
#include <vector>

GLuint TextureLoader::load_texture(std::string path)
{
    if (auto loaded{_textures.find(path)}; loaded != _textures.end())
        return loaded->second;

    // FIXME: assumes that the cwd is build/
    FILE* png_file = fopen(("../" + path).c_str(), "rb");
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
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header.width, header.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    spng_ctx_free(ctx);

    _textures.emplace(path, texture);

    return texture;
}
