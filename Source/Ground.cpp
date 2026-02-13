#include "../Header/Ground.h"
#include "../Header/shader.hpp"
#include "../Header/Util.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>

static std::vector<GroundSegment> ground;

static float groundScrollOffset = 0.0f;

static GLuint groundVAO = 0;
static GLuint groundVBO = 0;
static GLuint groundTex = 0;
static GLuint sideGroundVAO = 0;
static GLuint sideGroundVBO = 0;
static GLuint sideGroundTex = 0;
static GLuint buildingTex = 0;
static std::vector<GLuint> buildingTextures;
static GLuint skinTex = 0;
static GLuint cubeVAO = 0;
static GLuint cubeVBO = 0;
static GLuint buildingQuadVAO = 0;
static GLuint buildingQuadVBO = 0;
static const int   SEG_COUNT = 12;
static const float BUILDING_OFFSET = 2.0f;
static const float BUILDING_SIZE = 2.0f;
static const float SEG_LEN = 6.0f;
static const float ROAD_W = 6.0f;
static const float SIDE_GROUND_EXTENT = 14.0f;
static const float Y_LEVEL = 0.0f;
static const float GROUND_FRONT_Z = 4.5f;
static const float GROUND_MESH_LEN = 120.0f;

static GLuint CreateGroundTexture(int w = 512, int h = 512)
{
    std::vector<unsigned char> data(w * h * 3);
    const float invW = 1.0f / w;
    const float invH = 1.0f / h;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float u = (x + 0.5f) * invW;
            float v = (y + 0.5f) * invH;
            int i = (y * w + x) * 3;
            unsigned char baseR = 135, baseG = 132, baseB = 128;
            float noise = 0.90f + 0.20f * (float)((x * 31 + y * 17) % 127) / 127.0f;
            unsigned char R = (unsigned char)(baseR * noise);
            unsigned char G = (unsigned char)(baseG * noise);
            unsigned char B = (unsigned char)(baseB * noise);

            float uCycle = u - floorf(u);
            if (uCycle >= 0.46f && uCycle <= 0.54f)
            {
                R = 200; G = 185; B = 60;
            }
            else if ((uCycle >= 0.22f && uCycle <= 0.28f) || (uCycle >= 0.72f && uCycle <= 0.78f))
            {
                R = 220; G = 218; B = 210;
            }
            const float edge = 0.06f;
            if (u < edge || u > 1.0f - edge)
            {
                float t = (u < edge) ? (u / edge) : ((1.0f - u) / edge);
                unsigned char edgeR = 165, edgeG = 160, edgeB = 155;
                R = (unsigned char)(R * (1.0f - t) + edgeR * t);
                G = (unsigned char)(G * (1.0f - t) + edgeG * t);
                B = (unsigned char)(B * (1.0f - t) + edgeB * t);
            }

            data[i] = R;
            data[i + 1] = G;
            data[i + 2] = B;
        }
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

static GLuint CreateBuildingTexture(int w = 512, int h = 512)
{
    const int cols = 4;
    const int rows = 10;
    const float frame = 0.12f;
    const unsigned char wallR = 175, wallG = 168, wallB = 158;
    const unsigned char glassR = 50,  glassG = 75,  glassB = 105;

    std::vector<unsigned char> data(w * h * 3);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float u = (x + 0.5f) / float(w);
            float v = (y + 0.5f) / float(h);
            float cellU = u * cols - floorf(u * cols);
            float cellV = v * rows - floorf(v * rows);
            unsigned char r, g, b;
            if (cellU >= frame && cellU <= 1.0f - frame && cellV >= frame && cellV <= 1.0f - frame)
            {
                r = glassR; g = glassG; b = glassB;
            }
            else
            {
                float n = 0.92f + 0.16f * (float)((x * 7 + y * 13) % 101) / 101.0f;
                r = (unsigned char)(wallR * n);
                g = (unsigned char)(wallG * n);
                b = (unsigned char)(wallB * n);
            }
            int i = (y * w + x) * 3;
            data[i] = r; data[i + 1] = g; data[i + 2] = b;
        }
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

