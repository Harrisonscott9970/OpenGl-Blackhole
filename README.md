# COMP3015 – Games Graphics Pipelines  
## Coursework Project – Black Hole Research Platform  
**Student:** [YOUR NAME]  
**Student ID:** [YOUR STUDENT ID]  
**Module:** COMP3015 – Games Graphics Pipelines  
**Module Leader:** Dr Ji-Jian Chin  
**Repository:** [YOUR PUBLIC GITHUB REPO LINK]  
**Video Demo:** [YOUR UNLISTED YOUTUBE LINK]  
**Tested OS:** [YOUR WINDOWS VERSION]  
**Tested IDE:** [YOUR VISUAL STUDIO VERSION]

---

## Project Overview

This project is an OpenGL coursework submission developed from the COMP3015 lab template.  
It presents a stylised black hole scene built around a custom GLSL shader pipeline, textured scene rendering, HDR post-processing, bloom, cubemap skybox rendering, and interactive controls.

The scene is designed as a cinematic sci-fi environment in which the player explores a layered research platform positioned near a massive black hole. The prototype combines custom shader effects, post-processing, procedural/code-side geometry, and lightweight gamification into a single graphics-focused experience.

The visual goal was to create a dramatic and recognisable black hole environment inspired by cinematic science-fiction, while still keeping the rendering pipeline understandable, modular, and suitable for real-time execution in OpenGL.

This README explains:
- how to run the project,
- how the code is structured,
- what graphics and shader techniques were implemented,
- what makes the work distinct,
- how AI was used,
- and how research informed the black hole presentation.

---

## Main Features

### Rendering / Shader Features
- Custom black hole shader
- Custom accretion disk shader
- Skybox / cubemap environment
- HDR rendering with tone mapping
- Bloom post-processing using ping-pong blur
- Gaussian-style fullscreen blur pass
- Optional cinematic “film mode” screen effect
- Animated lighting
- Fragment-shader based lighting for platform and scene objects
- Texture sampling for scene materials
- Normal map support in the platform/object shader path

### Scene Features
- Custom generated platform mesh
- Multi-layer platform composition
- Custom generated accretion disk mesh
- Black hole placed as a large-scale background focal point
- Collectible glowing energy cores
- Satellite platform layout / mission space
- Window-title state feedback during play

### Interaction Features
- WASD movement
- Mouse camera look
- Keyboard toggles for HDR / bloom / film mode
- Exposure control
- Reset control
- Basic game loop with collectible objectives and fail/win conditions
- Black-hole pull mechanic outside safer platform space

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

### Important Note
All shader files, textures, cubemap images, and resources must remain in the correct relative paths inside the project/submission folder.

---

## Controls

| Key / Input | Action |
|---|---|
| `W` `A` `S` `D` | Move camera |
| `Mouse` | Look around |
| `Left Mouse` | Interact / collect nearby energy cell |
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
The idea was to create a visually distinctive graphics prototype that demonstrates shader programming, scene composition, lighting, texture work, and post-processing rather than a conventional game environment.

The black hole itself is the main focal point, supported by:
- a distorted skybox to suggest gravitational lensing,
- an emissive accretion disk,
- HDR and bloom for glow and brightness response,
- and a cinematic film pass to enhance presentation.

The platform acts as the player’s safe space and gameplay area, while the black hole creates danger, atmosphere, and visual identity.

The aim was not to build a physically exact astrophysics simulator. Instead, the goal was to create a stylised, recognisable, real-time black hole scene that demonstrates strong graphics programming choices and a coherent artistic direction.

---

## How the Project Works

The render pipeline follows a multi-stage structure.

### 1. Scene Rendering to HDR Framebuffer
The skybox, platforms, black hole, accretion disk, satellite platforms, and collectibles are rendered into a floating-point framebuffer.  
This preserves bright values before the final fullscreen composite.

### 2. Bright Pass and Blur
Bright areas are isolated and blurred using repeated ping-pong fullscreen passes.  
This creates bloom around emissive objects such as the accretion disk and bright scene highlights.

### 3. Final Fullscreen Composite
The HDR scene texture and blurred bloom texture are combined in the final fullscreen shader.  
Tone mapping and exposure are applied here.  
Film mode can also be enabled as a final presentation pass.

