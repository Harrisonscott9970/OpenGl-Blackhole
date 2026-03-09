# Black Hole Mission — COMP3015 CW1

**Student:** Harrison Scott  
**Student ID:** [YOUR STUDENT ID]  
**Module:** COMP3015 – Games Graphics Pipelines  
**Module Leader:** Dr Ji-Jian Chin  
**Repository:** [YOUR PUBLIC GITHUB REPO LINK]  
**Video Demo:** [YOUR UNLISTED YOUTUBE VIDEO LINK]

---

## Project Overview

Black Hole Mission is an OpenGL / GLSL real-time graphics prototype created for **COMP3015 CW1: Initial Prototype**.

The project presents a stylised sci-fi black hole scene in which the player explores a hazardous orbital environment, collects energy cells from satellite platforms, and returns safely to the home platform before being consumed by the black hole.

The main technical goal of the project was to combine a **custom shader-driven black hole effect** with a **textured Blinn-Phong lighting pipeline**, post-processing, and a small amount of **gamification** so that the scene is not only visually interesting but also interactive and purposeful.

The project was developed from the **COMP3015 Lab 1 template**, then extended with original scene logic, shader work, post-processing, procedural effects, and gameplay systems. 

---

## Build Information

| Item | Detail |
|---|---|
| IDE | Visual Studio 2022 |
| Operating System | Windows 11 |
| OpenGL Version | 4.3 Core Profile |
| Template Base | COMP3015 Lab 1 template |
| Libraries | GLAD, GLFW, GLM, Assimp, stb_image |

### Opening the project
Open the solution file in Visual Studio and set the startup project to the coursework executable target.

### Running the project
Run the project from Visual Studio in the normal way. The executable expects the `shader/` and `media/` folders to be available in the working directory.

### Submission note
The final submission zip should include:

- the full Visual Studio project
- the executable version
- all required assets and shader files
- this README
- the Generative AI declaration appendix
- the AI prompt/transcript appendix

The final zip should also remove unnecessary build folders such as `Debug`, `ipch`, and `.sdf` files to remain within the submission size guidance. 

---

## How the User Interacts With the Prototype

### Controls

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

### Objective

The player must:

1. move between the floating platforms
2. collect all energy cells
3. return to the home platform

The black hole applies increasing danger and pull as the player gets closer. If the player crosses the event horizon, the mission fails and resets. If all cells are collected and the player returns to base, the mission is completed and resets into a new layout. This was added to satisfy the scene gamification goal rather than producing a purely static showcase. 

---

## How It Works

The application is built around a single main scene class which handles initialisation, update logic, rendering, post-processing, and mission/game-state behaviour. The code is structured so that the graphics pipeline and scene logic are easy to follow.

### Main files

| File | Purpose |
|---|---|
| `main.cpp` | Application entry point |
| `scenebasic_uniform.h/.cpp` | Main scene class: initialisation, update, render, mission logic |
| `Camera.h/.cpp` | FPS-style camera controls and mouse look |
| `ModelLoader.h` | Assimp-based model loading |
| `shader/blackhole.vert/.frag` | Black hole sphere and gravitational lensing effect |
| `shader/disk.vert/.frag` | Procedural accretion disk rendering |
| `shader/platform.vert/.frag` | Blinn-Phong lighting for platforms, pickups, and asteroids |
| `shader/skybox.vert/.frag` | Cubemap skybox |
| `shader/hdr.vert/.frag` | HDR tone mapping, bloom combine, film mode |
| `shader/blur.frag` | Gaussian blur for bloom |
| `media/` | Models, cubemap textures, diffuse textures |

### Code flow

At a high level:

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
- the HDR framebuffer is then processed for bloom, tone mapping, gamma correction, and optional film-style post effects. 

---

## Shader Features

## 1. Black Hole / Gravitational Lensing

The centrepiece of the project is the black hole shader. The goal was to create a visually recognisable black hole rather than just a black sphere.

This shader performs an iterative ray-bending style effect to distort the sampled skybox around the black hole. The visual result includes:

- background lensing
- a photon-ring-like bright edge
- secondary ring detail
- capture inside the event horizon
- colour asymmetry inspired by relativistic motion

This part of the project was influenced by research into black hole visualisation. The implementation was adapted into the coursework scene rather than copied directly, and integrated into my own rendering pipeline and scene design.

**Research inspiration:**  
Müller, T. & Grave, F. (2010). *Catalogue of Spacetimes*. arXiv:0904.4184.

