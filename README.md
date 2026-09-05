# PGK

A C++ / OpenGL 3.3 core scene: a hollow-cube room, a handful of primitives
(spheres, cubes, cones), five point lights, free-fly camera movement, and a
collision/physics system with exact sphere/cone/box colliders — objects
fall under gravity, bounce off the room and each other, and can be pushed
around by the player.

## Dependencies

Fetched automatically by CMake ([`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html)) at configure time — no manual download/vendoring needed:

- [GLFW](https://github.com/glfw/glfw) 3.4 — window creation and input
- [glad](https://github.com/Dav1dde/glad) v2.0.8 — OpenGL 3.3 core function loader
- [GLM](https://github.com/g-truc/glm) 1.0.1 — math library

### Prerequisites on the build machine

- **CMake ≥ 3.20**
- **Visual Studio Build Tools (or full Visual Studio)** with the "Desktop development with C++" workload
- **Internet access** the first time you run `cmake` configure (dependency source is cloned into `build/_deps/`)
- **Python 3 on `PATH`, with the `Jinja2` package installed** (`pip install Jinja2`) — glad v2 generates its loader source at configure time by running `python -m glad`, using CMake's `find_package(Python COMPONENTS Interpreter REQUIRED)`. Without this, configure fails at the `glad_add_library` step.

## Building (Windows)

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

If your installed toolset is newer than 2022 (check with `cmake --help` — the
generator list marks the default with `*`), pass that generator name instead,
e.g. `-G "Visual Studio 18 2026"`. CMake matches the generator string to a
specific installed MSVC toolset; it does not fall back to a newer one
automatically.

The `PGK` executable is written to `build/bin/`, with `assets/` (shaders,
textures) copied alongside it after each build.

## Controls

| Input | Action |
|---|---|
| `W` / `A` / `S` / `D` | Move the camera (relative to where it's looking) |
| Hold right mouse button + move mouse | Look around |
| Left click | Push whatever's directly in front of the camera, within range — cones can't be pushed |
| `Esc` | Quit |

## Project layout

```
include/pgk/   public headers, mirrors src/ by subsystem
src/           implementation
  core/        application bootstrap, window, input handling
  camera/      free-fly camera
  graphics/    shader, mesh, renderer
  scene/       scene graph, room, generic game object
  geometry/    procedural primitives (cube, sphere, cone, plane)
  lighting/    point lights
  physics/     exact sphere/cone/box colliders, rigid-body fall + bounce
assets/
  shaders/     GLSL sources
  textures/    texture assets
docs/          write-up material, diagrams, screenshots
```

## AI usage

Prompts used while developing this project are logged in [`AI.md`](AI.md).
