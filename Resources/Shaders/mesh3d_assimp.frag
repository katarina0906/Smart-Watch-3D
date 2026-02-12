#version 330 core

out vec4 FragColor;

in vec2 vUV;
in vec3 vWorldPos;
in vec3 vNormal;

uniform sampler2D uTex;
uniform float uGroundScrollOffset;  /* offset UV.y za podlogu (0 = bez efekta) */
uniform float uBrightenGround;      /* 1=cesta/trotuar (svetlija da se vide sjenke), 0=zgrade */
uniform float uDebugSolidColor;
uniform vec3 uSolidColor;  /* kad uDebugSolidColor>0.5: baseColor = uSolidColor */

/* ---------- Fongov model - strukture kao profesor ---------- */
struct Material {
    vec3 kA;
    vec3 kD;
    vec3 kS;
    float shine;
};

/* ---------- Direkciono svetlo (sunce) ---------- */
uniform vec3 uLightDir;      // smer od tela ka svetlu
uniform vec3 uLightColor;    // kA=kD=kS za sunce

/* ---------- Točkasto svetlo ekrana sata ---------- */
uniform vec3 uScreenLightPos;
uniform vec3 uScreenLightColor;
uniform float uScreenLightStrength;

/* ---------- Materijal ---------- */
uniform Material uMaterial;

/* ---------- Ostalo ---------- */
uniform float uAmbientStrength;
uniform float uEmissive;     // >0 samo za ekran sata
uniform vec3 uViewPos;       // pozicija kamere (za spekular)

/* ---------- Shadow map ---------- */
uniform sampler2D uShadowMap;
uniform mat4 uLightSpace;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0 || projCoords.z < 0.0) return 0.0;
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.003 * (1.0 - dot(normal, lightDir)), 0.001);
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

void main()
{
    vec2 uv = vUV;
    if (uGroundScrollOffset != 0.0)
        uv.y = fract(vUV.y + uGroundScrollOffset);
    vec3 baseColor = texture(uTex, uv).rgb;
    if (uBrightenGround > 0.5)
        baseColor = baseColor * 1.4 + 0.12;
    if (uDebugSolidColor > 0.5)
        baseColor = uSolidColor;

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    /* -------- Ambient -------- */
    vec3 ambient = baseColor * uAmbientStrength;

    /* -------- Direkciono svetlo (sunce): diffuse + spekular + sjena -------- */
    vec3 L_sun = normalize(-uLightDir);
    float nDotL_sun = max(dot(N, L_sun), 0.0);
    vec4 fragPosLight = uLightSpace * vec4(vWorldPos, 1.0);
    float shadow = ShadowCalculation(fragPosLight, N, L_sun);
    float sunVisibility = 1.0 - shadow;
    vec3 diffSun = baseColor * uLightColor * nDotL_sun * sunVisibility;

    vec3 R_sun = reflect(-L_sun, N);
    float specSun = pow(max(dot(V, R_sun), 0.0), uMaterial.shine);
    vec3 specContribSun = uLightColor * uMaterial.kS * specSun * sunVisibility;

    /* -------- Točkasto svetlo ekrana: diffuse + spekular -------- */
    vec3 toScreen = uScreenLightPos - vWorldPos;
    float dist = length(toScreen);
    vec3 L_screen = normalize(toScreen);
    float atten = 1.0 / (1.0 + dist * dist);

    float nDotL_screen = max(dot(N, L_screen), 0.0);
    vec3 diffScreen = baseColor * uScreenLightColor * nDotL_screen * atten * uScreenLightStrength;

    vec3 R_screen = reflect(-L_screen, N);
    float specScreen = pow(max(dot(V, R_screen), 0.0), uMaterial.shine);
    vec3 specContribScreen = uScreenLightColor * uMaterial.kS * specScreen * atten * uScreenLightStrength;

    /* -------- Emissive (ekran sata) -------- */
    vec3 emissive = baseColor * uEmissive;

    /* -------- Finalna boja: Fongov model -------- */
    vec3 color = ambient
        + diffSun + specContribSun
        + diffScreen + specContribScreen
        + emissive;

    FragColor = vec4(color, 1.0);
}
