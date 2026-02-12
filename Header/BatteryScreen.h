#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>
void BatteryScreen_Init();
void BatteryScreen_Update(double dt);
void BatteryScreen_Render(unsigned int shader, unsigned int quadVAO);
