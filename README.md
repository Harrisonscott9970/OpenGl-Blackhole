# COMP3015 – Games Graphics Pipelines  
## Coursework Project – Black Hole Research Platform  
**Student:** [YOUR NAME]  
**Student ID:** [YOUR STUDENT ID]  
**Module:** COMP3015 – Games Graphics Pipelines  
**Module Leader:** Dr Ji-Jian Chin  
**Repository:** [YOUR GITHUB REPO LINK]  
**Video Demo:** [YOUR UNLISTED YOUTUBE LINK]

---

## Project Overview

This project is an OpenGL coursework submission developed from the COMP3015 lab template.  
It presents a stylised black hole scene built around a custom GLSL shader pipeline, HDR rendering, bloom, skybox rendering, and interactive controls.

The scene is designed as a cinematic sci-fi environment where the player explores a layered research platform positioned near a massive black hole. The project combines visual effects, post-processing, custom geometry, and simple gamification into a single graphics prototype.

The visual intention was to create a dramatic and recognisable black hole environment inspired by cinematic science-fiction, while keeping the rendering pipeline understandable and appropriate for real-time execution in OpenGL.

---

## Main Features

### Rendering / Shader Features
- Custom black hole shader
- Custom accretion disk shader
- Skybox / cubemap environment
- HDR rendering with tone mapping
- Bloom post-processing using ping-pong blur
- Optional cinematic “film mode” screen effect
- Animated lighting
- Fragment-shader based lighting for platform and scene objects

### Scene Features
- Custom generated platform mesh
- Multi-layer platform composition
- Custom generated accretion disk mesh
- Black hole placed as a large-scale background focal point
- Collectible glowing energy cores

### Interaction Features
- WASD movement
- Mouse camera look
- Keyboard toggles for HDR / bloom / film mode
- Exposure control
- Reset control
- Basic game loop with collectible objectives and fail/win conditions

---

## How to Run the Project

### Opening the project
Open the Visual Studio solution file included in the submission archive.

**Solution to open:**  
`Project_Template.sln`

### Tested Environment
- **Operating System:** [YOUR WINDOWS VERSION]
- **IDE:** [YOUR VISUAL STUDIO VERSION]

### Running
Build and run the project from Visual Studio.

An executable version is also included in the submission package where required.

---

## Controls

| Key / Input | Action |
|---|---|
| `W` `A` `S` `D` | Move camera |
| `Mouse` | Look around |
| `H` | Toggle HDR |
| `B` | Toggle Bloom |
| `F` | Toggle Film Mode |
| `Q` | Decrease exposure |
| `E` | Increase exposure |
| `G` | Reset game / scene |
| `ESC` | Exit |

---

## Project Concept

The scene is built around a black hole suspended in deep space, viewed from a research platform.  
The idea was to create a visually distinctive graphics prototype that demonstrates shader programming and post-processing rather than a conventional game environment.

The black hole itself is the main focal point, supported by:
- a distorted skybox to suggest gravitational lensing,
- an emissive accretion disk,
- HDR and bloom for glow and brightness response,
- a cinematic film pass to enhance presentation.

The platform acts as the player’s safe space and gameplay area, while the black hole creates danger, atmosphere, and visual identity.

---

## How the Project Works

The render pipeline follows a multi-stage process:

1. **Scene Rendering to HDR Framebuffer**
   - The skybox, platforms, black hole, accretion disk, and collectibles are rendered into a floating-point framebuffer.
   - This allows bright values to be preserved for post-processing.

2. **Bloom Blur Pass**
   - Bright areas are extracted and blurred using a ping-pong blur pass.
   - This creates glow around emissive elements such as the accretion disk and bright highlights.

3. **Final Fullscreen Composite**
   - The HDR scene texture and blurred bloom texture are combined.
   - Tone mapping is applied.
   - Optional film mode is applied as a final fullscreen screen effect.

This structure makes the project easier to extend and better reflects a modern graphics pipeline approach.

---

## Code Structure / Navigation Guide

The project is based on the module template and organised around a main scene class.

### Main Files

#### `main.cpp`
Entry point of the program.  
Initialises the scene and launches the application loop.

#### `scenebasic_uniform.h / scenebasic_uniform.cpp`
This is the main scene implementation and the core of the project.  
It is responsible for:
- compiling and linking shaders,
- creating geometry,
- loading the skybox,
- creating framebuffers,
- updating scene/game state,
- rendering all scene passes.

### Shader Files

#### `blackhole.vert` / `blackhole.frag`
Used to render the black hole sphere.  
This shader is designed to simulate:
- a dark event horizon,
- silhouette-based brightening,
- warped skybox sampling,
- a stylised photon ring effect.

#### `disk.vert` / `disk.frag`
Used to render the accretion disk around the black hole.  
This shader creates:
- layered emissive colour,
- hotter inner ring tones,
- animated banding and variation,
- front/back separated rendering for better compositing.

#### `platform.vert` / `platform.frag`
Used for rendering the platform and collectible objects.  
This includes:
- fragment-based lighting,
- glow/rim enhancement,
- pulsing emissive visuals for collectible cores.

#### `skybox.vert` / `skybox.frag`
Used for rendering the cubemap background.

#### `hdr.vert` / `hdr.frag`
Used for fullscreen post-processing.  
Handles:
- HDR tone mapping,
- bloom compositing,
- exposure control,
- optional film mode.

#### `blur.frag`
Performs Gaussian-style blur in multiple fullscreen passes for bloom.

---

## What Makes the Shader Program Special

