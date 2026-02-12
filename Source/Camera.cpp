#include "../Header/Camera.h"
#include "../Header/Input.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

double lastMouseY = 0.0;
bool firstMouse = true;
float mouseSensitivity = 0.08f;

Camera::Camera()
{
    position = glm::vec3(0.0f, 1.6f, 3.5f);
    pitch = 0.0f;
    yaw = -90.0f;

    baseY = position.y;
    bobPhase = 0.0f;
    mode = CameraMode::FreeLook;
}

void Camera::Update(float dt, bool running, bool focusMode)
{
    mode = focusMode ? CameraMode::Locked : CameraMode::FreeLook;

    if (mode == CameraMode::FreeLook)
    {
        if (firstMouse)
        {
            lastMouseY = mouseY;
            firstMouse = false;
        }

        double yOffset = lastMouseY - mouseY;
        lastMouseY = mouseY;

        yOffset *= mouseSensitivity;

        pitch += static_cast<float>(yOffset);

        if (pitch > 35.0f)
            pitch = 35.0f;
        if (pitch < -35.0f)
            pitch = -35.0f;

        if (running)
        {
            bobPhase += dt * 10.0f;
            position.y = baseY + sin(bobPhase) * 0.045f;
        }
        else
        {
            bobPhase = 0.0f;
            position.y += (baseY - position.y) * dt * 8.0f;
        }
    }
    else
    {
        pitch += (0.0f - pitch) * dt * 5.0f;
        yaw += (-90.0f - yaw) * dt * 5.0f;
        bobPhase = 0.0f;
        position.y += (baseY - position.y) * dt * 8.0f;
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(
        position,
        position + GetFront(),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

glm::vec3 Camera::GetFront() const
{
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(dir);
}

glm::vec3 Camera::GetRight() const
{
    return glm::normalize(glm::cross(GetFront(), glm::vec3(0, 1, 0)));
}

glm::vec3 Camera::GetUp() const
{
    return glm::normalize(glm::cross(GetRight(), GetFront()));
}
