#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "itemBuffer.hpp"
#include "shader.hpp"

class Entity : public ItemBuffer
{
private:
    ItemBuffer *m_buffer;

    unsigned int m_boundary;
    glm::vec3 m_origin;
    glm::vec3 m_translationAxis;
    glm::vec3 m_rotationAxis;
    glm::mat4 m_model;
    
public:
    Entity(ItemBuffer *buffer);
    ~Entity() = default;

    void Draw(int count);
    void Paint();
    void MoveOrigin(glm::mat4 &model);
    void Translate(const glm::vec3 &direction);
    void Rotate(float angle, const glm::vec3 &axis);
    void Scale(const glm::vec3 &factor);

    unsigned int GetBoundary() const
    {
        return m_boundary;
    }
    const glm::mat4 &GetModelMat() const
    {
        return m_model;
    }
    void ResetModel()
    {
        m_model = glm::mat4(1.0f);
    }

};


#endif /* ENTITY_HPP */