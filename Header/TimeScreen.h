#pragma once

#include <GL/glew.h>

void TimeScreen_Init();
void TimeScreen_Update(double deltaTime);
void TimeScreen_Render(unsigned int shader, unsigned int quadVAO);
