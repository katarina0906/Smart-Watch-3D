#include "../Header/Renderer.h"
#include "../Header/Util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <map>
#include <string>

unsigned int tex_watchFrame = 0;
unsigned int tex_nameplate = 0;

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID;
    int SizeX;
    int SizeY;
    int BearingX;
    int BearingY;
    unsigned int Advance;
};

static std::map<char, Character> Characters;
static unsigned int textVAO, textVBO;
static unsigned int textShader;

unsigned int tex_arrowLeft;
unsigned int tex_arrowRight;



void Renderer_Init()
{

    tex_watchFrame = loadImageToTexture("Resources/Textures/watch_frame.png");
    tex_nameplate = loadImageToTexture("Resources/Textures/nameplate2.png");
    if (tex_nameplate) {
        glBindTexture(GL_TEXTURE_2D, tex_nameplate);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (tex_watchFrame == 0)
        std::cout << "WATCH FRAME NIJE ucitan!\n";
    else
        std::cout << "WATCH FRAME ucitan, ID = " << tex_watchFrame << std::endl;

    glBindTexture(GL_TEXTURE_2D, tex_watchFrame);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    textShader = createShader(
        "Resources/Shaders/Fonts/text.vert",
        "Resources/Shaders/Fonts/text.frag"
    );
    std::cout << "Text shader ID = " << textShader << std::endl;


    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library\n";
    }

    FT_Face face;
    if (FT_New_Face(ft, "Resources/Fonts/arial.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font\n";
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph " << c << "\n";
            continue;
        }

        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            tex,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            (unsigned int)face->glyph->advance.x
        };

        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(4 * sizeof(float)), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Renderer_Init: TEXT SYSTEM READY.\n";
}




void drawTexturedQuad(
    unsigned int shader,
    unsigned int vao,
    float x, float y,
    float sx, float sy,
    unsigned int tex)
{
    glUseProgram(shader);

    glUniform2f(glGetUniformLocation(shader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader, "uScale"), sx, sy);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void drawQuadRect(
    unsigned int shader,
    unsigned int vao,
    float x, float y,
    float sx, float sy,
    float r, float g, float b, float a)
{
    glUseProgram(shader);

    glUniform2f(glGetUniformLocation(shader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader, "uScale"), sx, sy);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    glUniform4f(glGetUniformLocation(shader, "uColor"), r, g, b, a);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}




void Renderer_DrawText(
    const char* text,
    float x, float y,
    float scale,
    float r, float g, float b)
{
    glUseProgram(textShader);

    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);

    float projection[16] = {
         1,0,0,0,
         0,1,0,0,
         0,0,1,0,
         0,0,0,1
    };
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, projection);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    std::string t(text);
    for (char c : t)
    {
        Character ch = Characters[c];

        float xpos = x + ch.BearingX * scale;
        float ypos = y - (ch.SizeY - ch.BearingY) * scale;

        float w = ch.SizeX * scale;
        float h = ch.SizeY * scale;

        float verts[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer_DrawTextScreen(
    const char* text,
    float x, float y,
    float scale,
    float r, float g, float b,
    int screenW, int screenH)
{
    if (screenW <= 0 || screenH <= 0) return;
    glm::mat4 proj = glm::ortho(0.0f, (float)screenW, 0.0f, (float)screenH);

    glUseProgram(textShader);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, &proj[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    std::string t(text);
    for (char c : t)
    {
        Character ch = Characters[c];

        float xpos = x + ch.BearingX * scale;
        float ypos = y - (ch.SizeY - ch.BearingY) * scale;

        float w = ch.SizeX * scale;
        float h = ch.SizeY * scale;

        float verts[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
