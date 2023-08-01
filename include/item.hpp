#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>

class Item
{
    static constexpr int MAX_ACTIVE_TEXTURE = 16;

private:
    unsigned int m_VA0;
    unsigned int m_EB0;
    unsigned int m_VBO;
    void *m_buffer;
    std::vector<unsigned int> m_textures;

public:
    Item(void *vertexBuffer, int size, unsigned int *indices = nullptr, int sizeIndices = 0);
    ~Item();

    void EnableVertexAttrib(unsigned int index, unsigned int count, unsigned int stride, unsigned int offset);
    void EnablePositionAttrib(int stride);
    void EnableColorAttrib(int stride);
    void EnableTextureAttrib(int stride);

    void AddTexture2D(unsigned int &id, const std::string &img, int wrappingParam = GL_REPEAT, int filteringParam = GL_LINEAR);
    void BindTextures();

    void Bind();

    void Draw(int count);
    void Paint();
};


#endif /* ITEM_HPP */