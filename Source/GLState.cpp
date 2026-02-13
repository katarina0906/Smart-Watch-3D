#include <GL/glew.h>
#include "../Header/GLState.h"

bool depthTestEnabled = true;
bool cullFaceEnabled = false;

void ApplyGLState()
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

void ApplyGLStateUI()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void ApplyGLStateShadow()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
