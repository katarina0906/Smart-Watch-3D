#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>
#include "../Header/Sky.h"
#include "../Header/Util.h"
#include "../Header/shader.hpp"

unsigned int skyVAO = 0;
unsigned int skyVBO = 0;
unsigned int skyTex = 0;
unsigned int skyVertexCount = 0;

static void CreateSkySphere()
{
    const int slices = 32;
    const int stacks = 16;
    std::vector<float> verts;
    for (int i = 0; i <= stacks; i++)
    {
        float v = (float)i / stacks;
        float phi = 3.14159265f * v;
        float y = std::cos(phi);
        float r = std::sin(phi);
        for (int j = 0; j <= slices; j++)
        {
            float u = (float)j / slices;
            float theta = 2.0f * 3.14159265f * u;
            float x = r * std::cos(theta);
            float z = r * std::sin(theta);
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
        }
    }
    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            int a = i * (slices + 1) + j;
            int b = a + slices + 1;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);
            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }
    skyVertexCount = (unsigned int)indices.size();
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Sky_Init()
{
    skyTex = loadImageToTexture("Resources/Textures/starsky1.jpg");
    if (skyTex != 0)
    {
        glBindTexture(GL_TEXTURE_2D, skyTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    CreateSkySphere();
}

void Sky_Render(Shader* shader, unsigned int skyTexture,
    const glm::mat4& proj, const glm::mat4& view, const glm::vec3& cameraPos)
{
    if (shader == nullptr || skyTexture == 0 || skyVAO == 0)
        return;

    glDisable(GL_CULL_FACE);
    shader->use();
    glm::mat4 skyModel = glm::translate(glm::mat4(1.0f), cameraPos);
    skyModel = glm::scale(skyModel, glm::vec3(80.0f));
    shader->setMat4("uP", proj);
    shader->setMat4("uV", view);
    shader->setMat4("uM", skyModel);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    shader->setInt("uTex", 0);
    glBindVertexArray(skyVAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)skyVertexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
