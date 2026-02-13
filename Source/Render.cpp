#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../Header/App.h"
#include "../Header/GLState.h"
#include "../Header/Camera.h"
#include "../Header/Ground.h"
#include "../Header/HandPose.h"
#include "../Header/Renderer.h"
#include "../Header/SceneResources.h"
#include "../Header/TimeScreen.h"
#include "../Header/BPMScreen.h"
#include "../Header/BatteryScreen.h"
#include "../Header/ScreenManager.h"
#include "../Header/Shadow.h"
#include "../Header/shader.hpp"
#include "../Header/model.hpp"

void RenderUIScreen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);
    glViewport(0, 0, 1024, 1024);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);

    ApplyGLStateUI();
    if (currentScreen == SCREEN_TIME) TimeScreen_Render(uiShader, quadVAO);
    else if (currentScreen == SCREEN_BPM) BPMScreen_Render(uiShader, quadVAO);
    else BatteryScreen_Render(uiShader, quadVAO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderShadowPass(int w, int h)
{
    glm::mat4 lightSpaceS = ComputeLightSpaceMatrix(camera);
    glm::mat4 handModelM = ComputeHandModelMatrix(camera, focusMode);

    glViewport(0, 0, (GLsizei)SHADOW_WIDTH, (GLsizei)SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    ApplyGLStateShadow();

    if (shadowShader != nullptr)
    {
        Ground_RenderShadow(*shadowShader, lightSpaceS, camera.position);
        if (handModel != nullptr)
        {
            shadowShader->use();
            shadowShader->setMat4("uLightSpace", lightSpaceS);
            shadowShader->setMat4("uM", handModelM);
            handModel->Draw(*shadowShader);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
}

void RenderMainScene(int w, int h,
    const glm::mat4& proj, const glm::mat4& view,
    const glm::mat4& handModelM, const glm::vec3& watchCenter)
{
    glm::vec3 lightPos = GetMainLightPosition(camera);
    glm::mat4 lightSpace = ComputeLightSpaceMatrix(camera);
    glm::vec3 sceneCenter(0.0f, 1.5f, camera.position.z + 8.0f);
    glm::vec3 lightDir = glm::normalize(sceneCenter - lightPos);

    meshShader->use();
    meshShader->setMat4("uP", proj);
    meshShader->setMat4("uV", view);
    meshShader->setVec3("uViewPos", camera.position);
    meshShader->setVec3("uLightDir", lightDir);
    meshShader->setVec3("uLightColor", glm::vec3(1.0f, 0.96f, 0.9f));
    meshShader->setMat4("uLightSpace", lightSpace);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMapTex);
    meshShader->setInt("uShadowMap", 1);
    meshShader->setFloat("uAmbientStrength", 0.28f);
    meshShader->setVec3("uScreenLightPos", watchCenter);
    meshShader->setVec3("uScreenLightColor", glm::vec3(0.6f, 0.8f, 1.0f));
    meshShader->setFloat("uScreenLightStrength", 1.0f);
    meshShader->setFloat("uMaterial.shine", 32.0f);
    meshShader->setVec3("uMaterial.kS", glm::vec3(0.5f, 0.5f, 0.5f));
    meshShader->setFloat("uEmissive", 0.0f);
    meshShader->setFloat("uDebugSolidColor", 0.0f);
    meshShader->setVec3("uSolidColor", glm::vec3(1.0f, 0.1f, 0.1f));

    ApplyGLState();
    Ground_Render(*meshShader, proj, view, camera.position);

    if (handModel != nullptr && handShader != nullptr)
    {
        ApplyGLState();
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);

        handShader->use();
        handShader->setMat4("uP", proj);
        handShader->setMat4("uV", view);
        handShader->setMat4("uM", handModelM);
        handShader->setVec3("uLightPos", lightPos);
        handShader->setVec3("uViewPos", camera.position);
        handShader->setVec3("uLightColor", glm::vec3(1.0f, 0.98f, 0.95f));
        handShader->setVec3("uLightDir", lightDir);
        handShader->setVec3("uScreenLightPos", watchCenter);
        handShader->setVec3("uScreenLightColor", glm::vec3(0.4f, 0.65f, 1.0f));
        handShader->setFloat("uScreenLightStrength", 0.6f);
        handShader->setFloat("uDebugMode", 0.0f);
        handShader->setVec3("uDebugColor", glm::vec3(1.0f, 0.0f, 0.0f));

        handModel->Draw(*handShader);
    }

    float quadW, quadH;
    if (focusMode) {
        quadW = watchQuadWidthFocus * focusQuadScale;
        quadH = watchQuadHeightFocus * focusQuadScale;
    } else {
        quadW = quadH = watchQuadHeightRight * watchSizeMult;
    }
    glm::mat3 watchRot(camera.GetRight(), camera.GetUp(), -camera.GetFront());
    glm::mat4 watchModel = glm::translate(glm::mat4(1.0f), watchCenter)
        * glm::mat4(watchRot)
        * glm::scale(glm::mat4(1.0f), glm::vec3(quadW, quadH, 1.0f));

    ApplyGLState();
    glDepthFunc(GL_ALWAYS);
    screenQuadShader->use();
    screenQuadShader->setMat4("uP", proj);
    screenQuadShader->setMat4("uV", view);
    screenQuadShader->setMat4("uM", watchModel);
    screenQuadShader->setFloat("uAspect", quadW / quadH);
    screenQuadShader->setFloat("uCircleRadius", 0.95f);
    screenQuadShader->setFloat("uBezelWidth", 0.06f);
    screenQuadShader->setVec3("uBezelColor", glm::vec3(0.55f, 0.08f, 0.08f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uiTex);
    screenQuadShader->setInt("uTex", 0);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    ApplyGLStateUI();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (tex_nameplate)
        drawTexturedQuad(uiShader, quadVAO, -0.82f, -0.88f, 0.40f, 0.40f, tex_nameplate);
}
