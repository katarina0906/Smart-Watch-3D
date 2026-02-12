#include "../Header/BPMScreen.h"
#include "../Header/Renderer.h"
#include "../Header/ScreenManager.h"
#include "../Header/Util.h"

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

unsigned int tex_ekg = 0;

struct EKGImpulse {
    float x;
};

std::vector<EKGImpulse> ekgList;

bool isRunning = false;

float impulseSpawnTimer = 0.0f;
float impulseInterval = 0.70f;

float impulseSpeedNormal = 0.40f;
float impulseSpeedRun = 0.65f;

int bpmValue = 70;
double bpmAccumulator = 0.0;
unsigned int tex_alertCircle = 0;

void BPMScreen_Init()
{
    srand((unsigned int)time(nullptr));

    tex_ekg = loadImageToTexture("Resources/Textures/ekg.png");

    glBindTexture(GL_TEXTURE_2D, tex_ekg);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ekgList.clear();
    impulseSpawnTimer = 0.0f;
    impulseInterval = 0.70f;

    bpmValue = 60 + rand() % 20;
    bpmAccumulator = 0.0;

    tex_alertCircle = loadImageToTexture("Resources/Textures/alert_circle.png");

    glBindTexture(GL_TEXTURE_2D, tex_alertCircle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

}
void BPMScreen_Update(double dt, bool running)
{
    isRunning = running;

    float moveSpeed = 0.12f;

    float spawnInterval = isRunning ? 0.30f : 1.0f;

    impulseSpawnTimer += dt;

    if (impulseSpawnTimer >= spawnInterval)
    {
        impulseSpawnTimer = 0.0f;

        ekgList.push_back({ 0.33f });
    }

    for (auto& imp : ekgList)
        imp.x -= dt * moveSpeed;

    ekgList.erase(
        std::remove_if(
            ekgList.begin(),
            ekgList.end(),
            [](const EKGImpulse& imp) { return imp.x < -0.33f; }),
        ekgList.end()
    );

    bpmAccumulator += dt;
    if (isRunning)
    {
        if (bpmAccumulator >= 1.0)
        {
            bpmAccumulator = 0.0;
            bpmValue += 5;
            if (bpmValue > 220) bpmValue = 220;
        }
    }
    else
    {
        if (bpmAccumulator >= 1.0)
        {
            bpmAccumulator = 0.0;
            if (bpmValue > 80) bpmValue -= 2;
            if (bpmValue <= 80) bpmValue = 60 + rand() % 20;
        }
    }
}


void BPMScreen_Render(unsigned int shader, unsigned int quadVAO)
{
    float cx = 0.0f;
    float cy = 0.0f;

    float scaleArrow = 0.0032f;

    Renderer_DrawText("<", cx - 0.63f, cy, scaleArrow, 1, 1, 1);
    Renderer_DrawText(">", cx + 0.54f, cy, scaleArrow, 1, 1, 1);

    char bpmStr[32];
    sprintf(bpmStr, "%d BPM", bpmValue);

    Renderer_DrawText(bpmStr, cx - 0.30f, cy + 0.20f, 0.0040f, 1, 1, 1);

    float ekgW = 0.14f;
    float ekgH = 0.12f;
    float ekgY = -0.12f;

    for (auto& imp : ekgList)
    {
        drawTexturedQuad(shader, quadVAO,
            imp.x,
            ekgY,
            ekgW,
            ekgH,
            tex_ekg);
    }

    if (ekgList.size() >= 2)
    {
        for (int i = 0; i < ekgList.size() - 1; i++)
        {
            float x1 = ekgList[i].x + 0.05f;
            float x2 = ekgList[i + 1].x - 0.05f;

            if (x2 > x1)
            {
                float midX = (x1 + x2) * 0.5f;
                float connectorW = (x2 - x1);

                drawTexturedQuad(shader, quadVAO,
                    midX,
                    ekgY,
                    connectorW,
                    0.02f,
                    tex_ekg);
            }
        }
    }


    if (bpmValue > 200)
    {
        drawTexturedQuad(
            shader,
            quadVAO,
            0.0f, 0.0f,
            0.7f, 0.7f,
            tex_alertCircle
        );


    }
}
