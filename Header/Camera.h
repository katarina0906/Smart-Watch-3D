#pragma once
#include <glm/glm.hpp>

enum class CameraMode {
    FreeLook,
    Locked
};

class Camera {
public:
    Camera();

    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetFront() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    void Update(float dt, bool running, bool focusMode);

    glm::vec3 position;
    float pitch;
    float yaw;

private:
    float baseY;
    float bobPhase;
    CameraMode mode;
};

extern double lastMouseY;
extern bool firstMouse;
extern float mouseSensitivity;
