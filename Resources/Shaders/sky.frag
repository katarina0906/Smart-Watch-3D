#version 330 core

out vec4 FragColor;

in vec2 vUV;
in vec3 vDir;

uniform sampler2D uTex;

void main()
{
    FragColor = texture(uTex, vUV);
}