### 4. Gameplay / Scene Update
Alongside rendering, the scene update logic:
- processes movement and mouse look,
- updates animated lighting,
- animates collectibles,
- applies black-hole pull,
- checks objective completion,
- and handles win/lose/reset flow.

This structure keeps the project modular and reflects a more modern rendering pipeline rather than a single-pass scene.

---

## Code Structure / Navigation Guide

The project is based on the COMP3015 template and is organised around a central scene class.

### Main Files

#### `main.cpp`
The program entry point.  
This initialises the application and launches the main render/update loop.

#### `scenebasic_uniform.h / scenebasic_uniform.cpp`
This is the main scene implementation and the core of the project.  
It is responsible for:
- compiling and linking shaders,
- loading textures and cubemap resources,
- creating custom geometry,
- managing framebuffers,
- updating scene/game state,
- and rendering all scene passes.

### Shader Files

#### `blackhole.vert / blackhole.frag`
Used to render the black hole sphere.  
This shader is designed to simulate:
- a dark event-horizon-like centre,
- warped cubemap sampling,
- silhouette/ring emphasis,
- and stylised gravitational lensing.

#### `disk.vert / disk.frag`
Used to render the accretion disk around the black hole.  
This shader creates:
- layered emissive colour,
- hotter inner ring tones,
- animated streaking and variation,
- radial shaping and falloff,
- and separated front/back disk rendering for better compositing.

#### `platform.vert / platform.frag`
Used to render the platform and scene objects.  
This includes:
- fragment-based lighting,
- texture sampling,
- optional normal map support,
- and emissive / glow-enhanced collectible visuals.

#### `skybox.vert / skybox.frag`
Used for rendering the cubemap background.

#### `hdr.vert / hdr.frag`
Used for fullscreen post-processing.  
Handles:
- HDR tone mapping,
- bloom compositing,
- exposure control,
- and optional film mode.

#### `blur.frag`
Performs repeated Gaussian-style blur passes used for bloom.

---

## Shader / Graphics Techniques Used

### 1. Fragment Shader Lighting
The project uses lighting in fragment shaders for scene objects, allowing stronger surface detail and more convincing highlights than a simple vertex-lit pipeline.

### 2. Blinn-Phong Style Shading
The platform/object shader uses a halfway-vector style approach for specular lighting, giving a stronger and more controlled highlight response.

### 3. Multi-Light Scene Support
The scene supports more than one light contribution, helping the platform and objects feel less flat and improving scene readability.

### 4. Texture Sampling
The project uses texture sampling for platform materials and object surfaces, satisfying the requirement for textured rendering rather than relying only on flat colour.

### 5. Normal Mapping Support
The platform/object shader includes support for tangent-space normal mapping, allowing richer lighting response where enabled.

### 6. Skybox Rendering
A cubemap skybox is used to create the surrounding deep-space environment and is also sampled by the black hole shader.

### 7. HDR Rendering
The scene is rendered into a floating-point framebuffer so that bright highlights can be preserved before tone mapping.

### 8. Bloom
Bright areas are extracted and blurred to create glow around emissive scene elements such as the accretion disk and highlights.

### 9. Fullscreen Post-Processing
The project uses a final fullscreen composite pass for tone mapping, bloom composition, exposure control, and optional film-style presentation.

### 10. Procedural / Code-Side Geometry
Several pieces of scene geometry are generated in code rather than relying entirely on imported meshes:
- sphere,
- accretion disk,
- platform mesh.

### 11. Sensible Animation
The scene includes:
- animated light movement,
- animated accretion disk behaviour,
- collectible pulse / motion,
- and reset/fade state transitions.

---

## What Makes the Shader Program Special

The strongest part of the project is its visual focus on the black hole and surrounding environment.

Rather than using a generic single-object lighting demo, this project attempts to build a more distinctive scene that combines:
- custom procedural-style shader work,
- post-processing,
- stylised space visuals,
- and gameplay interaction.

The black hole shader is designed to do more than simply colour a sphere black.  
It bends sampled background direction data, exaggerates edge readability, and works together with the accretion disk to create a recognisable black-hole silhouette.

The accretion disk complements this with layered emissive shaping, animated variation, turbulence-inspired banding, and warm/cool colour separation to create a stronger sense of motion and energy.

This makes the project more visually intentional than a basic shader showcase and gives it a stronger portfolio identity.

---

## Research-Informed Design

A key influence on the visual direction of this project was research into how black holes and accretion disks are visualised in real-time rendering.

