# TON 618 (Black Hole Mission) — COMP3015 CW1

**Student:** Harrison Scott  
**Student ID:** 10805603
**Module:** COMP3015 – Games Graphics Pipelines  
**Module Leader:** Dr Ji-Jian Chin  
**Repository:** https://github.com/Harrisonscott9970/OpenGl-Blackhole.git
**Video Demo:** [ADD YOUR UNLISTED YOUTUBE VIDEO LINK]

---

## Project Overview

TON 618 (Black Hole Mission) is an OpenGL / GLSL real-time graphics prototype created for COMP3015 CW1.

The project presents a stylised sci-fi black hole scene in which the player explores an orbital environment, collects energy cells from floating platforms, and returns safely to the home platform before being consumed by the black hole, TON 618.

The main goal of the project was to combine a **custom shader-driven black hole effect** with a **textured Blinn-Phong lighting pipeline**, framebuffer post-processing, and a simple gameplay loop so that the scene is not only visually interesting but also interactive and purposeful.

The project was developed from the **COMP3015 Lab 1 template**, then expanded with original scene logic, shader work, post-processing, procedural effects, and gameplay systems.

<img width="799" height="602" alt="image" src="https://github.com/user-attachments/assets/b5302641-6f37-452b-873a-9d596373df13" />
<img width="801" height="632" alt="image" src="https://github.com/user-attachments/assets/255d8e35-73ab-4d22-8a50-e1cba532028e" />

---

## Build Information

| Item | Detail |
|---|---|
| IDE | Visual Studio 2022 |
| OpenGL Version | 4.3 Core Profile |
| Template Base | COMP3015 Lab 1 template |
| Libraries | GLAD, GLFW, GLM, Assimp, stb_image |

---

## Submission / Opening Instructions

Open **`Project_Template.sln`** in **Visual Studio**.

The final submission zip includes:

- the full Visual Studio project
- the executable version
- all required assets and shader files
- this README
- the signed Generative AI declaration

---

## Rubric Coverage Summary

This project was designed to address the CW1 rubric areas as follows:

- **Custom shading model:** Blinn-Phong fragment shading for the platform/pickup scene elements
- **Textured technique:** diffuse texture sampling and normal mapping
- **Lighting setup:** multiple lights and distance-based atmospheric/fog styling
- **Skybox:** cubemap environment used as both scene background and black hole distortion source
- **Image processing:** HDR framebuffer, bloom, Gaussian blur, tone mapping, gamma correction
- **Animation:** animated pickups, orbiting light behaviour, moving asteroid/debris elements, animated accretion disk
- **Keyboard / mouse control:** player free-look and movement controls plus rendering toggles
- **Gamification:** objective, fail state, win state, feedback, and reset loop

---

## Controls

| Key | Action |
|---|---|
| W / A / S / D | Move camera |
| Mouse | Look around |
| Left Mouse | Collect nearby energy cell |
| H | Toggle HDR |
| B | Toggle Bloom |
| F | Toggle Film Mode |
| Q / E | Decrease / increase exposure |
| G | Reset mission |
| Escape | Quit |

---

## Objective

The player must:

1. Fly between the floating platforms
2. Collect all energy cells
3. Return to the home platform

The black hole applies increasing danger and pull as the player gets closer. If the player crosses the event horizon, the mission fails and resets. If all cells are collected and the player returns to base, the mission is completed and resets into a new layout.

This was done to ensure the scene had clear mechanics, challenge, failure, and completion rather than functioning only as a visual showcase.

---

## How It Works

The application is built around a single main scene class which handles initialisation, update logic, rendering, post-processing, and mission/game-state behaviour. The code is structured so that the graphics pipeline and scene logic are easy to follow.

### Main files

| File | Purpose |
|---|---|
| `main.cpp` | Application entry point |
| `scenebasic_uniform.h/.cpp` | Main scene class: initialisation, update, rendering, post-processing, and mission logic |
| `Camera.h/.cpp` | FPS-style camera controls and mouse look |
| `ModelLoader.h` | Assimp-based model loading |
| `shader/blackhole.vert/.frag` | Black hole sphere and gravitational lensing effect |
| `shader/disk.vert/.frag` | Procedural accretion disk rendering |
| `shader/platform.vert/.frag` | Blinn-Phong lighting for platforms, pickups, and asteroids |
| `shader/skybox.vert/.frag` | Cubemap skybox |
| `shader/hdr.vert/.frag` | HDR tone mapping, bloom combine, film mode |
| `shader/blur.frag` | Gaussian blur for bloom |
| `media/` | Models, cubemap textures, diffuse textures and related assets |
| `helper/` | Template/helper OpenGL support code |

