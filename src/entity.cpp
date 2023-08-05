#include "entity.hpp"

Entity::Entity(ItemBuffer *buffer) : m_buffer {buffer}
{
    m_model = glm::mat4(1.0f);
}

// @brief Draws the entity.
// @note Relies on glDrawArrays() with GL_TRIANGLES mode.
void Entity::Draw(int count)
{
    m_buffer->Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}

void Entity::Paint()
{

}

void Entity::MoveOrigin(glm::mat4 &model)
{
    m_model = model;
}

// @brief Translates the entity along the given direction.
void Entity::Translate(const glm::vec3 &direction)
{
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
}



