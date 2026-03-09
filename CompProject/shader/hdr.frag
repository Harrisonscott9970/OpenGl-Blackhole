#version 430 core
// hdr.frag
// Post-processing final composite pass. Implements:
//   1. HDR Tone Mapping  – Reinhard (fallback) or Exponential (enabled)
//   2. Bloom compositing – adds the Gaussian-blurred bright-pass texture
//   3. Gamma Correction  – sRGB transfer function (gamma = 2.2)
//   4. Film Mode         – chromatic aberration, colour grading, vignette, grain
//   5. Black Hole lens haze – subtle screen-space flare when looking at the BH

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTex;       // HDR scene colour buffer
uniform sampler2D bloomBlurTex;   // Gaussian-blurred bright-pass
uniform int       hdrEnabled;     // Toggle exponential tone mapping
uniform int       bloomEnabled;   // Toggle bloom composite
uniform float     exposure;       // Camera exposure (EV)
uniform int       filmMode;       // Toggle film post-process
uniform float     uTime;

// Screen-space black hole lens haze parameters (set in C++ when BH is on screen)
uniform vec2  uBHScreenPos;
uniform float uBHVisible;
uniform float uBHGaze;

// Noise
float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// Film Mode
// Chromatic aberration, subtle colour grade, vignette and film grain.
vec3 applyFilmMode(vec2 uv, vec3 baseColor)
{
    vec2  centered = uv - vec2(0.5);
    float dist     = length(centered);

    // Chromatic aberration – R and B channels displaced radially
    vec2 caOffset = centered * dist * 0.008;
    float r = texture(sceneTex, uv + caOffset).r;
    float g = baseColor.g;
    float b = texture(sceneTex, uv - caOffset).b;
    vec3 col = mix(baseColor, vec3(r, g, b), 0.10);

    // Slight gamma lift (camera film response)
    col = pow(col, vec3(0.96));

    // Warm-cool colour grade based on luminance
    float luminance = dot(col, vec3(0.2126, 0.7152, 0.0722));
    vec3 warm = vec3(1.03, 1.00, 0.95);
    vec3 cool = vec3(0.95, 0.99, 1.04);
    col *= mix(cool, warm, smoothstep(0.18, 0.85, luminance));

    // Vignette – darkens corners
    float vignette = 1.0 - smoothstep(0.32, 0.86, dist);
    col *= mix(0.88, 1.0, vignette);

    // Film grain – temporally animated random noise
    float grain = hash(uv * vec2(1920.0, 1080.0) + uTime * 30.0) - 0.5;
    col += grain * 0.015;

    return clamp(col, 0.0, 1.0);
}

// Black Hole Screen Haze
// Adds a subtle lens-flare glow around the BH screen position when visible.
vec3 applyBlackHoleLensHaze(vec2 uv, vec3 color)
{
    if (uBHVisible < 0.5)
        return color;

    float gaze = clamp(uBHGaze, 0.0, 1.0);
    float d    = length(uv - uBHScreenPos);

    float radialGlow    = exp(-d * 10.0) * 0.06 * gaze;
    float outerGlow     = exp(-d * 4.0)  * 0.03 * gaze;

    // Horizontal streak (anamorphic-style lens effect)
    float streakY = abs(uv.y - uBHScreenPos.y);
    float streakX = abs(uv.x - uBHScreenPos.x);
    float horizontalStreak = exp(-streakY * 180.0) * exp(-streakX * 4.0) * 0.035 * gaze;

    vec3 flareCol = vec3(1.0, 0.84, 0.60);
    vec3 haloCol  = vec3(0.65, 0.75, 1.0);

    color += haloCol  * (radialGlow + outerGlow);
    color += flareCol * horizontalStreak;

    return color;
}

void main()
{
    // 1. Sample HDR scene
    vec3 hdrCol = texture(sceneTex, TexCoords).rgb;

    // 2. Add bloom (bright-pass Gaussian blur composite)
    if (bloomEnabled == 1)
        hdrCol += texture(bloomBlurTex, TexCoords).rgb;

    // 3. Tone mapping
    vec3 mapped;
    if (hdrEnabled == 1)
        // Exponential tone mapping – preserves highlights better than Reinhard
        mapped = vec3(1.0) - exp(-hdrCol * exposure);
    else
        // Reinhard tone mapping fallback
        mapped = hdrCol / (hdrCol + vec3(1.0));

    // 4. Gamma correction – convert from linear to sRGB (gamma = 2.2)
    mapped = pow(clamp(mapped, 0.0, 1.0), vec3(1.0 / 2.2));

    // 5. Screen-space lens haze around the black hole
    mapped = applyBlackHoleLensHaze(TexCoords, mapped);

    // 6. Optional film post-processing
    if (filmMode == 1)
        mapped = applyFilmMode(TexCoords, mapped);

    FragColor = vec4(mapped, 1.0);
}
