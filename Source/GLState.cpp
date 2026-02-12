#include <GL/glew.h>
#include "../Header/GLState.h"

bool depthTestEnabled = true;
bool cullFaceEnabled = false;
bool depthToggleReleased = true;
bool cullToggleReleased = true;

void ForceGLState()
{
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);
    if (cullFaceEnabled && !depthTestEnabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
}
