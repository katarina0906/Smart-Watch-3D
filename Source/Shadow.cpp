#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../Header/Shadow.h"
#include "../Header/Camera.h"

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
unsigned int shadowMapFBO = 0;
unsigned int shadowMapTex = 0;

void Shadow_Init()
{
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &shadowMapTex);
    glBindTexture(GL_TEXTURE_2D, shadowMapTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Shadow FBO nije kompletan." << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::vec3 GetMainLightPosition(const Camera& camera)
{
    return glm::vec3(-12.0f, 16.0f, camera.position.z - 6.0f);
}

glm::mat4 ComputeLightSpaceMatrix(const Camera& camera)
{
    glm::vec3 lightPos = GetMainLightPosition(camera);
    glm::vec3 lookAt(0.3f, 0.8f, camera.position.z + 5.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProj = glm::ortho(-40.0f, 40.0f, -30.0f, 40.0f, 0.5f, 100.0f);
    return lightProj * lightView;
}
