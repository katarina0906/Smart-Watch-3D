#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uM;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = uP * uV * uM * vec4(aPos, 1.0);
}
