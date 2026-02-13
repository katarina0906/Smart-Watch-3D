#pragma once

#include "Camera.h"
#include "model.hpp"
#include "shader.hpp"

extern Camera camera;

extern unsigned int uiShader;
extern Shader* meshShader;
extern Shader* handShader;
extern Shader* screenQuadShader;
extern Shader* shadowShader;
extern Shader* skyShader;
extern Model* handModel;

extern bool focusMode;
extern bool spaceReleased;

extern float watchOffsetRight;
extern float watchOffsetUp;
extern float watchSizeMult;
extern const float focusQuadScale;