### Code flow

- `main.cpp` creates the application and loads the scene
- `SceneBasic_Uniform::initScene()` compiles shaders, builds meshes, loads textures/models, sets OpenGL state, and resets the mission
- update functions handle:
  - black hole pull and danger
  - pickup animation
  - randomised mission/platform state
  - win/lose/reset flow
- render functions draw:
  - black hole
  - accretion disk
  - platforms
  - pickups
  - asteroids
  - skybox
- the HDR framebuffer is then processed for bloom, tone mapping, gamma correction, and optional film-style post effects

---

## Shader Features

## 1. Black Hole / Gravitational Lensing

The centrepiece of the project is the black hole shader. The goal was to create a visually recognisable black hole rather than just a black sphere.

This shader performs an iterative ray-bending style effect to distort the skybox around the black hole. The visual result:

- background lensing
- a photon-ring-like bright edge
- secondary ring detail
- colour asymmetry inspired by relativistic motion

This part of the project was **influenced by research into black hole visualisation**, but adapted into my own coursework scene rather than copied directly. The research-inspired element is the lensing / ray-bending visual concept. The surrounding scene integration, gameplay structure, post-processing pipeline, accretion disk design, and overall rendering composition are my own implementation choices for this project.

**Research inspiration:**  
Müller, T. & Grave, F. (2010). *Catalogue of Spacetimes*. arXiv:0904.4184.

This feature is also cited in the shader code and is discussed in the video report.  

---

## 2. Procedural Accretion Disk

The accretion disk is rendered separately from the black hole sphere and is designed to feel fast, unstable, and visually distinct from a simple textured ring.

Techniques used:

- procedural turbulence / fBm-style distortion
- spiral UV motion
- brightness and colour asymmetry
- lighting response from the orbiting light
- two-part rendering around the black hole to improve visual layering

This gives the scene more motion and visual identity than a static ring mesh and helps the black hole read as more cinematic.

---

## 3. Blinn-Phong Multi-Light Shading

The platform shader uses **Blinn-Phong** shading in the fragment shader.

The shader includes:

- diffuse lighting
- specular highlights
- multiple point lights
- normal mapping via TBN space
- distance fog / atmospheric falloff
- texture sampling
- pixel discard effects for animated cell visuals

This combination was chosen to make the platforms, pickups, and related scene props feel more sci-fi and connected in the same design as the black hole effect.

---

## 4. HDR, Bloom, and Post-Processing

The project renders to a floating-point framebuffer and applies post-processing after the main scene render.

Implemented post-processing includes:

- HDR framebuffer rendering
- bright-pass extraction
- Gaussian blur
- bloom composition
- exposure-based tone mapping
- gamma correction
- optional film-style grading and screen effects

This helps the black hole, energy cells, and bright highlights feel more cinematic and improves the overall scene presentation.

---

## 5. Skybox

A cubemap skybox is used to provide a space environment for both the background and the black hole lensing effect.

The skybox is not just decorative: it is essential to the black hole shader because the distorted environment is sampled from it. This makes it part of the rendering technique rather than only a background element.

---

## What Makes This Shader Program Special

The project tries to go beyond a standard lit textured model by combining several ideas into one scene:

- a research-inspired black hole visual effect
- a procedural accretion disk
- textured Blinn-Phong multi-light shading
- normal mapping
- HDR / bloom post-processing
- a small gameplay loop

The goal was to make the black hole the visual focus, while the platforms, pickups, and mission structure gave the player a reason to move around and observe the rendering from different angles.

---

## Gamification

| Element | Implementation |
|---|---|
| Objective | Collect all energy cells and return to base |
| Challenge | Increasing black hole pull and danger near the centre |
| Support mechanic | Nearby platforms reduce the pull effect |
| Feedback | Window title / mission status updates |
| Win state | Collect all cells and return to home platform |
| Lose state | Cross the event horizon |
| Reset loop | Automatic restart after win/lose |
| Pickup feedback | Spin, shrink, burst, camera shake |

This means the project has goals, challenge, failure, completion, and feedback, which helps it read as a game-like graphics prototype rather than only a technical demo.

