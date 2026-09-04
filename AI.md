# AI Prompt Log

This file logs the prompts used with AI assistance (Claude Code) throughout
the development of this project, for transparency and academic integrity
purposes.

Each entry: date, the prompt (verbatim or summarized), and what it produced.

---

## 2026-09-04

**Prompt:**
> Hi. You are in a project directory for a final university assignment. The
> goal is to create a program using c++ and openGL 3.3 core or highier. The
> final program will consist of the following: a scene consisting of a room
> (a hollow cube made out of "walls" that are themselves cubes, just
> correctly transformed); few simple objects like spheres, cubes, cones etc.;
> a lighting system consisting of one global light and a directional light
> coming from the camera (the directional light only on after user input);
> free camera to move around the scene; a collision system for all objects
> in the scene and simple physics to bounce the ball around. This directory
> was only just git inited. Plan out the file structure of the project.
> Create all the folders and files (no code!). Make sure that all the
> dependencies are in place. Aside from openGL 3.3 core and glm the project
> will use cmake to create and build windows 2022 project files and glad
> library for window and user input. Also inside the root directory of the
> project create AI.md file that will log all the prompts used throughout
> the project.

**Result:** Scaffolded the project skeleton — `include/pgk/` and `src/`
subsystem folders (core, camera, graphics, scene, geometry, lighting,
physics), `assets/shaders` + `assets/textures`, `docs/`, empty header/source
placeholders for each subsystem, root `CMakeLists.txt` wiring GLFW 3.4, glad
v2.0.8 (generated for `gl:core=3.3` via its CMake integration), and GLM 1.0.1
through `FetchContent`, plus `.gitignore`, `README.md`, and this file.

---

**Prompt:**
> Go through the installation. Install only what's necessairy

**Result:** Installed the local build toolchain: Python 3.13 (per-user, via
`winget`) plus `Jinja2` (glad v2's code-generation dependency), and Visual
Studio Build Tools with the "Desktop development with C++" workload
(installed by the user directly in an elevated shell, since the installer
needs admin rights `winget` can't self-elevate non-interactively). Ran a real
`cmake` configure + `cmake --build` against the project to confirm the whole
chain works: GLFW, GLM, and glad (real Python-generated OpenGL 3.3 core
loader) all fetched and compiled, and every placeholder `.cpp` compiled
cleanly — the build only fails at the link step for a missing `main`, which
is expected since no application code exists yet. Note: the installed
toolset identifies to CMake as generator `"Visual Studio 18 2026"`, not
`"Visual Studio 17 2022"` — `README.md` updated to explain both and how to
tell which one a given machine needs.

---

**Prompt:**
> You can code all the boilerplate stuff. Window and context creation,
> openGL basic pipeline - setting up data and buffers, sending data to GPU,
> shaders compilation etc. Give me an exact report on what you did and
> where. After that walk me through the current project structure - what is
> left to code up and where to put it.

**Result:** Implemented the engine-infrastructure layer (no scene/assignment
logic):
- `core/Window` — GLFW init, 3.3-core context creation, `gladLoadGL` function
  pointer loading, resize callback hook.
- `graphics/Renderer` — minimal GL state wrapper (depth test, clear, viewport).
- `core/Application` — owns `Window` + `Renderer`, provides the `run(onUpdate,
  onRender)` main loop.
- `graphics/Shader` — compiles/links GLSL from disk with real error surfacing,
  plus uniform setters (bool/int/float/vec2-4/mat3/mat4).
- `graphics/Mesh` — shared `Vertex{position,normal,texCoords}` layout,
  VAO/VBO/EBO upload, move-only, `draw()`.
- `assets/shaders/basic.vert`/`basic.frag` — throwaway unlit smoke-test
  shader (not the final lighting shader).
- `src/main.cpp` — hardcoded triangle exercising the whole pipeline, clearly
  marked to be deleted once `Scene` exists.
- `CMakeLists.txt` — added `VS_DEBUGGER_WORKING_DIRECTORY` so F5-debugging in
  Visual Studio finds the copied `assets/`.

Verified for real: `cmake --build` compiled all 20 `.cpp` files clean under
`/W4` (zero warnings), and `PGK.exe` was launched and screenshotted —
confirmed a titled "PGK" window with the dark clear color and orange
triangle rendering correctly. Then walked through `include/pgk/` and `src/`
marking what's implemented vs. still empty (camera, input, geometry, scene,
lighting, physics) and the suggested build order between them.
