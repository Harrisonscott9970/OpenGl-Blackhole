# Black Hole Mission — COMP3015 CW1

> OpenGL / GLSL real-time black hole scene with gravitational lensing,
> procedural accretion disk, Blinn-Phong multi-light shading, normal mapping,
> exponential fog, pixel discard, bloom, HDR tone mapping and gamification.

## Video Report
[Watch on YouTube — ADD YOUR LINK HERE]

## Build Info
| Item | Detail |
|---|---|
| IDE | Visual Studio 2022 |
| OS | Windows 11 |
| OpenGL | 4.3 Core Profile |
| Libraries | GLAD, GLFW, GLM, Assimp, stb_image |
| Template | COMP3015 Lab 1 template |

Open the .sln in Visual Studio, set startup project to Shader_Basics, press F5.
The exe expects shader/ and media/ folders in the working directory.

## Controls
| Key | Action |
|---|---|
| W A S D | Move camera |
| Mouse | Look |
| Left Mouse | Collect energy cell (range 8 units) |
| H | Toggle HDR |
| B | Toggle Bloom |
| F | Toggle Film Mode |
| Q / E | Exposure down / up |
| G | Reset mission |
| Escape | Quit |

## Shader Techniques

### 1. Gravitational Lensing — blackhole.frag
**Research paper:**
> Muller, T. & Grave, F. (2010). Catalogue of Spacetimes. arXiv:0904.4184

Iterative ray-bending in the fragment shader. At each step the photon path
is deflected by:

    delta_dir = perp(gravity, dir) * (Rs / r^2)

Physically motivated by the GR result that total deflection a ~ 2Rs/b.
Produces: skybox lensing warp, photon ring, secondary ring, Doppler colour
shift (blue-shift approaching, red-shift receding), total capture inside
the event horizon.

### 2. Accretion Disk — disk.frag
- Fractional Brownian Motion (fbm) 5-octave turbulence in spiral UV space
- Doppler asymmetry (approaching side brighter/hotter)
- Blinn-Phong specular from orbiting light
- Two-pass rendering (back half / front half around the sphere)

### 3. Blinn-Phong Multi-Light — platform.frag
- Two point lights: primary orbiting + secondary per-object coloured
- Normal mapping via TBN matrix (Gram-Schmidt re-orthogonalised)
- Diffuse texture (4K metal plate on satellite platforms)
- Exponential squared fog: f = exp(-(density * dist)^2)
- Pixel discard: animated sparkle dissolve on energy cells

### 4. Bloom + HDR — hdr.frag / blur.frag
- RGBA16F floating-point HDR framebuffer
- Bright-pass extraction (luminance > 1.0 threshold)
- Separable Gaussian blur, 10 ping-pong passes, 5-tap kernel
- Exponential tone mapping: 1 - exp(-color * exposure)
- Gamma correction: pow(color, 1/2.2)
- Film Mode: chromatic aberration, colour grade, vignette, grain

### 5. Skybox — skybox.frag
Cube-map. Translation stripped from view matrix. gl_Position = pos.xyww
forces depth = 1.0 so skybox always renders behind all geometry.

## Code Structure
```
main.cpp                    Entry point
scenebasic_uniform.h/.cpp   Scene init, update, render
Camera.h/.cpp               FPS fly camera
ModelLoader.h               Assimp loader, merges submeshes into one VAO
shader/
  blackhole.vert/.frag      BH sphere lensing (research paper)
  disk.vert/.frag           Procedural accretion disk
  platform.vert/.frag       Blinn-Phong platforms/cells/asteroids
  skybox.vert/.frag         Cube-map skybox
  hdr.vert/.frag            HDR + bloom + film mode
  blur.frag                 Gaussian blur bright-pass
media/
  skybox/                   6 PNG cube-map faces
  blue_metal_plate_diff_4k.jpg  Platform diffuse texture
  cell.obj                  Energy cell model
```

## Gamification
| Element | Detail |
|---|---|
| Objective | Collect all cells, return to home platform |
| Gravity | BH pulls camera inward, intensifies near horizon |
| Platform shield | Near a platform reduces pull strength |
| Danger bar | HUD: green to red proximity indicator |
| Win | All cells + return within 4.5 units of home |
| Lose | Enter event horizon |
| Auto-reset | Fade out, new random layout, fade in |
| Pickup FX | Spin-shrink, ring burst, camera shake |

## References
- Muller & Grave (2010). Catalogue of Spacetimes. arXiv:0904.4184
- LearnOpenGL: https://learnopengl.com
- Quilez, I. Noise/fbm: https://iquilezles.org
- Blinn (1977). Models of light reflection for computer synthesized pictures. SIGGRAPH.
- stb_image: https://github.com/nothings/stb
- Assimp: https://assimp.org

## AI Usage Declaration
