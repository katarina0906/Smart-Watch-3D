#pragma once
#include <glm/glm.hpp>

class Camera;

/* Parametri za pozicioniranje ruke i sata */
extern float armRotY, armRotX, armRotZ, armScale, armPosX;
extern float focusRotY, focusRotX, focusRotZ, focusScale;
extern float focusPosForward, focusPosRight, focusPosUp;
extern float watchOffsetX, watchOffsetY, watchOffsetZ;
extern float watchQuadWidthRight, watchQuadHeightRight;
extern float watchQuadWidthFocus, watchQuadHeightFocus;

glm::mat4 ComputeHandModelMatrix(const Camera& cam, bool focusMode);
glm::vec3 GetWatchPositionOnHand(const glm::mat4& handModelM, bool focusMode);

/* Provera da li je klik u 3D prostoru na watch quad.
   Vraca true ako je hit, a outMx, outMy su normalizovane koordinate [-1,1] unutar kvada. */
bool MouseToWatchQuad3D(
    double mouseX, double mouseY,
    int w, int h,
    const glm::mat4& proj, const glm::mat4& view,
    const glm::vec3& cameraPos,
    const glm::vec3& quadCenter,
    const glm::vec3& quadRight, const glm::vec3& quadUp,
    float halfWidth, float halfHeight,
    float& outMx, float& outMy);
