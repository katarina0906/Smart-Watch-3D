#pragma once
#include <glm/glm.hpp>

class Shader;

extern unsigned int skyVAO;
extern unsigned int skyVBO;
extern unsigned int skyTex;
extern unsigned int skyVertexCount;

void Sky_Init();
void Sky_Render(Shader* shader, unsigned int skyTexture,
    const glm::mat4& proj, const glm::mat4& view, const glm::vec3& cameraPos);
