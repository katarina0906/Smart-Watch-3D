#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTex;
uniform float uAspect;
uniform float uCircleRadius;

/* Okrugao sat – krug = ekran, van kruga transparentno (ruka se vidi) */
void main()
{
    vec2 cen = (vUV - 0.5) * vec2(uAspect, 1.0);
    float r = length(cen) * 2.0;

    if (r > uCircleRadius)
        discard;

    FragColor = texture(uTex, vUV);
}
