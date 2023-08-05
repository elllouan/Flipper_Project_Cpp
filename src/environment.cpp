#include "environment.hpp"

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Environment::Environment(Camera *cam, Shader *shader) : 
    m_camera {cam},
    m_shader {shader}
{
}

Environment::~Environment()
{
}

void Environment::AddEntity(Entity &entity)
{
    m_entities.emplace_back(entity);
}

// @brief Modifies each entity position with the given model matrix.
// @note index Starts at 1. 
void Environment::moveEntity(glm::mat4 &model, int index)
{
    if(index)
    {
        if (index < m_entities.size()) 
            m_entities.at(index-1).MoveOrigin(model);
    }
    else
    {
        for (auto &ent: m_entities)
        {
            ent.MoveOrigin(model);
        }
    }
}

void Environment::moveCamera(glm::mat4 view)
{
    // m_camera->ChangeView()
}


void Environment::Render(float timeFrame)
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