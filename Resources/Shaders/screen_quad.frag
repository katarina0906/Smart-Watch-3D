#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTex;
uniform float uAspect;
uniform float uCircleRadius;
uniform float uBezelWidth;
uniform vec3 uBezelColor;

float gearShape(vec2 p, float teeth)
{
    float ang = atan(p.y, p.x);
    float dist = length(p) * 2.0;
    float R = 0.14;
    float A = 0.025;
    float r_gear = R + A * cos(teeth * ang);
    return smoothstep(r_gear - 0.02, r_gear + 0.02, dist);
}

void main()
{
    vec2 cen = (vUV - 0.5) * vec2(uAspect, 1.0);
    float r = length(cen) * 2.0;

    vec2 cenTop = vec2(cen.x, cen.y - 0.48);
    vec2 cenBot = vec2(cen.x, cen.y + 0.48);
    float gearTop = 1.0 - gearShape(cenTop, 10.0);
    float gearBot = 1.0 - gearShape(cenBot, 10.0);
    float inGear = max(gearTop, gearBot);

    if (r > uCircleRadius) {
        if (inGear > 0.5) {
            vec3 edge = uBezelColor * 0.5;
            vec3 mid = uBezelColor * 1.0;
            FragColor = vec4(mix(edge, mid, inGear), 1.0);
        } else {
            discard;
        }
        return;
    }

    if (r > uCircleRadius - uBezelWidth) {
        float t = (r - (uCircleRadius - uBezelWidth)) / uBezelWidth;
        vec3 edge = uBezelColor * 0.6;
        vec3 face = uBezelColor * 1.2;
        FragColor = vec4(mix(edge, face, t), 1.0);
    } else {
        FragColor = texture(uTex, vUV);
    }
}
