#include "packet.hpp"

#include <utility>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Packet::Packet(Camera *cam, Shader *shader) : 
    m_camera {cam},
    m_shader {shader}
{
}

Packet::~Packet()
{
}

void Packet::AddEntity(Entity &entity)
{
    m_entities.emplace_back(entity);
}

// @brief Modifies each entity's pose from scratch according to the given model matrix.
// @param index Starts at 1. If not specified, all entities will be moved the same.
// @note Erases the current entity's model.
void Packet::MoveEntity(glm::mat4 &model, int index)
{
    if(index)
    {
        if (index < m_entities.size()) 
            m_entities.at(index-1).ChangeModel(model);
    }
    else
    {
        for (auto &ent: m_entities)
        {
            ent.ChangeModel(model);
        }
    }
}

// @brief Updates each entity's pose upon the current one according to the given vectors.
// @param index Starts at 1. If not specified, all entities will be moved the same.
// @note Updates the entity's members as well, so you can call it only when you want to update.
void Packet::UpdateEntity(glm::vec3 &translationAxis, glm::vec3 &rotationAxis, float rotationAngle, glm::vec3 &scaleFactor, int index)
{
    if(index)
    {
        if (index < m_entities.size()) 
            m_entities.at(index-1).UpdateModel(translationAxis, rotationAxis, rotationAngle, scaleFactor);
    }
    else
    {
        for (auto &ent: m_entities)
        {
            ent.UpdateModel(translationAxis, rotationAxis, rotationAngle, scaleFactor);
        }
    }
}

void Packet::CheckContact(float timeFrame, double x_mouse, double y_mouse, bool &click_mouse)
{
    for (auto &entity: m_entities)
    {
        if (entity.IsReachable(x_mouse, y_mouse, m_camera->GetTarget().z) && click_mouse)
        {
            std::cout << "DEBUG: IsReachable() = true.\n";
            entity.UpdateModel(m_camera->GetDirection(), glm::vec3(0.0f), timeFrame, glm::vec3(1.0f));
        }
    }
}

// @brief Renders every entity that is contained in the environment.
// @note Updates all uniforms and draws entities.
void Packet::Render(float timeFrame)
{
    // First, use the shader program
    m_shader->UseProgram();

    // Then, update uniforms
    m_shader->SetInt("woodSampler", 0); // woodSampler in the vertex shader is equal to the wood texture
    m_shader->SetInt("smileySampler", 1); // smileySampler in the vertex shader is equal to the smiley texture

    m_shader->SetMatrix4fv("view", glm::value_ptr(m_camera->GetViewMat()));
    m_shader->SetMatrix4fv("perspective", glm::value_ptr(m_camera->GetPerspectiveMat()));
    for (auto &ent: m_entities)
    {
        // Modifies entities positions in space using the model matrix
        m_shader->SetMatrix4fv("model", glm::value_ptr(ent.GetModelMat()));
        // Each cube is built with 6 squares containing 2 triangles each.
        // Each triangle has 3 summits: 6*2*3 = 36 points to draw.
        ent.Draw(36); 
    }

    // Generates a new lookAt/view matrix based off user interactions: mouse click, motion ...
    m_camera->UpdateView();
}