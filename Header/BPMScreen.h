#pragma once
#include <GL/glew.h>
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>

extern unsigned int tex_ekg;
extern bool isRunning;
extern bool coolingDown;

void BPMScreen_Init();
void BPMScreen_Update(double dt, bool running);
void BPMScreen_Render(unsigned int shader, unsigned int quadVAO);
