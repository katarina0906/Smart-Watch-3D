#include <GL/glew.h>
#include <iostream>
#include "../Header/SceneResources.h"

unsigned int quadVAO = 0;
unsigned int quadVBO = 0;
unsigned int uiFBO = 0;
unsigned int uiTex = 0;
unsigned int uiDepth = 0;

static void Create3DQuad()
{
    float v[] = {
        -0.5f,-0.5f,0,  0,0,  0,0,1,
         0.5f,-0.5f,0,  1,0,  0,0,1,
         0.5f, 0.5f,0,  1,1,  0,0,1,
        -0.5f,-0.5f,0,  0,0,  0,0,1,
         0.5f, 0.5f,0,  1,1,  0,0,1,
        -0.5f, 0.5f,0,  0,1,  0,0,1
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

static void CreateUIFBO()
{
    glGenFramebuffers(1, &uiFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);

    glGenTextures(1, &uiTex);
    glBindTexture(GL_TEXTURE_2D, uiTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTex, 0);

    glGenRenderbuffers(1, &uiDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, uiDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, uiDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "UI FBO nije kompletan." << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneResources_Init()
{
    Create3DQuad();
    CreateUIFBO();
}
