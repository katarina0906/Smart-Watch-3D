#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4      uColor;
uniform bool      useTexture;

void main()
{
    if (useTexture)
        FragColor = texture(uTexture, TexCoord);
    else
        FragColor = uColor;
}