A particularly relevant paper is:

**Bruneton, É. (2020). _Real-time High-Quality Rendering of Non-Rotating Black Holes_.**

This paper focuses on rendering a non-rotating black hole with an accretion disk and background stars, including light distortion effects.  
My project is **not** a full reproduction of that paper. Instead, the paper informed the **visual goals** of the prototype.

Research-informed goals included:
- making the black hole distort the background,
- ensuring the silhouette remains visually readable,
- using the accretion disk to strengthen the final shape,
- and building a recognisable black-hole presentation in real time.

My implementation translates those ideas into a coursework-appropriate GLSL solution using:
- iterative/stylised ray bending through the cubemap,
- ring/silhouette enhancement,
- a custom animated accretion disk,
- and HDR/bloom for final presentation.

This research connection is important because it shows that the project’s black-hole visuals were not designed blindly.  
The implementation is stylised and simplified, but it is informed by real black-hole visualisation ideas and adapted into a real-time OpenGL coursework scene.

### Research Honesty Statement
This prototype is a **stylised real-time approximation**, not a scientific simulator.  
The intention is to demonstrate graphics programming, visual design, and technical integration rather than astrophysical accuracy.

### Research Evidence Placeholder
- **Research paper screenshot / citation page:** [ADD IMAGE OR PDF HERE]
- **Notes showing how the paper informed the shader design:** [ADD IMAGE OR PDF HERE]

---

## Gameplay / Gamification

The project includes lightweight gamification so that the scene functions as more than a passive visual showcase.

### Objective
The player must:
- move between platforms,
- collect the energy cores,
- avoid being pulled too close to the black hole,
- and return safely after completing the objective.

### Mechanics
- collectible objects,
- safe platform region,
- black-hole attraction outside the safer area,
- fail state if the player gets too close,
- win state when objectives are completed,
- and reset/replay support.

### Why Gameplay Was Included
The goal was to give the scene a clear interactive purpose.  
This helps frame the graphics work inside a simple game-like loop and supports the rubric’s gamification category by providing evident mechanics, player goals, success/failure states, and replayability.

---

## Development Process

The project started from the COMP3015 lab template and was gradually extended into a more stylised and feature-rich scene.

The main development stages were:
1. setting up the scene structure and movement,
2. creating the platform layout,
3. adding the black hole focal object,
4. adding the accretion disk,
5. integrating the cubemap skybox,
6. adding textured lighting for the scene,
7. adding HDR and bloom,
8. refining the black hole composition and visual scale,
9. adding gameplay features and interaction,
10. adding a film-style fullscreen effect,
11. improving visual tuning and presentation.

A large amount of the work involved iterative visual tuning rather than purely functional coding.  
Examples include:
- moving the black hole farther away for better composition,
- adjusting pull strength for playability,
- refining the accretion disk so it reads clearly,
- balancing bloom intensity and exposure,
- and improving scene readability.

---

## Performance / Engineering Notes

This project balances visual quality and simplicity.

### Good Practice Choices
- scene rendering is separated into passes,
- shader roles are clearly separated,
- fullscreen post-processing is modular,
- controls are simple and easy to test,
- scene logic and render logic are separated,
- the project remains understandable despite multiple techniques being combined.

### Trade-Offs
- the black hole effect is visually inspired rather than physically exact,
- bloom and fullscreen effects improve presentation but add cost,
- the film mode is intentionally subtle so it does not hide scene detail,
- some values are tuned artistically for playability rather than realism.

The goal was to produce a convincing real-time graphics prototype rather than a full physical simulation.

---

## Evaluation

I believe the project succeeds most strongly in:
- visual identity,
- shader-based atmosphere,
- scene presentation,
- and the integration of multiple graphics techniques into one pipeline.

The strongest technical areas are:
- custom black hole and accretion disk shader work,
- HDR + bloom post-processing,
- the combined use of lighting, texturing, and post-processing,
- and the attempt to connect the final visuals to black-hole rendering research.

If I had more time, I would improve:
- stronger HUD / on-screen UI feedback,
- more detailed environmental set dressing,
- more advanced game-state presentation,
- and a more research-accurate lensing approach.

Overall, the project demonstrates a working OpenGL graphics prototype with custom shader work, texture usage, lighting, post-processing, interactivity, and a clear visual theme.

---

## AI Usage Declaration

