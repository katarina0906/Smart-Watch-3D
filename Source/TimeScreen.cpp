#include "../Header/TimeScreen.h"
#include "../Header/Renderer.h"
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>


int ts_hours = 0;
int ts_minutes = 0;
int ts_seconds = 0;

double ts_accumulator = 0.0;

void TimeScreen_Init()
{
    ts_hours = 0;
    ts_minutes = 0;
    ts_seconds = 0;
    ts_accumulator = 0.0;
}

void TimeScreen_Update(double deltaTime)
{
    ts_accumulator += deltaTime;

    while (ts_accumulator >= 1.0)
    {
        ts_accumulator -= 1.0;
        ts_seconds++;

        if (ts_seconds >= 60)
        {
            ts_seconds = 0;
            ts_minutes++;
        }

        if (ts_minutes >= 60)
        {
            ts_minutes = 0;
            ts_hours++;
        }

        if (ts_hours >= 24)
        {
            ts_hours = 0;
        }
    }
}
void TimeScreen_Render(unsigned int shader, unsigned int quadVAO)
{
    char timeStr[9];
    sprintf_s(timeStr, "%02d:%02d:%02d", ts_hours, ts_minutes, ts_seconds);

    float scaleTime = 0.0030f;
    float scaleArrow = 0.0032f;

    float cx = 0.0f;
    float cy = 0.0f;
    float offsetX = -0.2f;   

    Renderer_DrawText(timeStr, cx - 0.05f + offsetX, cy, scaleTime, 1, 1, 1);
    Renderer_DrawText(">", cx + 0.73f + offsetX, cy, scaleArrow, 1, 1, 1);
}






