#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

// @brief This class handles the view/camera coordinate system space.
class Camera
{
    static constexpr float MAX_ANGLE = 80.0f;
    static constexpr float MIN_ANGLE = 1.0f;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_direction; // z' = Dx*x + Dy*y + Dz*z
    glm::vec3 m_right; // x' = Rx*x + Ry*y + Rz*z
    glm::vec3 m_up; // y' = Ux*x + Uy*y + Uz*z
    std::array<glm::vec3, 4> m_base; // {z', x', y'}
    float m_width {800.0f};
    float m_height {600.0f};
    float m_near {0.1f};
    float m_far {100.0f};
    float m_fov {45.0f};
    float m_yaw {0.0f};
    float m_pitch {0.0f};
    float m_zoomSensitivity = {2.0f};
    float m_speed;
    float m_rotationSpeed;

    glm::mat4 m_view;
    glm::mat4 m_perspective;

    void __SetBase();

public:

    glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera(glm::vec3 &initialPosition,
           glm::vec3 &initialTarget,
           glm::vec3 &y = glm::vec3(0.0f, 1.0f, 0.0f),
           float speed = 1.0f,
           float rotationSpeed = 0.1f);

    ~Camera() = default;

    glm::mat4 CreateView();
    glm::mat4 Project(float width, float height, float near, float far, float fov);
    glm::mat4 ChangeView(const glm::vec3& newPosition, const glm::vec3& newTarget);
    glm::mat4 UpdateView();
    // glm::mat4 UpdatePerspective();
    void MoveRight(float timeFrame);
    void MoveLeft(float timeFrame);
    void MoveForward(float timeFrame);
    void MoveBackwards(float timeFrame);
    void SpinView(float yaw, float pitch);
    void ZoomView(float fov);
    void NodView(float time, float limitAngle = glm::radians(0.0f), bool right = true);

    const glm::mat4&
    GetViewMat() const
    {
        return m_view;
    }

    const glm::mat4&
    GetPerspectiveMat() const
    {
        return m_perspective;
    }

};


#endif /* CAMERA_HPP */