<img width="786" height="533" alt="image" src="https://github.com/user-attachments/assets/f2275410-c658-47db-ad56-cf54d13569f0" />

---

## Software Engineering Notes

A few practical trade-offs came up during development:

- the scene uses several separate shaders instead of forcing everything into one program, which improves clarity and debugging
- bloom and HDR improve image quality but add extra render passes and framebuffer cost
- higher mesh density on the black hole sphere improves appearance but increases geometry cost

I tried to keep things separated so that rendering logic, camera logic, shader stages, and mission logic were easier to follow and debug.

---

## Debugging, Iteration, and Development Process

During development the project went through several rounds of testing, debugging, and visual iteration.

Examples of work carried out during development included:

- fixing shader compilation issues and correcting rendering bugs
- tuning black hole distortion so the effect remained readable from different camera angles
- adjusting asteroid behaviour and scene motion to better support the visual theme
- refining bloom, exposure, and post-processing settings so the scene looked good without becoming too much
- resolving project setup and dependency issues so the coursework project could build and run correctly from the intended folder structure
- improving mission/game-state behaviour such as win/lose/reset flow and pickup feedback

---

## Evaluation

I believe the project successfully achieves the brief’s core goal of presenting a **custom GLSL shading prototype with textured rendering and intentional scene design**. It is stronger than a minimal pass because it combines multiple graphics techniques into one coherent theme and adds interaction/gameplay on top.

The strongest parts of the submission are:

- the black hole focal effect
- the combination of shader techniques
- the use of post-processing
- the interactive mission structure
- the overall visual identity

If I had more time, I would improve:

- build portability and packaging even further
- a more explicit on-screen HUD instead of relying mainly on window title feedback
- further optimisation and cleaner asset handling
- even stronger documentation of the research-paper connection

---
## References

### Research / Technical References

- Müller, T. & Grave, F. (2010). *Catalogue of Spacetimes*. arXiv:0904.4184.  
  Used as research inspiration for the black hole lensing / ray-bending visual approach implemented in `blackhole.frag`.

- Blinn, J. F. (1977). *Models of light reflection for computer synthesized pictures*.  
  Referenced for the Blinn-Phong shading model used in the platform/pickup lighting shader.

- LearnOpenGL.  
  Used as a general reference for OpenGL techniques, framebuffer workflows, cubemaps, camera handling, and rendering structure.  
  [ADD SPECIFIC PAGE LINKS IF YOU USED ANY]

- Assimp Documentation.  
  Used as reference for model loading and imported mesh handling.

- stb_image Documentation.  
  Used as reference for image/texture loading.

---

### Asset Credits

#### Skybox / Space Cubemap
The project uses a six-face space skybox/cubemap with files such as:

- `jettelly_space_common_blue_RIGHT.png`
- `jettelly_space_common_blue_LEFT.png`
- `jettelly_space_common_blue_UP.png`
- `jettelly_space_common_blue_DOWN.png`
- `jettelly_space_common_blue_FRONT.png`
- `jettelly_space_common_blue_BACK.png`

[ADD THE ORIGINAL SOURCE / AUTHOR / WEBSITE / LICENSE FOR THIS SKYBOX PACK HERE]

If the alternate renamed faces are also included in the project, these correspond to:
- `right.png`
- `left.png`
- `top.png`
- `bottom.png`
- `front.png`
- `back.png`

---

#### Platforms
The project uses metal textures including:

- `blue_metal_plate_diff_4k.jpg`
- `blue_metal_plate_disp_4k.png`

These were used for platform/environment surface materials.

[ADD THE ORIGINAL SOURCE / AUTHOR / WEBSITE / LICENSE FOR THESE TEXTURES HERE]

---

#### 3D Model
The project uses the following imported model asset:

- `cell.obj`
- `cell.mtl`

This model was created by me.

---

## AI Usage Declaration

Generative AI tools were used in an allowed way for this assessment, consistent with the module guidance for partnered use.

AI-supported assistance included:

- debugging support
- explanation of OpenGL / shader issues
- project setup troubleshooting
- documentation drafting and refinement
- code-structure suggestions during iteration

The final implementation was reviewed, edited, integrated, tested, and understood by me before submission.


<img width="794" height="546" alt="image" src="https://github.com/user-attachments/assets/10752ab8-1b9c-4bf7-896b-8054cdb71a56" />
