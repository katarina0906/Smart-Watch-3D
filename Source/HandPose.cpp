#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Header/HandPose.h"
#include "../Header/Camera.h"

float armRotY = -94.5f;
float armRotX = 83.5f;
float armRotZ = 71.5f;
float armScale = 20.284f;
float armPosX = 2.35f;
float focusRotY = -285.5f;
float focusRotX = -1.0f;
float focusRotZ = 89.0f;
float focusScale = 12.16f;
float focusPosForward = 0.0f;
float focusPosRight = 0.80f;
float focusPosUp = 0.20f;
float watchOffsetX = -0.002f, watchOffsetY = 0.024f, watchOffsetZ = -0.056f;
float watchQuadWidthRight = 0.18f;
float watchQuadHeightRight = 0.20f;
float watchQuadWidthFocus = 0.22f;
float watchQuadHeightFocus = 0.22f;

glm::mat4 ComputeHandModelMatrix(const Camera& cam, bool focusMode)
{
    glm::vec3 handPos;
    float rotY, rotX, rotZ, scale;
    if (focusMode)
    {
        handPos = glm::vec3(focusPosRight, 1.4f + focusPosUp, cam.position.z - (0.5f + focusPosForward));
        rotY = focusRotY; rotX = focusRotX; rotZ = focusRotZ;
        scale = focusScale;
    }
    else
    {
        handPos = glm::vec3(armPosX, 1.4f, cam.position.z - 2.2f);
        rotY = 180.0f + armRotY; rotX = -85.0f + armRotX; rotZ = 10.0f + armRotZ;
        scale = 0.55f * armScale;
    }
    glm::mat4 M = glm::translate(glm::mat4(1.0f), handPos);
    M = glm::rotate(M, glm::radians(rotY), glm::vec3(0, 1, 0));
    M = glm::rotate(M, glm::radians(rotX), glm::vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(rotZ), glm::vec3(0, 0, 1));
    M = glm::scale(M, glm::vec3(scale));
    return M;
}

glm::vec3 GetWatchPositionOnHand(const glm::mat4& handModelM, bool)
{
    glm::vec4 localOffset(watchOffsetX, watchOffsetY, watchOffsetZ, 1.0f);
    glm::vec4 worldPos = handModelM * localOffset;
    return glm::vec3(worldPos);
}

bool MouseToWatchQuad3D(
    double mouseX, double mouseY,
    int w, int h,
    const glm::mat4& proj, const glm::mat4& view,
    const glm::vec3& cameraPos,
    const glm::vec3& quadCenter,
    const glm::vec3& quadRight, const glm::vec3& quadUp,
    float halfWidth, float halfHeight,
    float& outMx, float& outMy)
{
    if (w <= 0 || h <= 0) return false;

    glm::vec4 viewport(0.0f, 0.0f, (float)w, (float)h);
    float winY = (float)h - (float)mouseY;
    glm::vec3 nearPt = glm::unProject(glm::vec3((float)mouseX, winY, 0.0f), view, proj, viewport);
    glm::vec3 farPt = glm::unProject(glm::vec3((float)mouseX, winY, 1.0f), view, proj, viewport);
    glm::vec3 rayDir = glm::normalize(farPt - nearPt);
    glm::vec3 rayOrig = nearPt;

    glm::vec3 planeNorm = glm::normalize(cameraPos - quadCenter);
    float denom = glm::dot(rayDir, planeNorm);
    if (std::abs(denom) < 1e-6f) return false;

    float t = glm::dot(quadCenter - rayOrig, planeNorm) / denom;
    if (t < 0.0f) return false;

    glm::vec3 hit = rayOrig + rayDir * t;
    float localX = glm::dot(hit - quadCenter, quadRight);
    float localY = glm::dot(hit - quadCenter, quadUp);

    if (localX < -halfWidth || localX > halfWidth || localY < -halfHeight || localY > halfHeight)
        return false;

    outMx = localX / halfWidth;
    outMy = localY / halfHeight;
    return true;
}
