#version 330 core
out vec4 FragColor;

in vec3 chNormal;
in vec3 chFragPos;
in vec2 chUV;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform sampler2D uDiffMap1;

uniform vec3 uLightDir;
uniform vec3 uScreenLightPos;
uniform vec3 uScreenLightColor;
uniform float uScreenLightStrength;

uniform float uDebugMode;
uniform vec3 uDebugColor;

void main()
{
    if (uDebugMode > 0.5) {
        FragColor = vec4(uDebugColor, 1.0);
        return;
    }

    vec4 texColor = texture(uDiffMap1, chUV);
    vec3 baseColor = texColor.rgb;
    if (texColor.r == 1.0 && texColor.g == 1.0 && texColor.b == 1.0) {
        baseColor = vec3(0.95, 0.8, 0.7);
    }
    
    vec3 norm = normalize(chNormal);

    float ambientStrength = 0.18;  /* usklađeno s mesh – vide se sjene */
    vec3 ambient = ambientStrength * uLightColor;

    vec3 lightDir = normalize(uLightPos - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 sunDir = normalize(-uLightDir);
    float sunDiff = max(dot(norm, sunDir), 0.0);
    vec3 sunLight = sunDiff * uLightColor * 0.8;

    vec3 toScreen = uScreenLightPos - chFragPos;
    float dist = length(toScreen);
    vec3 screenL = normalize(toScreen);
    float screenDiff = max(dot(norm, screenL), 0.0);
    float atten = 1.0 / (1.0 + dist * dist);
    vec3 screenLight = screenDiff * uScreenLightColor * atten * uScreenLightStrength;

    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * uLightColor;

    vec3 finalColor = baseColor * (ambient + diffuse + sunLight + screenLight + specular);

    FragColor = vec4(finalColor, 1.0);
}
