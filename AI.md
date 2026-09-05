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

---

**Prompt:**
> I implemented the cube. CAn you check?

**Result:** User wrote `include/pgk/geometry/Cube.h` + `src/geometry/Cube.cpp`
(24-vertex, per-face-flat-normal cube, 36 indices) and switched `main.cpp` to
render it instead of the smoke-test triangle. Reviewed by building
(`cmake --build`, clean under `/W4`), running the exe, and screenshotting it.
Findings reported:
- Geometry itself is correct (verified winding/outward-normal consistency by
  hand on several faces).
- Renders as a flat orange square, not a visible cube — expected, since
  `basic.vert/frag` still has no model/view/projection transform and no
  per-face shading; not a defect in the cube data.
- Real bug: `Cube.h` declared a member function `Cube::cube(...)` that was
  never defined (a same-named but unrelated free function existed in the
  `.cpp` instead) — dead declaration that would fail to link if ever called,
  and whose parameter names shadowed the class's own data members.
- Dead code: an unused `triangleVertices` vector left over in `main.cpp`.
- Nitpick: cube data stored as non-static members of an instantiable class
  rather than as a free-function generator.

---

**Prompt:**
> Did as you suggested with free function. Looks good now?

**Result:** User refactored to a free function `pgk::buildCubeMesh()` in
`Cube.cpp`/`Cube.h`, called from `main.cpp` as
`pgk::Mesh cube = pgk::buildCubeMesh();`, and removed the dead triangle code.
Re-verified: rebuilt (binary timestamp confirmed newer than both edited
source files), zero warnings, launched and stayed responsive with no crash.
Both real issues from the previous review are resolved. Noted one purely
cosmetic nit (an unnecessary `const&` bound to a braced-init-list temporary
in `Cube.cpp` — legal and correct, just stylistically needless) and reminded
that the flat-square look is still expected until `Camera` + a real shader
exist.

---

**Prompt:**
> I setup the camera class with the some lookAt values hardcoded. I wanted
> to test it, so i created a new shader but it looks like it cannot compile.
> I am not sure where to set the projection and view matrix for it, so maybe
> that's the problem

**Result:** User wrote `pgk::Camera` (position/yaw/pitch/aspect, hardcoded
`glm::lookAt`) and `assets/shaders/basic_cam.vert`. Diagnosed by actually
building and running (twice, with a temporary filename patch to get past
the first error): found two stacked bugs plus a convention mismatch:
- `main.cpp` requested `assets/shaders/basic_camera.vert`, a filename that
  didn't exist on disk (`basic_cam.vert`) — confirmed via the actual
  "Failed to open shader file" runtime error.
- Once that path was patched, the real GLSL compile error surfaced:
  `vec4(aPosition, 1.0;` was missing its closing paren — confirmed via the
  driver's own `0(12) : error C0000` message.
- `view * projection * vec4(...)` had the multiply order backwards (should
  be `projection * view * ...`).
- The user's own suspicion was right: `setMat4("projection"/"view", ...)`
  was called before `basicShader.use()` — `glUniformMatrix4fv` always
  targets whichever program is currently bound, so those calls were no-ops.
- Also flagged: `Camera.cpp`'s lookAt used `glm::vec3(0,0,1)` as up, while
  `Cube.cpp` treats `+Y` as up — inconsistent convention.

---

**Prompt:**
> all corrected. You can fill in the correct values in the lookAt

**Result:** Verified the user's fixes (filename, closing paren, multiply
order, `use()` before `setMat4`, up vector already changed to `(0,1,0)`) —
all correct. Then filled in the one remaining hardcoded value: the `lookAt`
call always targeted `glm::vec3(0,0,0)`, ignoring the `yaw`/`pitch`
parameters entirely. Added a `front` member to `Camera` (`include/pgk/camera/Camera.h`)
and computed it from yaw/pitch in `Camera.cpp` using the standard
spherical-to-cartesian formula, then built the view matrix from
`lookAt(position, position + front, {0,1,0})` instead of a fixed target.
Rebuilt (clean) and screenshotted: renders as a flat orange square again,
but confirmed this is geometrically correct, not a bug — the camera at
`(-3,0,0)` looks exactly down the `+X` axis at an axis-aligned cube, so only
one face is visible; on-screen size matches the expected footprint for a
1x1x1 cube at distance 3 with a 60 deg FOV. Suggested nudging `yaw` off zero
to visually confirm multiple faces render correctly.

---

**Prompt:**
> You can cleanup the camera class. Do a destructor, and get rid of the move
> and rotate functions. They should be in input later

**Result:** `include/pgk/camera/Camera.h` — confirmed `rotate()`/`move()`
were unimplemented stubs never defined in `Camera.cpp` and never called
anywhere (grepped the whole repo first), then removed both declarations.
Replaced the commented-out `//~Camera();` with a real `~Camera() = default;`
(the class owns no resources — plain `glm` value types only — so there's
nothing to release, but it documents intent). Rebuilt clean; only the
pre-existing unrelated `main.cpp:19` narrowing warning remains.

---

**Prompt:**
> Setup the input class schafolding for me. The controlls will be as
> folows:
> - WSAD keys let you move the camera, where W moves you in the direction
>   of the front.
> - When and only when the right mouse button is down you can rotate the
>   camera with the movement of the mouse.

