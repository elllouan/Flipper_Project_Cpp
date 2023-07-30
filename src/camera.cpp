#include "camera.hpp"

#include <utility>

void
Camera::__SetBase()
{
    // Builds the new system (view system) where the camera is the origin
    m_direction = glm::normalize(m_position-m_target);
    m_right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_direction));
    m_up = glm::normalize(glm::cross(m_direction, m_right));
    // Generates a orthogonal-normalized coordinates system: base = (origin, z', x', y')
    m_base = {m_position, m_direction, m_right, m_up};
}

Camera::Camera(glm::vec3 &initialPosition,
               glm::vec3 &initialTarget,
               glm::vec3 &y,
               float speed,
               float rotationSpeed):
    m_position {std::move(initialPosition)},
    m_target {std::move(initialTarget)},
    y {std::move(y)},
    m_speed {speed},
    m_rotationSpeed {rotationSpeed}
{
    __SetBase();
}

glm::mat4 Camera::CreateView()
{
    m_view = glm::lookAt(m_position, m_target, y);
    return m_view;
}

glm::mat4 Camera::Project(float width, float height, float near, float far, float fov)
{
    // TODO: Handling Error
    if (fov > MAX_ANGLE)
    {
        m_fov = MAX_ANGLE;
    }
    else if (fov < MIN_ANGLE)
    {
        m_fov = MIN_ANGLE;
    }
    else
    {
        m_fov = fov;
    }
    float m_width = width;
    float m_height = height;
    float m_near = near;
    float m_far = far;
    m_perspective = glm::mat4(1.0f);
    m_perspective = glm::perspective(glm::radians(fov), m_width/m_height, m_near, m_far);
    return m_perspective;
}

glm::mat4 Camera::ChangeView(const glm::vec3 &newPosition, const glm::vec3 &newTarget)
{
    m_view = glm::mat4(1.0f);
    m_view = glm::lookAt(newPosition, newTarget, y);
    __SetBase();
    return m_view;
}

glm::mat4 Camera::UpdateView()
{
    m_view = glm::lookAt(m_position, m_target, y);
    __SetBase();
    return m_view;
}

void Camera::MoveRight(float timeFrame)
{
    glm::vec3 delta = m_speed*timeFrame*glm::normalize(glm::cross(m_up, m_direction));
    m_position += delta;
    m_target += delta;
}

void Camera::MoveLeft(float timeFrame)
{
    glm::vec3 delta = m_speed*timeFrame*glm::normalize(glm::cross(m_direction, m_up));
    m_position += delta;
    m_target += delta;
}

void Camera::MoveForward(float timeFrame, Mode mode)
{
    // m_direction is by my definition m_position - m_target (<0)
    if (mode == Mode::FPS)
    {
        // We discard y component to keep moving across z0x plan
        glm::vec3 delta = m_speed*timeFrame*glm::normalize(glm::vec3(m_direction.x, 0, m_direction.z));
        m_position -= delta;
        m_target -= delta;
    }
    else if (mode == Mode::NORMAL)
    {
        glm::vec3 delta = m_speed*timeFrame*glm::normalize(m_direction);
        m_position -= delta;
        m_target -= delta;
    }
}

void Camera::MoveBackwards(float timeFrame, Mode mode)
{
    // m_direction is by my definition m_position - m_target (<0)
    if (mode == Mode::FPS)
    {
        // We discard y component to keep moving across z0x plan
        glm::vec3 delta = m_speed*timeFrame*glm::normalize(glm::vec3(m_direction.x, 0, m_direction.z));
        m_position += delta;
        m_target += delta;
    }
    else if (mode == Mode::NORMAL)
    {
        glm::vec3 delta = m_speed*timeFrame*glm::normalize(m_direction);
        m_position += delta;
        m_target += delta;
    }
}


void Camera::SpinView(float yaw, float pitch)
{
    m_direction = glm::vec3(sin(glm::radians(yaw))*cos(glm::radians(pitch)),
                            sin(glm::radians(pitch)),
                            cos(glm::radians(yaw))*cos(glm::radians(pitch)));
    m_target = m_position - m_direction;
}

void Camera::ZoomView(float fov)
{
    // TODO: Handling Error
    if (fov > MAX_ANGLE)
    {
        fov = MAX_ANGLE;
    }
    else if (fov < MIN_ANGLE)
    {
        fov = MIN_ANGLE;
    }
    m_fov = fov;
    m_perspective = glm::perspective(glm::radians(m_fov), m_width/m_height, m_near, m_far);
}


// @param limitAngle in degrees
void Camera::NodView(float frameTime, float limitAngle, bool right)
{
    // Start by nodding upwards 
    static size_t sign = 1;
    static float pitch = 0;

    if (pitch < glm::radians(limitAngle) || pitch > -glm::radians(limitAngle))
    {
        sign = -sign;
    }
    pitch = pitch + sign*glm::radians(m_rotationSpeed*frameTime);

    float projXZ = sqrt(m_direction.x*m_direction.x + m_direction.z*m_direction.z);
    float yawX = projXZ/m_direction.x;
    float yawZ = projXZ/m_direction.z; // cos(yawZ) = sin(yawX)
    // -pitch along y-axis because I want the camera to nod upwards if pitch is positive
    m_direction = glm::vec3(projXZ*sin(-pitch)*cos(yawX),
                            m_direction.y*sin(-pitch),
                            projXZ*sin(-pitch)*cos(yawZ));
    
    m_target = m_position-m_direction;
    // m_view = glm::lookAt(m_position, m_target, y);
}