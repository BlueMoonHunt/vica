#include "image.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <print>

vica::Image::Image(const std::filesystem::path& path) :m_Path(path), m_Name(path.filename().string().c_str()) {
    int width, height, channels;
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data)
        std::println("Failed to load image {}, {}", path.string(), stbi_failure_reason());

    m_Width = width;
    m_Height = height;

    GLenum internalFormat = 0, dataFormat = 0;
    switch (channels) {
    case 4: internalFormat = GL_RGBA8; dataFormat = GL_RGBA; break;
    case 3: internalFormat = GL_RGB8;  dataFormat = GL_RGB;  break;
    }

    if (!internalFormat & dataFormat)
        std::println("Format not supported");

    m_InternalFormat = internalFormat;
    m_DataFormat = dataFormat;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_ImageID);
    glTextureStorage2D(m_ImageID, 1, internalFormat, m_Width, m_Height);

    glTextureParameteri(m_ImageID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_ImageID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(m_ImageID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_ImageID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(m_ImageID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
    //glGenerateTextureMipmap(m_ImageID);

    stbi_image_free(data);
}

vica::Image::Image(const char* name, void* data, const uint32_t size, const uint32_t width, const uint32_t height) : m_Name(name), m_Width(width), m_Height(height) {
    m_InternalFormat = GL_RGBA8;
    m_DataFormat = GL_RGBA;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_ImageID);
    glTextureStorage2D(m_ImageID, 1, m_InternalFormat, m_Width, m_Height);

    //glTextureParameteri(m_ImageID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_ImageID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_ImageID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(m_ImageID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_ImageID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (size != m_Width * m_Height * (m_DataFormat == GL_RGBA ? 4 : 3))
        std::println("Data size does not match image size");
    glTextureSubImage2D(m_ImageID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}


vica::Image::~Image() {
    glDeleteTextures(1, &m_ImageID);
}

bool vica::Image::operator==(const Image& other) const {
    return m_ImageID == ((Image&)other).m_ImageID;
}

void vica::Image::bind(uint32_t slot) const {
    glBindTextureUnit(slot, m_ImageID);
}
