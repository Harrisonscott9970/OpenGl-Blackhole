#version 430 core
// platform.frag
// Multi-purpose Blinn-Phong fragment shader used for:
//   - Satellite landing platforms (diffuse texture + normal map)
//   - Energy cell pickups (emissive pulsing core mode)
//   - Asteroids (colour tinted by proximity to black hole)
//
// Lighting techniques:
//   - Blinn-Phong with two point lights
//   - Exponential depth fog (lighting sub-technique)
//
// Texturing techniques:
//   - Diffuse texture sampling
//   - Normal map via TBN matrix
//   - Pixel discard (sparkle dissolve on energy cells)

out vec4 FragColor;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in mat3 vTBN;

uniform vec3  uLightPos;
uniform vec3  uCamPos;
uniform float uTime;
uniform vec3  uBHPos;

uniform vec3  uLightPos2;
uniform vec3  uLightColor2;
uniform float uLightStrength2;

uniform vec3  uBaseColor;
uniform vec3  uGlowColor;
uniform float uGlowStrength;
uniform int   uIsCore;

uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalTex;
uniform int       uHasDiffuse;
uniform int       uHasNormal;

// ── Fog ──────────────────────────────────────────────────────────────────────
// Exponential squared fog: f = e^(-(density * dist)^2)
// Deep-space purple-black so distant objects fade into the void.
const float FOG_DENSITY = 0.00042;
const vec3  FOG_COLOR   = vec3(0.01, 0.01, 0.04);

float computeFog()
{
    float dist = length(uCamPos - vWorldPos);
    float f    = exp(-pow(FOG_DENSITY * dist, 2.0));
    return clamp(f, 0.0, 1.0);
}

// ── Blinn-Phong helper ───────────────────────────────────────────────────────
vec3 applyLight(
    vec3  N, vec3  V,
    vec3  lightPos, vec3  lightCol,
    float intensity, vec3  baseCol,
    float shininess, float specScale)
{
    vec3  L    = normalize(lightPos - vWorldPos);
    vec3  H    = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess) * specScale;
    return baseCol * lightCol * diff * intensity
         + lightCol * spec * intensity;
}

void main()
{
    // ── Normal ───────────────────────────────────────────────────────────────
    vec3 N;
    if (uHasNormal == 1)
    {
        // Unpack normal map and transform to world space via TBN
        vec3 sampledN = texture(uNormalTex, vUV).rgb * 2.0 - 1.0;
        N = normalize(vTBN * sampledN);
    }
    else
    {
        N = normalize(vNormal);
    }

    vec3 V = normalize(uCamPos - vWorldPos);

    // ── Energy cell / emissive core mode ─────────────────────────────────────
    if (uIsCore == 1)
    {
        vec3  albedo  = (uHasDiffuse == 1) ? texture(uDiffuseTex, vUV).rgb : uBaseColor;
        float pulse   = 0.55 + 0.45 * sin(uTime * 4.0 + vUV.y * 12.0);
        float fresnel = pow(1.0 - max(dot(N, V), 0.0), 3.0);

        // ── Pixel Discard (texturing sub-technique) ───────────────────────────
        // Animated sparkle dissolve: discards fragments whose UV-space hash
        // falls below a time-animated threshold. Creates a crackling energy
        // shell effect with no extra geometry.
        float sparkle   = fract(sin(dot(vUV * 18.3, vec2(127.1, 311.7))) * 43758.5);
        float threshold = 0.08 + 0.06 * sin(uTime * 3.5 + vUV.x * 20.0);
        if (sparkle < threshold) discard;

        vec3 col = albedo * 0.22;
        col += applyLight(N, V, uLightPos, vec3(1.0), 0.75, albedo, 64.0, 0.20);
        if (uLightStrength2 > 0.0)
            col += applyLight(N, V, uLightPos2, uLightColor2, uLightStrength2, albedo, 96.0, 0.18);
        col += uGlowColor * uGlowStrength * (0.45 + pulse * 0.55);
        col += uGlowColor * fresnel * 0.90;

        if (uHasNormal == 1)
        {
            vec3  H1     = normalize(normalize(uLightPos - vWorldPos) + V);
            float nmSpec = pow(max(dot(N, H1), 0.0), 128.0) * 0.45;
            col += uGlowColor * nmSpec;
        }

        col = mix(FOG_COLOR, col, computeFog());
        FragColor = vec4(col, 1.0);
        return;
    }

    // ── Platform / asteroid mode ──────────────────────────────────────────────
    vec3 baseCol = (uHasDiffuse == 1) ? texture(uDiffuseTex, vUV).rgb : uBaseColor;

    float gridX = abs(fract(vUV.x * 6.0) - 0.5);
    float gridY = abs(fract(vUV.y * 6.0) - 0.5);
    float grid  = smoothstep(0.46, 0.50, max(gridX, gridY));
    float pulse = sin(uTime * 1.5) * 0.5 + 0.5;

    vec3 surfaceCol = baseCol;
    if (uHasDiffuse == 1)
    {
        surfaceCol *= 0.80 + 0.20 * pulse;
        surfaceCol  = mix(surfaceCol, surfaceCol * 1.12, grid * 0.18);
    }
    else
    {
        surfaceCol  = mix(baseCol, uGlowColor * 0.22, grid * 0.55);
        surfaceCol += uGlowColor * grid * pulse * 0.05;
    }

    vec3 finalCol = surfaceCol * 0.10;
    finalCol += applyLight(N, V, uLightPos, vec3(1.0), 0.90, surfaceCol, 64.0, 0.24);
    if (uLightStrength2 > 0.0)
        finalCol += applyLight(N, V, uLightPos2, uLightColor2, uLightStrength2, surfaceCol, 80.0, 0.18);

    float edgeDist  = length(vUV - vec2(0.5)) * 2.0;
    float rim       = smoothstep(0.82, 1.0, edgeDist);
    float hazePulse = 0.65 + 0.35 * sin(uTime * 2.2 + vUV.x * 8.0 + vUV.y * 10.0);
    finalCol += uGlowColor * rim * (0.10 + 0.12 * pulse) * uGlowStrength;
    finalCol += uGlowColor * rim * hazePulse * 0.10;

    if (uHasDiffuse == 1)
    {
        float scratchGlow = smoothstep(0.35, 0.48, grid) * 0.08;
        finalCol += uLightColor2 * scratchGlow * uLightStrength2;
    }

    // ── Exponential fog ───────────────────────────────────────────────────────
    // Distant platforms/asteroids fade into the dark void of space.
    finalCol = mix(FOG_COLOR, finalCol, computeFog());

    FragColor = vec4(finalCol, 1.0);
}