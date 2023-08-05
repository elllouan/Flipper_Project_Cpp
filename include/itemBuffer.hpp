#ifndef ITEM_HPP
#define ITEM_HPP

#if WINDOWS_MSVC
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

class ItemBuffer
{
    static constexpr int MAX_ACTIVE_TEXTURE = 16;

private:
    unsigned int m_VA0;
    unsigned int m_EB0;
    unsigned int m_VBO;
    void *m_buffer;
    std::vector<unsigned int> m_textures;

public:
    ItemBuffer() {}
    ItemBuffer(void *vertexBuffer, int size, unsigned int *indices = nullptr, int sizeIndices = 0);
    ~ItemBuffer();

    void AddVertexAttrib(unsigned int index, unsigned int count, unsigned int stride, unsigned int offset);
    void AddPositionAttrib(int stride);
    void AddColorAttrib(int stride);
    void AddTextureAttrib(int stride);

    void AddTexture2D(unsigned int &id, const std::string &img, int wrappingParam = GL_REPEAT, int filteringParam = GL_LINEAR);
    void BindTextures();

    void Bind();
};


#endif /* ITEM_HPP */