This project used generative AI in a partnered-work manner consistent with the assignment brief.

AI assistance was used for:
- code assistant support,
- debugging and shader iteration,
- gameplay/system design discussion,
- rendering pipeline discussion,
- and README/report drafting support.

All AI-assisted work was reviewed, tested, integrated, and adapted into the final submission by the student.

The final implementation decisions, debugging, integration, testing, and submission curation were carried out by the student.

### AI Declaration Evidence
- **Signed Generative AI declaration form:** [ADD PDF OR IMAGE HERE]

---

## AI Prompt and Transcript Evidence

As required by the module brief, AI-supported features should be accompanied by prompt and transcript evidence where applicable.

### Evidence Index
- **Appendix A – Black hole shader iteration:** [ADD SCREENSHOT / PDF HERE]
- **Appendix B – Accretion disk shader iteration:** [ADD SCREENSHOT / PDF HERE]
- **Appendix C – HDR / bloom / exposure support:** [ADD SCREENSHOT / PDF HERE]
- **Appendix D – Gameplay / black-hole pull / reset loop:** [ADD SCREENSHOT / PDF HERE]
- **Appendix E – README / report support:** [ADD SCREENSHOT / PDF HERE]

### Feature-to-Evidence Mapping

| Feature | AI Used? | Evidence |
|---|---|---|
| Black hole shader development | Yes | Appendix A |
| Accretion disk shader development | Yes | Appendix B |
| HDR / bloom / post-processing support | Yes | Appendix C |
| Gameplay / black-hole pull / reset flow | Yes | Appendix D |
| README / documentation drafting | Yes | Appendix E |

### Example Prompts Used
- “Help me make a stylised black-hole shader in GLSL.”
- “Improve the accretion disk so it looks more cinematic and turbulent.”
- “Help me add HDR, bloom, and exposure controls.”
- “Tune the black-hole pull so it is dangerous but still playable.”
- “Help me rewrite the README to better match the rubric.”
- “Suggest how to explain the project clearly in the video.”

### AI Reflection
AI was used as a development support tool rather than a replacement for implementation.  
Suggestions often required adaptation, debugging, simplification, and testing before they could work properly inside the COMP3015 template and final scene structure.

---

## Sources / References / What I Adapted

This section should remain honest and specific.  
If any external code, algorithm, design idea, or tutorial informed the final implementation, it should also be cited in comments and explained in the video where appropriate.

### Academic / Research Inspiration
- Bruneton, É. (2020). *Real-time High-Quality Rendering of Non-Rotating Black Holes*.

### Module Base
- COMP3015 lab template provided as part of the module.

### Additional Development Support
- [ADD ANY TUTORIAL / ARTICLE / FORUM SOURCE YOU USED]
- [ADD ANY STACK OVERFLOW / GITHUB / REFERENCE SOURCE YOU USED]
- [ADD ANY ADDITIONAL RESEARCH SOURCES YOU USED]

---

## Video Report

**Unlisted YouTube video:**  
[YOUR UNLISTED YOUTUBE LINK]

The video should cover:
- the project idea,
- what makes the scene visually distinctive,
- how the shader pipeline works,
- the code structure,
- controls and interaction,
- HDR / bloom / film mode demonstration,
- gameplay features,
- development problems encountered,
- and how research informed the black-hole presentation.

### Suggested Insert Placeholders
- **Full scene screenshot:** [ADD IMAGE HERE]
- **Black hole close-up:** [ADD IMAGE HERE]
- **Accretion disk close-up:** [ADD IMAGE HERE]
- **HDR/bloom comparison:** [ADD IMAGE HERE]
- **Gameplay objective screenshot:** [ADD IMAGE HERE]
- **Shader code screenshot:** [ADD IMAGE HERE]

---

## Repository / Submission Notes

This repository is intended to reflect the submitted coursework version.

Before submission, ensure that:
- the DLE zip matches the repository,
- all resources required to compile and run are included,
- the executable is included if required,
- this README is finalised with no placeholder metadata left behind,
- the public GitHub link works,
- the unlisted YouTube link works,
- AI evidence is included,
- the signed AI declaration form is included,
- unnecessary generated files are removed,
- and any adapted external sources are properly referenced.

---

## Links

**GitHub Repository:** [YOUR PUBLIC GITHUB REPO LINK]  
**YouTube Video:** [YOUR UNLISTED YOUTUBE LINK]
