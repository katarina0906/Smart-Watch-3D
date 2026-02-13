#pragma once
#include <GL/glew.h>

extern unsigned int tex_watchFrame;
extern unsigned int tex_nameplate;

void Renderer_Init();

void drawTexturedQuad(
    unsigned int shader,
    unsigned int vao,
    float x, float y,
    float sx, float sy,
    unsigned int tex
);

void drawQuadRect(
    unsigned int shader,
    unsigned int vao,
    float x, float y,
    float sx, float sy,
    float r, float g, float b, float a
);
void Renderer_DrawText(
    const char* text,
    float x, float y,
    float scale,
    float r, float g, float b
);

void Renderer_DrawTextScreen(
    const char* text,
    float x, float y,
    float scale,
    float r, float g, float b,
    int screenW, int screenH
);
