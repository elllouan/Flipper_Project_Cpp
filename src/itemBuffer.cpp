#include <utility>
#include <iostream>

#include <glad/glad.h>

#include "stb_image.h"
#include "itemBuffer.hpp"

// @brief Generates, binds and fills data buffers.
// @param vertexBuffer Pointer to the Vertex Buffer Array (VBO).
// @param sizeBuffer In bytes, the size of the vertexBuffer.
// @param indices Pointer to the Element Buffer Array (EBO).
// @param sizeIndices In bytes, the size of indices.
// @note An EBO can be added (optional).
ItemBuffer::ItemBuffer(void *vertexBuffer, int sizeBuffer, unsigned int *indices, int sizeIndices)
{
    glGenVertexArrays(1, &m_VA0);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VA0);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeBuffer, vertexBuffer, GL_STATIC_DRAW);

    if (indices)
    {
        glGenBuffers(1, &m_EB0);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, vertexBuffer, GL_STATIC_DRAW);
    }

    m_buffer = vertexBuffer;
}

ItemBuffer::~ItemBuffer()
{
    glDeleteVertexArrays(1, &m_VA0);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EB0);
}

void ItemBuffer::AddVertexAttrib(unsigned int index, unsigned int count, unsigned int stride, unsigned int offset)
{
    glVertexAttribPointer(index, count, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    glEnableVertexAttribArray(index);
}

void ItemBuffer::AddPositionAttrib(int stride)
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);
}

void ItemBuffer::AddColorAttrib(int stride)
{
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void ItemBuffer::AddTextureAttrib(int stride)
{
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(5*sizeof(float)));
    glEnableVertexAttribArray(2);
}

void ItemBuffer::AddTexture2D(unsigned int &id, const std::string &img, int wrappingParam, int filteringParam)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    m_textures.emplace_back(id);

    // Wrapping methods
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingParam);
    // Filtering methods
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringParam);

    int width, height, nrChannels;
    std::string path = "C:\\Users\\Elouan THEOT\\Documents\\Programming\\c++\\Flipper_Project_Cpp\\img\\" + img;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to generate a 2D texture image.\n";
    }
    stbi_image_free(data);
}

void ItemBuffer::BindTextures()
{
    for (int i=0; i<m_textures.size() && i<MAX_ACTIVE_TEXTURE; i++)
    {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, m_textures.at(i));
    }
    
}

// @brief Binds the Vertex Array which vertices attributes are to be drawn.
// @note Rely exclusively on glBindVertexArray().
void ItemBuffer::Bind()
{
    glBindVertexArray(m_VA0);
}
