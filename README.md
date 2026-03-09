Black Hole Mission — COMP3015 CW1

An OpenGL / GLSL real-time black hole scene with gravitational lensing, a procedural accretion disk, Blinn-Phong multi-light shading, bloom, HDR tone mapping and gamification.


Video Report
📹 [Watch on YouTube — coming soon]
(Replace this link with your unlisted YouTube URL before submission)

System & Build Info
ItemDetailIDEVisual Studio 2022 (v17.x)OSWindows 11OpenGL4.3 Core ProfileLibrariesGLAD, GLFW, GLM, Assimp, stb_imageTemplateCOMP3015 Lab 1 template (helper/, GLSLProgram)

To build: Open the .sln file in Visual Studio, set the startup project to Shader_Basics, and press F5. All library paths are configured in the project properties. The executable expects the shader/ and media/ folders to be in the working directory (set automatically by VS to the project root).


Controls
Key / InputActionW A S DMove camera (fly-style)MouseLook / aimLeft Mouse ButtonCollect nearby energy cell (within 8 units)HToggle HDR tone mapping on/offBToggle Bloom on/offFToggle Film Mode (chromatic aberration + grain + vignette)Q / EDecrease / increase exposureGReset mission (new platform layout)EscapeExit

Project Overview
What is it?
A real-time 3D black hole simulation with a mini-game layer. The player navigates a spacecraft camera near a supermassive black hole, collects glowing energy cells from satellite platforms, and returns to base — all while the black hole's gravity continuously pulls them inward and an asteroid belt spirals into the event horizon.
Scene Layout

Black hole — centre of the world at (0, 30, -1350). Rendered as a sphere with a gravitational lensing shader.
Accretion disk — procedurally generated ring rendered in two passes (back half behind the sphere, front half in front).
Home platform — at (0, 0, 145), the player's start/return point.
Satellite platforms — 3–5 procedurally placed platforms at randomised positions, each with an energy cell hovering above.
Asteroid belt — 320 asteroids orbiting at radii 560–980 units, spiralling inward over time.
Skybox — deep-space cube map.


Shader Implementation
1. Gravitational Lensing — blackhole.vert / blackhole.frag
The black hole sphere uses an iterative ray-bending technique to simulate gravitational lensing. Starting from each fragment's world-space position, a ray is marched through space and deflected toward the black hole's centre at each step according to:
bend_force ∝ Rs / dist²
This produces the characteristic light-bending and photon-ring effects seen around real black holes. The photon ring is brightened using a Fresnel-like edge term, and a Doppler colour shift (warm/cool) is applied based on the angular position relative to the camera orbit.
Techniques used: iterative ray deflection, value noise, Fresnel edge masking, Doppler colour mixing, cube-map sampling.

2. Accretion Disk — disk.vert / disk.frag
The disk is a procedural mesh of concentric quad rings. The fragment shader builds a turbulent glowing plasma appearance using:

Fractional Brownian Motion (fbm) — 5-octave value noise in a spiral UV space for gas filament structure
Spiral coordinate system — UV parameterised by angle and radius, rotated with time
Doppler asymmetry — one side of the disk appears hotter/brighter to simulate the approaching side moving toward the camera
Blinn-Phong lighting from the orbiting point light source
Two-pass rendering — back half rendered before the sphere, front half after, so the sphere correctly occludes the disk


3. Platform / Asteroid / Energy Cell — platform.vert / platform.frag
A multi-purpose Blinn-Phong shader supporting:

Normal mapping — full TBN matrix (tangent/bitangent/normal) passed from vertex shader; normal map sampled and transformed to world space for per-fragment specular highlights
Diffuse texture — optional texture sampling via uHasDiffuse flag; satellite platforms use a 4K metal plate texture
Two-light Blinn-Phong — primary orbiting light + secondary per-object coloured point light (used for cell glow illuminating its platform)
Energy cell emissive mode (uIsCore = 1) — Fresnel rim glow + sine-wave pulse emission + specular highlight from normal map
Procedural grid lines — UV-space grid pattern etched into untextured surfaces via smoothstep
Asteroid heat tinting — colour and emissive strength lerped based on proximity to the black hole, simulating tidal heating


4. Bloom + HDR Tone Mapping — hdr.vert / hdr.frag / blur.frag
A full post-processing pipeline rendered via framebuffer objects:

