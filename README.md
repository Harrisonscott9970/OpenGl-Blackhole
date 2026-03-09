# TON 618 (Black Hole Mission) — COMP3015 CW1

**Student:** Harrison Scott  
**Student ID:** [ADD YOUR STUDENT ID]  
**Module:** COMP3015 – Games Graphics Pipelines  
**Module Leader:** Dr Ji-Jian Chin  
**Repository:** [ADD YOUR PUBLIC GITHUB REPO LINK]  
**Video Demo:** [ADD YOUR UNLISTED YOUTUBE VIDEO LINK]

---

## Project Overview

TON 618 (Black Hole Mission) is an OpenGL / GLSL real-time graphics prototype created for **COMP3015 CW1**.

The project presents a stylised sci-fi black hole scene in which the player explores a hazardous orbital environment, collects energy cells from floating platforms, and returns safely to the home platform before being consumed by the black hole, TON 618.

The main goal of the project was to combine a **custom shader-driven black hole effect** with a **textured Blinn-Phong lighting pipeline**, framebuffer post-processing, and a simple gameplay loop so that the scene is not only visually interesting but also interactive and purposeful.

The project was developed from the **COMP3015 Lab 1 template**, then expanded with original scene logic, shader work, post-processing, procedural effects, and gameplay systems.

[ADD 2–4 SCREENSHOTS HERE OF: overall scene, black hole close-up, accretion disk/platform view, win/lose or gameplay view]

---

## Build Information

| Item | Detail |
|---|---|
| IDE | Visual Studio 2022 |
| Operating System | [ADD YOUR OPERATING SYSTEM, e.g. Windows 10 Pro 22H2 / Windows 11] |
| OpenGL Version | 4.3 Core Profile |
| Template Base | COMP3015 Lab 1 template |
| Libraries | GLAD, GLFW, GLM, Assimp, stb_image |

---

## Submission / Opening Instructions

Open **`[ADD YOUR SOLUTION FILE NAME, e.g. CompProject.sln]`** in **Visual Studio 2022**.

Set **`[ADD YOUR STARTUP PROJECT NAME, e.g. Project_Template]`** as the startup project.

Run the project from the project root so the `shader/` and `media/` folders are found correctly.

The final submission zip includes:

- the full Visual Studio project
- the executable version
- all required assets and shader files
- this README
- the signed Generative AI declaration appendix
- the AI prompt / transcript appendix

The final submission should not include unnecessary build/cache folders such as `Debug`, `Release`, `x64`, `.vs`, `ipch`, or `.sdf` files.

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

This structure was intentionally chosen so that the project would go beyond a minimal graphics demo and instead function as a coherent, game-like rendering prototype.

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

1. fly between the floating platforms
2. collect all energy cells
3. return to the home platform

The black hole applies increasing danger and pull as the player gets closer. If the player crosses the event horizon, the mission fails and resets. If all cells are collected and the player returns to base, the mission is completed and resets into a new layout.

This was done to ensure the scene had clear mechanics, challenge, failure, completion, and feedback rather than functioning only as a static visual showcase.

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
  - asteroids / debris
  - skybox
- the HDR framebuffer is then processed for bloom, tone mapping, gamma correction, and optional film-style post effects

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

This part of the project was **influenced by research into black hole visualisation**, but adapted into my own coursework scene rather than copied directly. The research-inspired element is the lensing / ray-bending visual concept. The surrounding scene integration, gameplay structure, post-processing pipeline, accretion disk design, and overall rendering composition are my own implementation choices for this project.

**Research inspiration:**  
Müller, T. & Grave, F. (2010). *Catalogue of Spacetimes*. arXiv:0904.4184.

This feature is also cited inline in the shader code and is discussed in the accompanying video report.  
[ONLY KEEP THE SENTENCE ABOVE IF YOU HAVE ACTUALLY ADDED THE INLINE COMMENT(S) TO THE SHADER AND TALK ABOUT IT IN THE VIDEO]

---

## 2. Procedural Accretion Disk

The accretion disk is rendered separately from the black hole sphere and is designed to feel energetic, unstable, and visually distinct from a simple textured ring.

Techniques used include:

- procedural turbulence / fBm-style distortion
- spiral UV motion
- brightness and colour asymmetry
- lighting response from the orbiting light
- two-part rendering around the black hole to improve visual layering

This gives the scene more motion and visual identity than a static ring mesh and helps the black hole read as a more cinematic focal point.

---

## 3. Blinn-Phong Multi-Light Shading

