#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Header/App.h"
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
#include "../Header/GLState.h"
#include "../Header/Render.h"

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

float watchOffsetRight = -0.21f;
float watchOffsetUp = -0.01f;
float watchSizeMult = 0.91f;
const float focusQuadScale = 0.78f;

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

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        depthTestEnabled = true;
        ApplyGLState();
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        depthTestEnabled = false;
        ApplyGLState();
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        cullFaceEnabled = true;
        ApplyGLState();
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        cullFaceEnabled = false;
        ApplyGLState();
    }
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