HDR Framebuffer — scene rendered into an RGBA16F floating-point texture, allowing colour values above 1.0
Bright-pass extraction — first Gaussian blur pass filters to only pixels with luminance > 1.0
Separable Gaussian blur — 10 ping-pong passes (5 horizontal + 5 vertical) using a 5-tap kernel with weights [0.227, 0.195, 0.122, 0.054, 0.016]
HDR Tone Mapping — Exponential (1 - exp(-color * exposure)) or Reinhard (color / (color + 1))
Gamma Correction — pow(color, 1/2.2) converts linear space to sRGB for display
Film Mode — chromatic aberration, luminance-based colour grade, vignette, temporally animated film grain


5. Skybox — skybox.vert / skybox.frag
Standard cube-map skybox. Translation is stripped from the view matrix so the sky never moves. gl_Position = pos.xyww forces depth to 1.0 so the skybox always renders behind all objects.

Code Architecture
main.cpp                   → SceneRunner entry point
scenebasic_uniform.h/.cpp  → Main scene: init, update, render loop
Camera.h/.cpp              → FPS-style fly camera (WASD + mouse)
ModelLoader.h              → Assimp-based mesh loader (merges all submeshes into one VAO)
shader/
  blackhole.vert/.frag     → Black hole sphere with gravitational lensing
  disk.vert/.frag          → Procedural accretion disk (fbm turbulence)
  platform.vert/.frag      → Blinn-Phong (platforms, cells, asteroids)
  skybox.vert/.frag        → Cube-map skybox
  hdr.vert/.frag           → HDR tone mapping + bloom composite + film mode
  blur.frag                → Separable Gaussian blur (bright-pass bloom)
  blinnphong.vert          → Reference Blinn-Phong vertex shader
media/
  skybox/                  → 6 PNG cube-map faces (right/left/top/bottom/front/back)
  blue_metal_plate_diff_4k.jpg → Satellite platform diffuse texture
  cell.obj                 → Energy cell 3D model (optional; fallback to sphere)
How the render loop works
update(t)
  ├── Camera input (keyboard + mouse)
  ├── Light orbit animation
  ├── Asteroid physics (orbit, gravity pull, respawn)
  ├── Game state (collect cells, danger level, win/lose, fade)
  └── Window title update

render()
  ├── Bind HDR FBO
  ├── Draw skybox (depth trick: xyww)
  ├── Draw satellite platforms (Blinn-Phong + texture + normal map)
  ├── Draw energy cells (emissive Blinn-Phong + pickup animation)
  ├── Draw asteroids (Blinn-Phong + heat tinting)
  ├── Draw accretion disk BACK half (additive blend, depth-write off)
  ├── Draw black hole sphere (lensing shader)
  ├── Draw accretion disk FRONT half
  ├── 10x Gaussian blur ping-pong (bloom)
  ├── HDR tone map + bloom composite → default framebuffer
  └── Scissor-based HUD (danger bar, cell counter, fade overlay)

Gamification
ElementDetailObjectiveCollect all energy cells (3–5) and fly back to the home platformGravityThe black hole continuously pulls the camera inward; pull intensifies near the event horizonPlatform shieldBeing near any platform reduces the effective gravitational pullAsteroids320 asteroids spiral into the black hole; visual hazard and atmosphereDanger barHUD bar shows proximity danger level (green → red)Cell counterHUD boxes show collected / total cellsWin conditionAll cells collected + return within 4.5 units of home platformLose conditionCamera enters the event horizon (distance < lethal radius)Auto-resetFade-out → new random layout → fade-in after win or lossCell pickup animationSpin-shrink → poof ring burst, camera shake on collection

What Makes This Special

Iterative gravitational lensing in GLSL — the black hole bends light rays in the fragment shader itself using a custom ray-marching loop, not a simple reflection. This produces visible photon ring brightening and realistic light wrapping at the event horizon.
Procedural accretion disk with fbm turbulence and Doppler colour shift — no textures needed; entirely computed per-fragment.
Full HDR pipeline with floating-point framebuffer, bloom bright-pass, separable Gaussian blur and gamma-corrected output.
Normal mapping with TBN on the satellite platforms gives convincing surface detail from a flat hexagonal mesh.
Gamified scene with gravity mechanics, win/lose states, randomised layout on reset, and a pixel-art HUD built with OpenGL scissor rects.


AI Usage Declaration
This project used AI assistance (Claude / ChatGPT) for:

Generating and reviewing GLSL shader code
Debugging framebuffer and blending issues
Drafting this README

All AI-generated code was reviewed, understood, modified, and integrated by the student. Source references are noted inline in shader comments.

References

LearnOpenGL — HDR, Bloom, Normal Mapping, Skybox: https://learnopengl.com
Inigo Quilez — Noise and fbm techniques: https://iquilezles.org
Blinn (1977) — "Models of light reflection for computer synthesized pictures"
stb_image: https://github.com/nothings/stb
Assimp: https://assimp.org
