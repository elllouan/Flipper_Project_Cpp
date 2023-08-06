#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "entity.hpp"
#include "camera.hpp"
#include "shader.hpp"

// #include <vector>
// #include <memory>

class Packet
{
    glm::vec3 x = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 y = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 z = glm::vec3(0.0, 0.0, 1.0);

private:
    std::vector<Entity> m_entities;
    Camera *m_camera;
    Shader *m_shader;
    
public:
    Packet(Camera *cam, Shader *shader);
    ~Packet();

    void AddEntity(Entity &entity);

    void MoveEntity(glm::mat4 &model, int index = 0);
    void UpdateEntity(glm::vec3 &translationAxis = glm::vec3(0.0f),
                    glm::vec3 &rotationAxis = glm::vec3(0.0f),
                    float rotationAngle = 0.0f,
                    glm::vec3 &scaleFactor = glm::vec3(1.0f),
                    int index = 0);

    void CheckContact(float timeFrame, double x_mouse, double y_mouse, bool &click_mouse);
    void Render(float timeFrame);
};


#endif /* ENVIRONMENT_HPP */