The platform shader uses **Blinn-Phong** shading in the fragment shader, which satisfies the CW1 custom shading requirement while allowing a more polished specular response than a purely diffuse model.

The shader includes:

- diffuse lighting
- specular highlights
- multiple point lights
- normal mapping via TBN space
- distance fog / atmospheric falloff
- texture sampling
- pixel discard effects for animated cell visuals

This combination was chosen to make the platforms, pickups, and related scene props feel more grounded in the same world as the black hole effect.

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

This means the prototype has goals, challenge, failure, completion, and feedback, which helps it read as a game-like graphics prototype rather than only a technical demo.

[ADD A SCREENSHOT HERE OF: gameplay objective view OR collected cells / platform route / near-black-hole danger moment]

---

## Software Engineering Notes

A few practical trade-offs came up during development:

- the scene uses several separate shaders instead of forcing everything into one program, which improves clarity and debugging
- bloom and HDR improve image quality but add extra render passes and framebuffer cost
- higher mesh density on the black hole sphere improves appearance but increases geometry cost
- the project favours visual readability and demonstration value over deep optimisation because this is a coursework prototype rather than a shipping game

I tried to keep responsibilities separated so that rendering logic, camera logic, shader stages, and mission logic were easier to follow and debug.

---

## Debugging, Iteration, and Development Process

During development the project went through several rounds of testing, debugging, and visual iteration.

Examples of work carried out during development included:

- fixing shader compilation issues and correcting rendering bugs
- tuning black hole distortion so the effect remained readable from different camera angles
- adjusting asteroid / debris behaviour and scene motion to better support the visual theme
- refining bloom, exposure, and post-processing settings so the scene looked dramatic without becoming unreadable
- resolving project setup and dependency issues so the coursework project could build and run correctly from the intended folder structure
- improving mission/game-state behaviour such as win/lose/reset flow and pickup feedback

Generative AI was used as a **partnered support tool** during parts of this process, including debugging help, explanation of compiler/runtime issues, code-structure suggestions, and assistance with documenting the final project. All final code decisions, integrations, and testing were reviewed and carried out by me.

[ADD 1–2 SHORT EXAMPLES IN BRACKETS HERE IF YOU WANT THIS TO LOOK EVEN STRONGER, e.g.  
- “Example: fixing `glad.c` project path issues when testing the project from a copied GitHub folder”  
- “Example: debugging shader compilation problems and removing unsupported `#include` usage in GLSL”]

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

This project was built from the **COMP3015 Lab 1 template**, as required by the brief.

From that starting point, I added my own:

- black hole rendering concept and integration
- accretion disk implementation
- shader combinations and scene composition
- mission/gameplay loop
- post-processing setup
- platform / pickup / asteroid scene logic
- scene balancing and presentation choices

Where external ideas, papers, or standard resources influenced the work, they are acknowledged below and referenced in relevant code comments where appropriate.

---

## References

- Müller, T. & Grave, F. (2010). *Catalogue of Spacetimes*. arXiv:0904.4184.
- LearnOpenGL – general OpenGL techniques and reference material.
- Blinn, J. F. (1977). *Models of light reflection for computer synthesized pictures*.
- Assimp documentation.
- stb_image documentation.
- [ADD ANY MODEL / TEXTURE / ASSET SOURCES YOU USED]
- [ADD ANY OTHER SHADER / RESEARCH SOURCES YOU REFERENCED]

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

**Appendix A:** Signed Generative AI Declaration  
**Appendix B:** AI Prompt / Transcript Log

[MAKE SURE APPENDIX A AND B ACTUALLY EXIST IN YOUR SUBMISSION]

---

## AI Prompts / Transcript Appendix

As required by the brief, the AI prompt/transcript log is included in the submission appendix / supporting documentation.

To make this clear for the marker, the appendix should show the main types of partnered AI support used during development, for example:

- debugging shader compilation or project setup problems
- improving README/report wording
- discussing rendering ideas and implementation options
- helping reason about portability/build issues
- refining gameplay or post-processing behaviour

[ADD YOUR REAL PROMPT / TRANSCRIPT DOCUMENT TO THE SUBMISSION ZIP]

---

## Final Notes

This project was designed to satisfy the CW1 requirement for a **custom GLSL shading prototype using textured 3D rendering, intentional scene design, interaction, and presentation quality**. The focus was on producing a visually coherent and technically layered scene rather than a minimal single-technique demonstration.

[OPTIONAL: ADD A FINAL SCREENSHOT HERE OF THE BEST-LOOKING CAMERA ANGLE IN THE PROJECT]
