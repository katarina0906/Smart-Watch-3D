#include "../Header/Input.h"

double mouseX = 0;
double mouseY = 0;
bool mouseClicked = false;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        mouseClicked = true;
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

float convertMouseX(double mouseX, int windowWidth)
{
    return (float)((mouseX / windowWidth) * 2.0 - 1.0);
}

float convertMouseY(double mouseY, int windowHeight)
{
    return (float)(-((mouseY / windowHeight) * 2.0 - 1.0));
}
