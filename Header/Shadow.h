#pragma once
#include <glm/glm.hpp>

class Camera;

extern const unsigned int SHADOW_WIDTH;
extern const unsigned int SHADOW_HEIGHT;
extern unsigned int shadowMapFBO;
extern unsigned int shadowMapTex;

void Shadow_Init();
glm::vec3 GetMainLightPosition(const Camera& camera);
glm::mat4 ComputeLightSpaceMatrix(const Camera& camera);
