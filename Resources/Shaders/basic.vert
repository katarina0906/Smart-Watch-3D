#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform vec2 uPos;      // centar [-1,1]
uniform vec2 uScale;    // sirina/visina

void main()
{
    vec2 scaled   = aPos * uScale;
    vec2 finalPos = scaled + uPos;

    gl_Position = vec4(finalPos, 0.0, 1.0);
    TexCoord    = aTex;
}
