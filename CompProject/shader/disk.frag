#version 430 core
// disk.frag
// Procedural accretion disk shader. Key techniques:
//  - Fractional Brownian Motion (fbm) for turbulent gas structure
//  - Spiral UV coordinate system for rotation
//  - Doppler-like colour asymmetry (one side hotter / brighter)
//  - Blinn-Phong specular highlight from the orbiting light source
//  - Back/front half split so the black hole sphere sits between them

out vec4 FragColor;

in vec3 vWorldPos;
in vec3 vLocalPos;

uniform float uTime;
uniform vec3  uLightPos;
uniform vec3  uCamPos;
uniform int   uDrawBack;  // 1 = back half, 0 = front half

// ── Noise helpers ────────────────────────────────────────────────────────────
float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Fractional Brownian Motion – 5 octaves of layered noise
float fbm(vec2 p)
{
    float value = 0.0;
    float amp   = 0.5;
    for (int i = 0; i < 5; i++)
    {
        value += noise(p) * amp;
        p     *= 2.0;
        amp   *= 0.5;
    }
    return value;
}

void main()
{
    vec2  p = vLocalPos.xz;
    float r = length(p);

    // Disk radial bounds (in local normalised space)
    const float inner = 1.32;
    const float outer = 3.25;

    if (r < inner || r > outer)
        discard;

    // Smooth edge mask – avoids hard clip artefacts
    float ringMask =
        smoothstep(inner, inner + 0.08, r) *
        (1.0 - smoothstep(outer - 0.14, outer, r));

    if (ringMask <= 0.001)
        discard;

    // Split disk: back rendered behind the sphere, front rendered in front
    float zSign = vLocalPos.z;
    if (uDrawBack == 1 && zSign < 0.0) discard;
    if (uDrawBack == 0 && zSign > 0.0) discard;

    float ang = atan(p.y, p.x);

    // Spiral UV – rotates with time to simulate orbital motion
    float spin  = ang + uTime * 1.1;
    vec2  flowUV = vec2(spin * 2.8 - r * 2.1, r * 3.6);

    // Layered turbulence using fbm
    float n1 = fbm(flowUV + vec2(0.0, uTime * 0.10));
    float n2 = fbm(flowUV * 1.9 + vec2(uTime * 0.18, -uTime * 0.07));
    float n3 = noise(vec2(spin * 12.0, r * 18.0 - uTime * 1.5));

    // Swirling streak structures (magnetic field filaments)
    float streaks = sin(spin * 20.0 - r * 26.0 - uTime * 5.5) * 0.5 + 0.5;
    streaks = pow(streaks, 6.0);

    float bands = sin(spin * 9.0 + r * 12.0 - uTime * 2.4) * 0.5 + 0.5;
    float wisps = sin(spin * 5.0 - r * 8.0 + uTime * 1.7) * 0.5 + 0.5;

    // Radial density fall-off
    float t        = smoothstep(inner, outer, r);
    float outerFade = 1.0 - smoothstep(inner, outer, r);
    float innerRim  = 1.0 - smoothstep(inner, inner + 0.15, r);
    innerRim = pow(innerRim, 3.5);

    // Travelling heat pulses (plasma instabilities)
    float pulse = sin(r * 18.0 - uTime * 4.0 + n2 * 3.0) * 0.5 + 0.5;
    pulse = pow(pulse, 3.0);

    float density =
        (0.18 +
         0.28 * n1 +
         0.18 * n2 +
         0.10 * n3 +
         0.10 * bands +
         0.08 * wisps +
         0.20 * streaks +
         0.18 * pulse) * pow(outerFade, 1.45);

    density *= ringMask;

    // Colour gradient: white-hot inner rim -> dark orange outer edge
    vec3 hotCol   = vec3(1.00, 0.98, 0.95);
    vec3 warmCol  = vec3(1.00, 0.74, 0.38);
    vec3 emberCol = vec3(0.95, 0.42, 0.12);
    vec3 outerCol = vec3(0.14, 0.04, 0.02);

    vec3 col = mix(hotCol, outerCol, t);
    col = mix(col, warmCol, 1.0 - smoothstep(inner, inner + 0.55, r));
    col = mix(col, emberCol, smoothstep(inner + 0.30, outer, r));

    // Doppler asymmetry: approaching side appears blue-shifted and brighter
    float doppler  = sin(ang - uTime * 0.20) * 0.5 + 0.5;
    vec3  warmShift = col * vec3(1.45, 0.78, 0.42);
    vec3  coolShift = col * vec3(0.72, 0.84, 1.08);
    col = mix(warmShift, coolShift, doppler);
    col *= mix(0.55, 1.20, doppler);

    // Blinn-Phong lighting from orbiting light source
    vec3  Nn = vec3(0.0, 1.0, 0.0);
    vec3  L  = normalize(uLightPos - vWorldPos);
    vec3  Vn = normalize(uCamPos   - vWorldPos);
    vec3  H  = normalize(L + Vn);
    float diff = max(dot(Nn, L), 0.0);
    float spec = pow(max(dot(Nn, H), 0.0), 96.0);
    col += vec3(1.0, 0.92, 0.80) * (diff * 0.06 + spec * 0.12) * ringMask;

    // Bright streak flashes
    col += hotCol * streaks * (0.25 + 0.75 * innerRim) * 0.95;

    // Intense inner rim emission
    col += hotCol * innerRim * 2.2;

    // Outer dust / nebula halo
    float dust = smoothstep(outer - 0.45, outer, r) * (0.25 + 0.75 * n1);
    col += vec3(0.30, 0.12, 0.06) * dust * 0.65;

    // Back half is slightly dimmer (occluded from the primary light)
    if (uDrawBack == 1)
        col *= 0.60;

    float alpha = clamp(density * (0.42 + 0.85 * innerRim + 0.20 * pulse), 0.0, 1.0);

    FragColor = vec4(col * density * 2.8, alpha);
}
