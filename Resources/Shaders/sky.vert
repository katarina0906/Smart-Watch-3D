#version 330 core

layout (location = 0) in vec3 aPos;

out vec2 vUV;
out vec3 vDir;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uM;

void main()
{
    vec4 worldPos = uM * vec4(aPos, 1.0);
    vDir = normalize(aPos);
    /* Equirectangular UV za 2D sky teksturu */
    float u = 0.5 + atan(vDir.z, vDir.x) / (2.0 * 3.14159265);
    float v = 0.5 - asin(vDir.y) / 3.14159265;
    vUV = vec2(u, v);

    gl_Position = uP * uV * worldPos;
}