This feature should also be cited inline in the shader code and explained in the video, in line with the module guidance on outside algorithms and research-derived features. 

---

## 2. Procedural Accretion Disk

The accretion disk is rendered separately from the black hole sphere and is designed to feel energetic and unstable.

Techniques used include:

- procedural turbulence / fBm-style distortion
- spiral UV motion
- brightness and colour asymmetry
- lighting response from the orbiting light
- two-part rendering around the black hole to improve the visual layering

This gives the scene more motion and visual identity than a simple textured ring.

---

## 3. Blinn-Phong Multi-Light Shading

The platform shader uses **Blinn-Phong** shading in the fragment shader, which satisfies the CW1 custom shading requirement while allowing a more polished specular response than a purely diffuse model. :contentReference[oaicite:12]{index=12}

The shader includes:

- diffuse lighting
- specular highlights
- multiple point lights
- normal mapping via TBN space
- distance fog
- texture sampling
- pixel discard effects for animated cell visuals

This combination was chosen to make the platforms and pickups feel more grounded within the same scene as the black hole effect.

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

This helps the black hole, energy cells, and bright highlights feel more cinematic and improves the overall aesthetics of the scene. :contentReference[oaicite:13]{index=13}

---

## 5. Skybox

A cubemap skybox is used to provide a space environment for both the background and the black hole lensing effect.

The skybox is not just decorative: it is essential to the black hole shader because the distorted environment is sampled from it. This makes it part of the visual technique rather than a separate background-only feature. :contentReference[oaicite:14]{index=14}

---

## What Makes This Shader Program Special

The project tries to go beyond a standard “lit textured model” by combining several ideas into one scene:

- a research-inspired black hole visual effect
- a procedural accretion disk
- textured Blinn-Phong multi-light shading
- normal mapping
- HDR / bloom post-processing
- a small gameplay loop

The main idea came from wanting to build a scene that looked more like a cinematic space anomaly than a basic graphics lab exercise. I started from the provided COMP3015 template and then designed an original black-hole-centred scene around it. The goal was to make the black hole the visual focus, while the platforms, pickups, and mission structure gave the player a reason to move around and observe the rendering from different angles. 

---

## Gamification

To strengthen the scene and meet the gamification side of the rubric, I added game-like mechanics rather than leaving the project as a static rendering demo.

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

This means the prototype has goals, challenge, failure, completion, and feedback, which helps it read as a game-like graphics prototype instead of only a technical demo. 

---

## Software Engineering Notes

A few practical trade-offs came up during development:

- The scene uses several separate shaders instead of forcing everything into one program, which improves clarity and debugging.
- Bloom and HDR improve image quality but add extra render passes and framebuffer cost.
- Higher mesh density on the black hole sphere improves appearance but increases geometry cost.
- The project favours visual readability and demonstration value over deep optimisation because this is a coursework prototype rather than a shipping game.

I tried to keep the code separated by responsibility so that rendering logic, camera logic, shader stages, and mission logic were easier to follow.

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

## Originality / Starting Point

This project was built from the **COMP3015 Lab 1 template**, as required by the brief. I then added my own:

- black hole rendering concept
- accretion disk implementation
- shader combinations and scene integration
- mission/gameplay loop
- post-processing setup
- platform / pickup / asteroid scene logic

Where external ideas, papers, or standard resources influenced the work, they are acknowledged below and should also be referenced in relevant code comments. 

---

## References

- Müller, T. & Grave, F. (2010). *Catalogue of Spacetimes*. arXiv:0904.4184.
- LearnOpenGL – general OpenGL techniques and reference material.
- Blinn, J. F. (1977). *Models of light reflection for computer synthesized pictures*.
- Assimp documentation.
- stb_image documentation.
- Any additional sources used for textures, models, or shader inspiration should be listed here.

---

## AI Usage Declaration

Generative AI tools were used in an allowed way for this assessment, consistent with the module guidance for partnered use, including assistance with code support, testing support, and README drafting. The final implementation was reviewed, edited, integrated, and understood by me before submission. 

A signed **Generative AI Declaration** is included in the submission appendix, as required by the assignment brief. 

---

## AI Prompts / Transcript Appendix

As required by the brief, all implemented features should be accompanied by the AI prompts/transcript used during development. These are included in the submission appendix / supporting documentation. :contentReference[oaicite:20]{index=20}
