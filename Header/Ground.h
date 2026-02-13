#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
class Shader;

struct GroundSegment {
    float z;
};

void Ground_Init();
void Ground_Update(float dt, float speed, float cameraZ);
void Ground_Render(Shader& shader,
    const glm::mat4& proj,
    const glm::mat4& view,
    const glm::vec3& cameraPos);

void Ground_RenderShadow(Shader& shadowShader, const glm::mat4& lightSpace, const glm::vec3& cameraPos);

void Ground_DrawCube(unsigned int shader,
    const glm::mat4& proj,
    const glm::mat4& view,
    const glm::mat4& model);

GLuint Ground_GetBuildingTexture();
GLuint Ground_GetSkinTexture();
