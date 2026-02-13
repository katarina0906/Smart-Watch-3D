 #include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../Header/Util.h"
#include "../Header/Renderer.h"
#include "../Header/TimeScreen.h"
#include "../Header/BPMScreen.h"
#include "../Header/BatteryScreen.h"
#include "../Header/ScreenManager.h"
#include "../Header/Camera.h"
#include "../Header/Ground.h"
#include "../Header/Input.h"
#include "../Header/model.hpp"
#include "../Header/shader.hpp"
#include "../Header/Shadow.h"
#include "../Header/Sky.h"
#include "../Header/HandPose.h"
#include "../Header/SceneResources.h"

Camera camera;

unsigned int uiShader;
Shader* meshShader = nullptr;
Shader* handShader = nullptr;
Shader* screenQuadShader = nullptr;
Shader* shadowShader = nullptr;
Shader* skyShader = nullptr;
Model* handModel = nullptr;

bool focusMode = false;
bool spaceReleased = true;
bool depthTestEnabled = true;
bool cullFaceEnabled = false;

float watchOffsetRight = -0.21f;
float watchOffsetUp = -0.01f;
float watchSizeMult = 0.91f;
const float focusQuadScale = 0.78f;

static void ApplyGLState()
{
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (cullFaceEnabled) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

static void ApplyGLStateUI()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

static void ApplyGLStateShadow()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

static void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        spaceReleased = true;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && spaceReleased)
    {
        spaceReleased = false;
        focusMode = !focusMode;
        glfwSetInputMode(window, GLFW_CURSOR, focusMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        if (focusMode)
        {
            camera.pitch = 0.0f;
            camera.yaw = -90.0f;
        }
        firstMouse = true;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        depthTestEnabled = true;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        depthTestEnabled = false;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        cullFaceEnabled = true;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        cullFaceEnabled = false;
}

static void ProcessWatchPicking(int pickW, int pickH)
{
    if (!focusMode || !mouseClicked)
        return;

    float mx, my;
    glm::mat4 handM = ComputeHandModelMatrix(camera, true);
    glm::vec3 quadCenter = GetWatchPositionOnHand(handM, true);
    quadCenter -= camera.GetRight() * 0.07f;
    float halfW = watchQuadWidthFocus * focusQuadScale * 0.5f;
    float halfH = watchQuadHeightFocus * focusQuadScale * 0.5f;
    glm::mat4 projPick = glm::perspective(glm::radians(60.0f), float(pickW) / float(pickH), 0.1f, 100.0f);
    glm::mat4 viewPick = camera.GetViewMatrix();

    if (!MouseToWatchQuad3D(mouseX, mouseY, pickW, pickH, projPick, viewPick,
        camera.position, quadCenter, camera.GetRight(), camera.GetUp(),
        halfW, halfH, mx, my))
    {
        mouseClicked = false;
        return;
    }

    if (currentScreen == SCREEN_TIME)
    {
        if (mx > 0.35f && mx < 0.80f && my > -0.25f && my < 0.25f)
            currentScreen = SCREEN_BPM;
    }
    else if (currentScreen == SCREEN_BPM)
    {
        if (mx > -0.85f && mx < -0.40f && my > -0.25f && my < 0.25f)
            currentScreen = SCREEN_TIME;
        else if (mx > 0.35f && mx < 0.80f && my > -0.25f && my < 0.25f)
            currentScreen = SCREEN_BATTERY;
    }
    else if (currentScreen == SCREEN_BATTERY)
    {
        if (mx > -0.85f && mx < -0.40f && my > -0.25f && my < 0.25f)
            currentScreen = SCREEN_BPM;
    }
    mouseClicked = false;
}

static void RenderUIScreen()
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

static void RenderShadowPass(int w, int h)
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

static void RenderMainScene(int w, int h,
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

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const unsigned int wWidth = 1280, wHeight = 720;
    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "Smart Watch 3D", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();

    GLFWcursor* heartCursor = loadImageToCursor("Resources/Textures/hello.png");
    if (heartCursor != nullptr)
        glfwSetCursor(window, heartCursor);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    ApplyGLState();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);

    Renderer_Init();
    TimeScreen_Init();
    BPMScreen_Init();
    BatteryScreen_Init();
    Ground_Init();

    SceneResources_Init();
    Shadow_Init();
    Sky_Init();

    uiShader = createShader("Resources/Shaders/basic.vert", "Resources/Shaders/basic.frag");

    shadowShader = new Shader(
        "Resources/Shaders/shadow_depth.vert",
        "Resources/Shaders/shadow_depth.frag"
    );

    meshShader = new Shader(
        "Resources/Shaders/mesh3d_assimp.vert",
        "Resources/Shaders/mesh3d_assimp.frag"
    );
    screenQuadShader = new Shader(
        "Resources/Shaders/screen_quad.vert",
        "Resources/Shaders/screen_quad.frag"
    );
    handShader = new Shader(
        "Resources/Shaders/hand_model.vert",
        "Resources/Shaders/hand_model.frag"
    );
    handModel = new Model("Resources/Models/Hands/rigged_arm.obj");

    skyShader = new Shader(
        "Resources/Shaders/sky.vert",
        "Resources/Shaders/sky.frag"
    );

    currentScreen = SCREEN_TIME;
    double last = glfwGetTime();
    double lastFrame = glfwGetTime();
    const double targetFrameTime = 1.0 / 75.0;

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        while (now - lastFrame < targetFrameTime) now = glfwGetTime();
        lastFrame = now;

        float dt = float(now - last);
        last = now;

        ProcessInput(window);
        ApplyGLState();

        int pickW, pickH;
        glfwGetFramebufferSize(window, &pickW, &pickH);
        if (pickW <= 0 || pickH <= 0) { pickW = 1280; pickH = 720; }

        ProcessWatchPicking(pickW, pickH);

        bool running = (currentScreen == SCREEN_BPM &&
            glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
        camera.Update(dt, running, focusMode);
        float runningSpeed = running ? 5.0f : 0.0f;
        Ground_Update(dt, runningSpeed, camera.position.z);

        TimeScreen_Update(dt);
        BPMScreen_Update(dt, running);
        BatteryScreen_Update(dt);

        RenderUIScreen();
        ApplyGLState();

        int w = pickW, h = pickH;
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(w) / float(h), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        Sky_Render(skyShader, skyTex, proj, view, camera.position);
        ApplyGLState();

        glm::mat4 handModelM = ComputeHandModelMatrix(camera, focusMode);
        glm::vec3 watchCenter = GetWatchPositionOnHand(handModelM, focusMode);
        if (focusMode) {
            watchCenter -= camera.GetRight() * 0.07f;
        } else {
            watchCenter += camera.GetRight() * watchOffsetRight;
            watchCenter += camera.GetUp() * watchOffsetUp;
        }

        RenderShadowPass(w, h);
        ApplyGLState();

        RenderMainScene(w, h, proj, view, handModelM, watchCenter);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (heartCursor != nullptr)
        glfwDestroyCursor(heartCursor);

    glfwTerminate();
    return 0;
}