**Result:** Implemented `pgk::Input` (`include/pgk/core/Input.h` /
`src/core/Input.cpp`): `update(Camera&, float deltaSeconds)` moves the
camera along its own `front`/`right` axes on W/A/S/D (normalized so
diagonal movement isn't faster) and, only while the right mouse button is
held, converts the mouse delta since last frame into yaw/pitch changes
(pitch clamped to +-89 deg), guarding the first frame of a drag against a
stale-position jump. Added `Camera::updateViewMatrix()` (factored out of the
constructor, which now calls it too) so both construction and `Input` share
one front/view computation. Wired into `main.cpp`: constructs `Input`, calls
`input.update(camera, dt)` from the update callback, and moved
`setMat4("view", ...)` into the render callback since the view now changes
every frame instead of being fixed at startup.

Verified with actual simulated OS input (`user32.dll` `keybd_event`/
`mouse_event` via PowerShell), not just by reading the code: held W for
500ms — cube visibly grew closer; held the right mouse button and dragged
200px — view rotated and the cube went from showing one flat face to two
adjacent faces at an angle, which also incidentally confirms the cube
geometry, camera, and projection are all correct together.

---

**Prompt:**
> We can work on the GameObject class. I assume it should consist of the
> game objects mesh, position, rotation, scale... What else should be in
> the class?

**Result:** Advisory only, no code written. Recommended: a color/material
field (the shader's `uColor` needs to differ per object), a non-owning
`Mesh*`/reference rather than owning a `Mesh` by value (so objects can share
GPU buffers instead of each duplicating them), and a cached model matrix
plus a `draw(Shader&)` method. Flagged the open design question of whether
physics state belongs on `GameObject` directly vs. `PhysicsWorld` tracking
bodies separately — left for later since `Collider`/`RigidBody` don't exist
yet.

---

**Prompt:**
> How can I calculate the rotation angle for the rotate matrix, based on the
> rotation vec3?

**Result:** Explanation only, no code written. Covered building a `mat4`
from a `vec3` of Euler angles via `glm::mat4_cast(glm::quat(glm::radians(rotation)))`
(recommended, since `glm::rotate` only takes one angle+axis at a time and
GLM's `quat(vec3)` constructor fixes the axis-composition order for you),
versus manually chaining three `glm::rotate` calls (order-sensitive, easy to
get inconsistent results).

---

**Prompt:**
> Is this function for drawing the object good: [pasted an early draft of
> GameObject::draw] ... how can I call the draw function from mesh?

**Result:** User had already written `GameObject.h`/`.cpp` with the fields
discussed above. Reviewed by reading the actual files (not just the pasted
snippet) and building — confirmed a real link error matching what the code
predicted:
- `static glm::mat4 modelMatrix;` in the header made the matrix shared
  across every `GameObject` instance instead of per-object, and had no
  out-of-class definition — `LNK2001` on `pgk::GameObject::modelMatrix`.
- `.cpp` defined a free function `pgk::draw(Shader&)` instead of
  `GameObject::draw(Shader&)` — same class of mistake as the earlier `Cube`
  bug (missing `GameObject::` qualifier).
- `color` was stored but never uploaded as `uColor`.
Gave the corrected `draw()` (`shader.use()`, `setMat4("model", ...)`,
`setVec3("uColor", color)`, `mesh->draw()` since `mesh` is a pointer) and
offered to apply; user applied the fixes themselves.

---

**Prompt:**
> You can check the current state before I commit. In the future there will
> be an update funciton for interacting with the physics. Also you can
> setup a simple file for performence measurment in the utils folder. I
> want to display the fps in the window top bar

**Result:** Reviewed the user's own fixes to `GameObject` (all three issues
from the previous review resolved) plus two new `GameObject`s wired into
`main.cpp` with a new `assets/shaders/basic_model.vert` (adds a `model`
uniform, `projection * view * model * vertex` — correct order). Built clean
and screenshotted: two independently-positioned/colored/rotated cubes
render correctly, confirming per-instance model matrices and shared-mesh
rendering both work. Flagged (not yet fixed, since there's no `update()` to
call it from yet): `modelMatrix` is cached at construction only, so once a
physics `update()` mutates position/rotation/scale later, `draw()` needs
`modelMatrix` recomputed too (same pattern as `Camera::updateViewMatrix()`).

Then implemented FPS display: added `Window::setTitle(const std::string&)`
(`core/Window.h`/`.cpp`, wraps `glfwSetWindowTitle`) and a new
`pgk::FpsCounter` (`include/pgk/utils/FpsCounter.h` / `src/utils/FpsCounter.cpp`
— the first file in `src/utils/`, which didn't exist before) that averages
frame time over ~1-second windows and reports `true` from `update()` only
when its value just refreshed. Wired into `main.cpp`'s update callback to
push a new window title (`"PGK - <fps> FPS"`) only on those refreshes.
Verified at runtime (not just by reading the code): launched the exe and
polled `MainWindowTitle` twice, ~1.3s apart — got `"PGK - 63 FPS"` then
`"PGK - 60 FPS"` (60 matches the vsync-locked refresh rate `Window` already
enables). Also noted for future reference: the first build after adding
`FpsCounter.cpp` hit a transient `LNK2019` because `CONFIGURE_DEPENDS`
regenerated project files mid-build without the new file; a second
`cmake --build` picked it up fine — not a code bug, just a one-time hazard
whenever a new `.cpp` file is added.
