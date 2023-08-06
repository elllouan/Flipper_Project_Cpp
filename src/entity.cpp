#include "entity.hpp"

Entity::Entity(ItemBuffer *buffer, glm::vec3 &translationAxis, glm::vec3 &rotationAxis, float rotationAngle, glm::vec3 &scaleFactor) : 
    m_buffer {buffer},
    m_origin {translationAxis},
    m_rotationAxis {rotationAxis},
    m_rotationAngle {rotationAngle},
    m_scaleFactor {scaleFactor}
{
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, m_origin);
    m_model = glm::rotate(m_model, glm::radians(rotationAngle), m_rotationAxis);
    m_model = glm::scale(m_model, m_scaleFactor);
    x = m_origin.x;
    y = m_origin.y;
    z = m_origin.z;
    // m_boundary = 
}

// @brief Binds data buffer and draws the entity.
// @note Relies on glDrawArrays() with GL_TRIANGLES mode.
void Entity::Draw(int count)
{
    m_buffer->Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}

void Entity::Paint()
{

}

bool Entity::IsReachable(double x_mouse, double y_mouse, float z_camera)
{
    if ((x_mouse-x)*(x_mouse-x) + (y_mouse-y)*(y_mouse-y) < m_boundary*m_boundary)
    {
        if ((z-m_boundary < z_camera)  && (z_camera < z+m_boundary))
        {
            return true;
        }
    }
    return false;
}

void Entity::Expulse(float timeFrame, glm::vec3 direction)
{

}

void Entity::ChangeModel(const glm::mat4 &model)
{
    m_model = model;
}

// @brief Updates upon the current entity's pose.
// @note If you don't want to update but build from scratch, call ResetModel() beforehand.    
void Entity::UpdateModel(const glm::vec3 &translationAxis, const glm::vec3 &rotationAxis, float rotationAngle, const glm::vec3 &scaleFactor)
{
    m_origin += translationAxis;
    m_rotationAxis += rotationAxis;
    m_rotationAngle += rotationAngle;
    m_scaleFactor *= scaleFactor;
    m_model = glm::mat4(1.0f);
    m_model = glm::translate(m_model, m_origin);
    m_model = glm::rotate(m_model, m_rotationAngle, m_rotationAxis);
    m_model = glm::scale(m_model, m_scaleFactor);
}

// @brief Translates the entity along the given direction.
void Entity::Translate(const glm::vec3 &direction)
{
    m_origin += direction;
    x = m_origin.x + direction.x;
    y = m_origin.y + direction.y;
    z = m_origin.z + direction.z;
    m_model = glm::translate(m_model, direction);
}

// @brief Rotates the entity of the given angle around the given axis.
// @param angle in degree. 
void Entity::Rotate(float angle, const glm::vec3 &axis)
{
    m_model = glm::rotate(m_model, glm::radians(angle), axis);
}

// @brief Scales the entity according to the factor.
// @note The scale is proportional along all axis.
void Entity::Scale(const glm::vec3 &factor)
{
    m_model = glm::scale(m_model, factor);
    // m_boundary *= factor;
}



