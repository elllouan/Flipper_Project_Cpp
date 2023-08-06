#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "itemBuffer.hpp"
#include "shader.hpp"

class Entity : public ItemBuffer
{
private:
    ItemBuffer *m_buffer;

    float m_boundary;
    glm::vec3 m_origin;
    glm::vec3 m_rotationAxis;
    float m_rotationAngle;
    glm::vec3 m_scaleFactor;
    glm::mat4 m_model;
    
public:
    // Entity(ItemBuffer *buffer, glm::vec3 &translationAxis = glm::vec3(0.0f), glm::vec3 &rotationAxis = glm::vec3(0.0f));
    Entity(ItemBuffer *buffer,
           float boundary,
           glm::vec3 &translationAxis = glm::vec3(0.0f),
           glm::vec3 &rotationAxis = glm::vec3(0.0f),
           float rotationAngle = 0.0f,
           glm::vec3 &scaleFactor = glm::vec3(1.0f));
    ~Entity() = default;

    float x;
    float y;
    float z;

    void Draw(int count);
    void Paint();
    void ChangeModel(const glm::mat4 &model);
    void UpdateModel(const glm::vec3 &translationAxis, const glm::vec3 &rotationAxis, float rotationAngle, const glm::vec3 &scaleFactor);
    void Translate(const glm::vec3 &direction);
    void Rotate(float angle, const glm::vec3 &axis);
    void Scale(const glm::vec3 &factor);
    
    bool IsReachable(double x_mouse, double y_mouse, float z_camera);

    void Expulse(float timeFrame, glm::vec3 direction);

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
        m_origin = glm::vec3(0.0f);
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        m_rotationAxis = glm::vec3(0.0f);
        m_rotationAngle = 0.0f;

        m_model = glm::mat4(1.0f);
    }

};


#endif /* ENTITY_HPP */