#include "camera.hpp"

#include <utility>

// @brief Implements my own lookAt matrix to view
// @note All args are not required to be positive values, they can be negative
// @param n near plan coordinate
// @param f far plan distance
// @param r right screen coordinate
// @param l left screen coordinate
// @param t top screen coordinate
// @param b bottom screen coordinate
static glm::mat4
Clip(float n, float f, float r, float l, float t, float b)
{
    float xe = -2*n/(r-l);
    float ye = -2*n/(t-b);
    float ze1 = (r+l)/(r-l);
    float ze2 = (t+b)/(t-b);
    float A = -(f+n)/(f-n);
    float B = -2*n*f/(f-n);
    glm::mat4 clip = glm::mat4(xe,  0,  ze1,    0,  // 1st column
                                0, ye,  ze2,    0,  // 2nd column
                                0,  0,    A,    B,  // 3rd column
                                0,  0,-1.0f,    0); // 4th column
    return glm::transpose(clip);
}

static glm::mat4
MyLookAt(const glm::vec3 &target, const glm::vec3 &y, const glm::vec3 &position)
{
    glm::vec3 direction = glm::normalize(position-target);
    glm::vec3 right = glm::normalize(glm::cross(y,direction));
    glm::vec3 up = glm::normalize(glm::cross(direction, right));
    glm::mat4 base = glm::mat4(right.x, up.x, direction.x, 0,
                               right.y, up.y, direction.y, 0,
                               right.z, up.z, direction.z, 0,
                                     0,    0,           0, 1);
    glm::mat4 translate = glm::mat4(          1,           0,           0, 0,
                                              0,           1,           0, 0,
                                              0,           0,           1, 0,
                                    -position.x, -position.y, -position.z, 1);
    glm::mat4 view = glm::transpose(base)*glm::transpose(translate);
    return view;
}

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
    // m_view = MyLookAt(m_target, y, m_position);
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
    // m_view = MyLookAt(m_target, y, m_position);
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

void Camera::MoveForward(float timeFrame)
{
    // m_direction is by my definition m_position - m_target (<0)
    glm::vec3 delta = m_speed*timeFrame*glm::normalize(m_direction);
    m_position -= delta;
    m_target -= delta;
}

void Camera::MoveBackwards(float timeFrame)
{
    glm::vec3 delta = m_speed*timeFrame*glm::normalize(m_direction);
    m_position += delta;
    m_target += delta;
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
}