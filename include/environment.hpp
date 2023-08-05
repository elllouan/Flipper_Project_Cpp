#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "entity.hpp"
#include "camera.hpp"
#include "shader.hpp"

// #include <vector>
// #include <memory>

class Environment
{
    glm::vec3 x = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 y = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 z = glm::vec3(0.0, 0.0, 1.0);

private:
    std::vector<Entity> m_entities;
    Camera *m_camera;
    Shader *m_shader;
    
public:
    Environment(Camera *cam, Shader *shader);
    ~Environment();

    void AddEntity(Entity &entity);
    void moveEntity(glm::mat4 &model, int index = 0);
    void moveCamera(glm::mat4 view);
    void Render(float timeFrame);
};


#endif /* ENVIRONMENT_HPP */