#version 430 core
// blackhole.frag
// Gravitational lensing shader — simulates light bending around a Schwarzschild
// (non-rotating) black hole using iterative ray deflection.
//
// Based on the optical path deflection model described in:
//   Muller, T. & Grave, F. (2010). "Catalogue of Spacetimes"
//   arXiv:0904.4184  https://arxiv.org/abs/0904.4184
//
// The ray-bending approximation uses discrete integration of the geodesic
// deflection angle. At each step the ray direction is perturbed toward the
// black hole centre by a force proportional to Rs/r^2:
//
//   delta_dir = perp(gravity, dir) * (Rs / r^2)
//
// This is physically motivated: in GR, photon deflection angle a ~ 2Rs/b
// where b is the impact parameter. Iterating with small steps reproduces
// the correct qualitative behaviour including photon-ring brightening and
// total capture inside the event horizon.

out vec4 FragColor;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform float       uTime;
uniform samplerCube uSkybox;
uniform vec3        uBHPos;
uniform vec3        uCamPos;

// ?? Noise ????????????????????????????????????????????????????????????????????
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

// ?? Iterative Ray Bending ?????????????????????????????????????????????????????
// Simulates the curved path of a photon through the Schwarzschild metric.
// ro  = ray origin (fragment world pos, on the BH sphere surface)
// rd  = initial ray direction (view vector)
// Rs  = effective Schwarzschild radius
//
// At each iteration:
//   1. Compute perpendicular component of gravity relative to ray direction
//   2. Deflect ray toward BH centre proportional to Rs/dist^2
//   3. Step forward along the new direction
//   4. Terminate if inside event horizon (black) or escaped to far field
vec3 traceRay(vec3 ro, vec3 rd, float Rs)
{
    vec3 pos = ro;
    vec3 dir = normalize(rd);

    for (int i = 0; i < 160; i++)
    {
        vec3  toC  = uBHPos - pos;
        float dist = length(toC);

        // Inside event horizon — all light captured, return pure black
        if (dist < Rs * 0.48)
            return vec3(0.0);

        vec3 n = normalize(toC);

        // Gravitational deflection magnitude proportional to Rs / r^2
        float pull = 1.9 * Rs / max(dist * dist, 0.02);
        pull = clamp(pull, 0.0, 0.95);

        // Perturb ray: project gravity onto plane perpendicular to current dir
        vec3 bend = n - dir * dot(n, dir);
        dir = normalize(dir + bend * pull);

        // Adaptive step: smaller near BH for accuracy in high-curvature region
        float stepSize = (dist < Rs * 3.2) ? 0.45 : 1.35;
        pos += dir * stepSize;

        // Ray escaped — sample skybox with final bent direction
        if (length(pos - uBHPos) > 700.0)
            break;
    }

    return texture(uSkybox, dir).rgb;
}

void main()
{
    vec3  N       = normalize(vNormal);
    vec3  viewDir = normalize(vWorldPos - uCamPos);

    // Edge factor: 0 = surface faces camera, 1 = grazing angle
    float edge        = 1.0 - abs(dot(N, -viewDir));
    float horizonMask = smoothstep(0.68, 0.97, edge);

    // ?? Gravitational lensing ?????????????????????????????????????????????????
    vec3 bentSky = traceRay(vWorldPos + N * 1.1, viewDir, 18.0);

    // Brighten the lensing band just before the photon ring (Einstein ring region)
    float lensBand = smoothstep(0.55, 0.78, edge) * (1.0 - smoothstep(0.82, 0.98, edge));
    bentSky *= 1.0 + lensBand * 9.0;
    bentSky *= 1.0 + horizonMask * 5.5;

    // ?? Photon ring (primary + secondary) ????????????????????????????????????
    // The photon sphere exists at r = 1.5 Rs. Light orbiting here produces a
    // bright ring at the apparent edge of the black hole shadow.
    float photonRing    = pow(clamp((edge - 0.88) / 0.12, 0.0, 1.0), 10.0);
    float secondaryRing = pow(clamp((edge - 0.93) / 0.07, 0.0, 1.0), 5.0);

    // ?? Relativistic Doppler colour shift ?????????????????????????????????????
    // Approaching plasma is blue-shifted, receding is red-shifted.
    // Simulated with sinusoidal modulation keyed to azimuthal angle + time.
    float ang     = atan(N.z, N.x);
    float doppler = sin(ang + uTime * 0.18) * 0.5 + 0.5;

    vec3 warmTint = vec3(1.15, 0.55, 0.16);  // red-shifted (receding)
    vec3 coolTint = vec3(0.75, 0.95, 1.25);  // blue-shifted (approaching)
    vec3 ringTint = mix(warmTint, coolTint, doppler);

    vec3 ring  = ringTint * photonRing * 16.0;
    ring      += vec3(1.0, 0.85, 0.55) * secondaryRing * 5.0;

    // ?? Disk-light equatorial wrapping ????????????????????????????????????????
    // Warm orange from the accretion disk illuminating the lower hemisphere
    float equator  = pow(1.0 - abs(dot(N, vec3(0.0, 1.0, 0.0))), 4.0);
    float wrapMask = smoothstep(0.50, 0.90, edge) * equator;
    vec3  wrapped  = texture(uSkybox,
        reflect(normalize(vec3(viewDir.x, -abs(viewDir.y) - 0.25, viewDir.z)), N)).rgb;
    wrapped *= vec3(1.2, 0.8, 0.35) * wrapMask * 1.3;

    // ?? Inner heat bleed ??????????????????????????????????????????????????????
    float innerNoise  = noise(N.xz * 6.5 + uTime * 0.04);
    float centerBleed = (1.0 - smoothstep(0.0, 0.50, edge)) * innerNoise * 0.01;
    vec3  coreGlow    = vec3(1.0, 0.38, 0.08) * centerBleed;

    // ?? Composite ?????????????????????????????????????????????????????????????
    float coreFade = 1.0 - smoothstep(0.0, 0.64, edge);
    vec3  blackCore = vec3(0.0) * coreFade;

    vec3 finalCol = mix(blackCore, bentSky, horizonMask) + ring + wrapped + coreGlow;

    FragColor = vec4(finalCol, 1.0);
}