The strongest part of the project is the visual focus on the black hole and its surrounding environment.

Rather than using a standard single-object lighting demo, this project attempts to create a more distinctive scene that combines:
- custom procedural-style shader work,
- post-processing,
- stylised space visuals,
- gameplay interaction.

The black hole is not physically exact, but it is designed to evoke recognisable features associated with black holes in cinematic science-fiction:
- a dark central body,
- a bright ring-like edge,
- an accretion disk,
- background distortion.

This makes the project more visually intentional than a generic shader showcase and gives it a stronger portfolio feel.

---

## Shader / Graphics Techniques Used

### 1. Fragment Shader Lighting
The project uses lighting in fragment shaders for scene objects, supporting stronger surface detail than a basic vertex-lit pipeline.

### 2. HDR Rendering
The scene is rendered into a floating-point framebuffer to preserve high brightness values before tone mapping.

### 3. Bloom
A blur-based bloom pipeline is used to glow bright areas in the scene, especially the accretion disk and emissive highlights.

### 4. Skybox
A cubemap skybox is used to create the surrounding space environment and is also sampled by the black hole shader.

### 5. Fullscreen Film Mode
A toggleable cinematic post-process effect is included.  
This applies:
- subtle vignette,
- grain,
- mild colour grading,
- slight edge distortion / chromatic variation.

### 6. Animated Light
The light source moves over time to add life to the scene and improve visual presentation.

### 7. Custom Geometry
The project generates several pieces of geometry procedurally/code-side rather than relying entirely on imported meshes:
- sphere
- accretion disk
- platform mesh

---

## Gameplay / Gamification

The project includes basic gamification elements to move beyond a static scene.

### Game Objective
The player must:
- collect energy cores placed around the platform,
- avoid being drawn too close to the black hole,
- return safely after collecting the objectives.

### Mechanics
- collectible objects
- safe platform region
- black hole attraction outside the platform zone
- fail state if the player gets too close
- reset and replay support

This is intentionally lightweight, but it gives the scene an interactive purpose and helps frame it as more than a passive graphics demo.

---

## Development Process

The project started from the lab template and was gradually extended into a more stylised and feature-rich scene.

The process included:
1. setting up the platform and scene structure,
2. creating a black hole focal object,
3. adding an accretion disk,
4. integrating skybox rendering,
5. adding HDR and bloom,
6. refining the black hole scale and scene composition,
7. adding gameplay features and controls,
8. adding a film-style fullscreen post-process effect.

A lot of the work involved iterative visual tuning rather than purely functional coding.  
For example:
- moving the black hole farther away,
- adjusting platform scale,
- tuning the black hole pull so the starting area remains playable,
- improving the accretion disk so it no longer visually cuts through the black hole.

---

## Performance / Engineering Notes

This project balances visual quality and simplicity.

### Good practice choices
- scene rendering is separated into passes,
- shader roles are clearly separated,
- fullscreen post-processing is modular,
- controls are simple and easy to test.

### Trade-offs
- the black hole effect is visually inspired rather than physically accurate,
- the film mode is intentionally subtle so it does not hide scene detail,
- bloom and fullscreen effects improve presentation but add extra cost,
- some logic is tuned artistically for playability rather than scientific realism.

The goal was to create a convincing real-time scene rather than a simulation.

---

## AI Usage Declaration

This project used generative AI in a **partnered work** manner consistent with the assignment brief.

AI assistance was used for:
- code assistant support,
- debugging and shader iteration,
- programming testing support,
- README/report drafting support.

All AI-assisted work was reviewed, integrated, and adapted into the final submission by the student.

Where outside ideas, techniques, or code structures informed the implementation, they should be cited clearly in code comments, documentation, and/or video discussion as appropriate.

---

## AI Prompt and Transcript Evidence

As required by the module brief, all features should be accompanied by AI prompt and transcript evidence where applicable.

Examples of prompts used during development included:
- improving black hole realism,
- tuning accretion disk visuals,
- implementing HDR and bloom,
- adding exposure controls,
- adjusting platform distance and black hole pull,
- adding an optional film-style fullscreen effect,
- improving gamification and scene composition.

These AI-assisted development notes and transcripts should be included alongside the final submission where required by the brief.

---

## Video Report

Unlisted YouTube video:  
[YOUR UNLISTED YOUTUBE LINK]

The video should cover:
- the project idea,
- unique visual goals,
- shader techniques used,
- code structure overview,
- scene controls,
- HDR/bloom/film mode demonstration,
- gameplay features,
- problems encountered and how they were solved.

---

## Repository / Submission Notes

This repository is intended to reflect the submitted coursework version.

For submission:
- ensure the DLE zip matches this repository,
- remove `Debug`, `ipch`, `.sdf` and unnecessary generated files,
- include all resources required to compile and run,
- include the executable if required,
- include the write-up and video link,
- include the signed Generative AI declaration form as required by the brief.

---

## Evaluation

I believe the project succeeds most strongly in:
- visual identity,
- shader-based atmosphere,
- scene presentation,
- integration of multiple graphics techniques into one pipeline.

If I had more time, I would improve:
- UI / on-screen feedback,
- stronger game-state presentation,
- more detailed platform environment assets,
- possibly a more research-led black hole or lensing implementation.

Overall, the project demonstrates a working OpenGL graphics prototype with custom shader work, post-processing, interactivity, and a clear visual theme.

---

## Links

- **GitHub Repository:** [YOUR GITHUB REPO LINK]
- **YouTube Video:** [YOUR UNLISTED YOUTUBE LINK]

---
