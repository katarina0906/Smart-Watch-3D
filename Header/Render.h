#pragma once
#include <glm/glm.hpp>

void RenderUIScreen();
void RenderShadowPass(int w, int h);
void RenderMainScene(int w, int h,
    const glm::mat4& proj, const glm::mat4& view,
    const glm::mat4& handModelM, const glm::vec3& watchCenter);
