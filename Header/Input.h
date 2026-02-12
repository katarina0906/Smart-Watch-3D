#pragma once
#include <GLFW/glfw3.h>

extern double mouseX;
extern double mouseY;
extern bool mouseClicked;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

float convertMouseX(double mouseX, int windowWidth);
float convertMouseY(double mouseY, int windowHeight);