static GLuint CreateSkinTexture(int w = 64, int h = 64)
{
    std::vector<unsigned char> data(w * h * 3);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
        {
            float n = 0.96f + 0.08f * (float)((x * 11 + y * 17) % 97) / 97.0f;
            int i = (y * w + x) * 3;
            data[i] = (unsigned char)(245 * n);
            data[i + 1] = (unsigned char)(215 * n);
            data[i + 2] = (unsigned char)(195 * n);
        }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

static void CreateBuildingQuadVAO()
{
    float quad[] = {
         0.5f, -0.5f,  0.5f,   1,0,0,  0,0,
         0.5f, -0.5f, -0.5f,   1,0,0,  1,0,
         0.5f,  0.5f, -0.5f,   1,0,0,  1,1,
         0.5f, -0.5f,  0.5f,   1,0,0,  0,0,
         0.5f,  0.5f, -0.5f,   1,0,0,  1,1,
         0.5f,  0.5f,  0.5f,   1,0,0,  0,1,
        -0.5f, -0.5f, -0.5f,  -1,0,0,  0,0,
        -0.5f, -0.5f,  0.5f,  -1,0,0,  1,0,
        -0.5f,  0.5f,  0.5f,  -1,0,0,  1,1,
        -0.5f, -0.5f, -0.5f,  -1,0,0,  0,0,
        -0.5f,  0.5f,  0.5f,  -1,0,0,  1,1,
        -0.5f,  0.5f, -0.5f,  -1,0,0,  0,1,
        -0.5f, -0.5f,  0.5f,  0,0,1,  0,0,
         0.5f, -0.5f,  0.5f,  0,0,1,  1,0,
         0.5f,  0.5f,  0.5f,  0,0,1,  1,1,
        -0.5f, -0.5f,  0.5f,  0,0,1,  0,0,
         0.5f,  0.5f,  0.5f,  0,0,1,  1,1,
        -0.5f,  0.5f,  0.5f,  0,0,1,  0,1,
         0.5f, -0.5f, -0.5f,  0,0,-1,  0,0,
        -0.5f, -0.5f, -0.5f,  0,0,-1,  1,0,
        -0.5f,  0.5f, -0.5f,  0,0,-1,  1,1,
         0.5f, -0.5f, -0.5f,  0,0,-1,  0,0,
        -0.5f,  0.5f, -0.5f,  0,0,-1,  1,1,
         0.5f,  0.5f, -0.5f,  0,0,-1,  0,1
    };
    glGenVertexArrays(1, &buildingQuadVAO);
    glGenBuffers(1, &buildingQuadVBO);
    glBindVertexArray(buildingQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, buildingQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

static void CreateCubeVAO()
{
    float cube[] = {
        -1,-1, 1,  0,0,1,  0,0,   1,-1, 1,  0,0,1,  1,0,   1, 1, 1,  0,0,1,  1,1,
        -1,-1, 1,  0,0,1,  0,0,   1, 1, 1,  0,0,1,  1,1,  -1, 1, 1,  0,0,1,  0,1,
         1,-1,-1,  0,0,-1,  0,0,  -1,-1,-1,  0,0,-1,  1,0,  -1, 1,-1,  0,0,-1,  1,1,
         1,-1,-1,  0,0,-1,  0,0,  -1, 1,-1,  0,0,-1,  1,1,   1, 1,-1,  0,0,-1,  0,1,
         1,-1, 1,  1,0,0,  0,0,   1,-1,-1,  1,0,0,  1,0,   1, 1,-1,  1,0,0,  1,1,
         1,-1, 1,  1,0,0,  0,0,   1, 1,-1,  1,0,0,  1,1,   1, 1, 1,  1,0,0,  0,1,
        -1,-1,-1, -1,0,0,  0,0,  -1,-1, 1, -1,0,0,  1,0,  -1, 1, 1, -1,0,0,  1,1,
        -1,-1,-1, -1,0,0,  0,0,  -1, 1, 1, -1,0,0,  1,1,  -1, 1,-1, -1,0,0,  0,1,
        -1, 1, 1,  0,1,0,  0,0,   1, 1, 1,  0,1,0,  1,0,   1, 1,-1,  0,1,0,  1,1,
        -1, 1, 1,  0,1,0,  0,0,   1, 1,-1,  0,1,0,  1,1,  -1, 1,-1,  0,1,0,  0,1,
        -1,-1,-1,  0,-1,0,  0,0,   1,-1,-1,  0,-1,0,  1,0,   1,-1, 1,  0,-1,0,  1,1,
        -1,-1,-1,  0,-1,0,  0,0,   1,-1, 1,  0,-1,0,  1,1,  -1,-1, 1,  0,-1,0,  0,1
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Ground_Init()
{
    ground.clear();
    for (int i = 0; i < SEG_COUNT; i++)
        ground.push_back({ GROUND_FRONT_Z - (float)i * SEG_LEN });

    float verts[] = {
        -ROAD_W * 0.5f, Y_LEVEL,  0.0f,           0,1,0,  0.0f, 0.0f,
         ROAD_W * 0.5f, Y_LEVEL,  0.0f,           0,1,0,  1.0f, 0.0f,
         ROAD_W * 0.5f, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  1.0f, 1.0f,
        -ROAD_W * 0.5f, Y_LEVEL,  0.0f,           0,1,0,  0.0f, 0.0f,
         ROAD_W * 0.5f, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  1.0f, 1.0f,
        -ROAD_W * 0.5f, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    float sideLeft = -ROAD_W * 0.5f - SIDE_GROUND_EXTENT;
    float sideRight = ROAD_W * 0.5f + SIDE_GROUND_EXTENT;
    float vertsSide[] = {
        sideLeft, Y_LEVEL,  0.0f,           0,1,0,  0.0f, 0.0f,
        -ROAD_W * 0.5f, Y_LEVEL,  0.0f,    0,1,0,  1.0f, 0.0f,
        -ROAD_W * 0.5f, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  1.0f, 1.0f,
        sideLeft, Y_LEVEL,  0.0f,           0,1,0,  0.0f, 0.0f,
        -ROAD_W * 0.5f, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  1.0f, 1.0f,
        sideLeft, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  0.0f, 1.0f,
        ROAD_W * 0.5f, Y_LEVEL,  0.0f,    0,1,0,  0.0f, 0.0f,
        sideRight, Y_LEVEL,  0.0f,         0,1,0,  1.0f, 0.0f,
        sideRight, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  1.0f, 1.0f,
        ROAD_W * 0.5f, Y_LEVEL,  0.0f,    0,1,0,  0.0f, 0.0f,
        sideRight, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  1.0f, 1.0f,
        ROAD_W * 0.5f, Y_LEVEL, -GROUND_MESH_LEN, 0,1,0,  0.0f, 1.0f
    };
    glGenVertexArrays(1, &sideGroundVAO);
    glGenBuffers(1, &sideGroundVBO);
    glBindVertexArray(sideGroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sideGroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertsSide), vertsSide, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    sideGroundTex = loadImageToTexture("Resources/Textures/close-up-bright-glitter.jpg");
    if (sideGroundTex != 0)
    {
        glBindTexture(GL_TEXTURE_2D, sideGroundTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    groundTex = loadImageToTexture("Resources/Textures/ground.png");
    if (groundTex == 0)
        groundTex = loadImageToTexture("Resources/Textures/ground.jpg");
    if (groundTex == 0)
        groundTex = CreateGroundTexture();
    else
    {
        glBindTexture(GL_TEXTURE_2D, groundTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    buildingTextures.clear();
    const char* texPaths[] = {
        "Resources/Textures/building1.jpg",
        "Resources/Textures/building2.jpg"
    };
    for (const char* p : texPaths)
    {
        GLuint tex = loadImageToTexture(p);
        if (tex != 0)
        {
            buildingTextures.push_back(tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    if (buildingTextures.empty())
    {
        buildingTex = CreateBuildingTexture();
        buildingTextures.push_back(buildingTex);
    }
    else
    {
        buildingTex = buildingTextures[0];
    }
    skinTex = CreateSkinTexture();
    CreateCubeVAO();
    CreateBuildingQuadVAO();
}

void Ground_Update(float dt, float speed, float cameraZ)
{
    if (speed <= 0.0f) return;

    for (auto& s : ground)
        s.z += speed * dt;
    groundScrollOffset += speed * dt / GROUND_MESH_LEN;
    if (groundScrollOffset >= 1.0f) groundScrollOffset -= 1.0f;

    const float resetBehindCameraZ = cameraZ + 6.0f;

    float minZ = ground[0].z;
    for (auto& s : ground)
        minZ = std::min(minZ, s.z);

    for (auto& s : ground)
    {
        if (s.z > resetBehindCameraZ)
        {
            s.z = minZ - SEG_LEN;
            minZ = s.z;
        }
    }
}

void Ground_Render(Shader& shader,
    const glm::mat4& proj,
    const glm::mat4& view,
    const glm::vec3& cameraPos)
{
    const GLuint shaderId = shader.ID;
    shader.use();
    glUniform1f(glGetUniformLocation(shaderId, "uEmissive"), 0.0f);

    const float groundBaseZ = cameraPos.z + GROUND_FRONT_Z;

    glPolygonOffset(0.5f, 0.5f);
    glEnable(GL_POLYGON_OFFSET_FILL);

    if (sideGroundTex != 0 && sideGroundVAO != 0)
    {
        shader.setFloat("uGroundScrollOffset", groundScrollOffset);
        shader.setFloat("uBrightenGround", 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sideGroundTex);
        glUniform1i(glGetUniformLocation(shaderId, "uTex"), 0);
        glm::mat4 modelSide = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, groundBaseZ));
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &modelSide[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uP"), 1, GL_FALSE, &proj[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uV"), 1, GL_FALSE, &view[0][0]);
        glBindVertexArray(sideGroundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTex);
    glUniform1i(glGetUniformLocation(shaderId, "uTex"), 0);
    shader.setFloat("uGroundScrollOffset", groundScrollOffset);
    shader.setFloat("uBrightenGround", 1.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, groundBaseZ));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "uP"), 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "uV"), 1, GL_FALSE, &view[0][0]);
    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_POLYGON_OFFSET_FILL);

    shader.setFloat("uGroundScrollOffset", 0.0f);
    shader.setFloat("uBrightenGround", 0.0f);
    glUniform1f(glGetUniformLocation(shaderId, "uEmissive"), 0.0f);
    glBindVertexArray(buildingQuadVAO);
    int segIndex = 0;
    for (auto& s : ground)
    {
        float segCenterZ = s.z - SEG_LEN * 0.5f;
        float hL = 1.4f + 0.35f * (segIndex % 6);
        float wL = 1.6f + 0.2f * (segIndex % 3);
        float dL = 1.4f + 0.15f * (segIndex % 4);
        float hR = 1.6f + 0.4f * ((segIndex + 2) % 5);
        float wR = 1.5f + 0.25f * ((segIndex + 1) % 4);
        float dR = 1.5f + 0.2f * (segIndex % 3);
        glm::vec3 leftPos(-ROAD_W * 0.5f - BUILDING_OFFSET - wL * 0.5f, Y_LEVEL + hL, segCenterZ);
        glm::vec3 rightPos( ROAD_W * 0.5f + BUILDING_OFFSET + wR * 0.5f, Y_LEVEL + hR, segCenterZ);
        glm::mat4 scaleL = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * dL, 2.0f * hL, 2.0f * dL));
        glm::mat4 scaleR = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * dR, 2.0f * hR, 2.0f * dR));
        glm::mat4 modelL = glm::translate(glm::mat4(1.0f), leftPos) * scaleL;
        glm::mat4 modelR = glm::translate(glm::mat4(1.0f), rightPos)
            * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) * scaleR;
        GLuint tex = buildingTextures[segIndex % (int)buildingTextures.size()];
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(shaderId, "uTex"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &modelL[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uP"), 1, GL_FALSE, &proj[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uV"), 1, GL_FALSE, &view[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &modelR[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        segIndex++;
    }

    glBindVertexArray(0);
}

void Ground_RenderShadow(Shader& shadowShader, const glm::mat4& lightSpace, const glm::vec3& cameraPos)
{
    const GLuint shaderId = shadowShader.ID;
    shadowShader.use();
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "uLightSpace"), 1, GL_FALSE, &lightSpace[0][0]);

    const float groundBaseZ = cameraPos.z + GROUND_FRONT_Z;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, groundBaseZ));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if (sideGroundVAO != 0)
    {
        glBindVertexArray(sideGroundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    glBindVertexArray(buildingQuadVAO);
    int segIndex = 0;
    for (auto& s : ground)
    {
        float segCenterZ = s.z - SEG_LEN * 0.5f;
        float hL = 1.4f + 0.35f * (segIndex % 6);
        float wL = 1.6f + 0.2f * (segIndex % 3);
        float dL = 1.4f + 0.15f * (segIndex % 4);
        float hR = 1.6f + 0.4f * ((segIndex + 2) % 5);
        float wR = 1.5f + 0.25f * ((segIndex + 1) % 4);
        float dR = 1.5f + 0.2f * (segIndex % 3);
        glm::vec3 leftPos(-ROAD_W * 0.5f - BUILDING_OFFSET - wL * 0.5f, Y_LEVEL + hL, segCenterZ);
        glm::vec3 rightPos( ROAD_W * 0.5f + BUILDING_OFFSET + wR * 0.5f, Y_LEVEL + hR, segCenterZ);
        glm::mat4 scaleL = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * dL, 2.0f * hL, 2.0f * dL));
        glm::mat4 scaleR = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * dR, 2.0f * hR, 2.0f * dR));
        glm::mat4 modelL = glm::translate(glm::mat4(1.0f), leftPos) * scaleL;
        glm::mat4 modelR = glm::translate(glm::mat4(1.0f), rightPos)
            * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) * scaleR;
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &modelL[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glUniformMatrix4fv(glGetUniformLocation(shaderId, "uM"), 1, GL_FALSE, &modelR[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        segIndex++;
    }
    glBindVertexArray(0);
}

void Ground_DrawCube(unsigned int shader,
    const glm::mat4& proj,
    const glm::mat4& view,
    const glm::mat4& model)
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uP"), 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uV"), 1, GL_FALSE, &view[0][0]);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

GLuint Ground_GetBuildingTexture()
{
    return buildingTex;
}

GLuint Ground_GetSkinTexture()
{
    return skinTex;
}
