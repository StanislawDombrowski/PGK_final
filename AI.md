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

---

**Prompt:**
> We can work on the lighting part. Can you walk me through what to setup
> in the point light and how to prepare the shader?

**Result:** Explanation only, no code written. Walked through: `PointLight`
fields (position, color, ambient/diffuse/specular strength scalars,
constant/linear/quadratic attenuation); `lit.vert` needing to output
world-space `FragPos` and a properly transformed `Normal` (via the normal
matrix `transpose(inverse(model))`, needed because `GameObject` allows
non-uniform scale); and `lit.frag`'s Blinn-Phong calculation (halfway-vector
specular, distance attenuation, `viewPos`/`uColor` uniforms). Flagged that
`GameObject` has no shininess field — recommended hardcoding a shader
constant for now rather than adding a material struct.

---

**Prompt:**
> Should there be 'use' function that makes the light work?

**Result:** Advisory only. Recommended against naming it `use()` — unlike
`Shader::use()` (a real `glUseProgram` state change), a light has no GL
object to activate; it just pushes CPU-side data into whichever shader is
already bound. Suggested `uploadTo(Shader&) const` instead.

---

**Prompt:**
> Could this functionality just be in a constructor?

**Result:** Advisory only. Explained why not: a constructor runs once, but
the light's uniforms need refreshing whenever the light's data changes or a
different shader needs them — same reason `view`/`projection` are re-set
every frame rather than once at startup.

---

**Prompt:**
> How do I use shader.set... in this shader setup?

**Result:** User had already written `PointLight.h`/`.cpp` themselves
(constructor + empty `uploadTo` stub). Pointed out `PointLight::uploadTo(Shader&){`
was missing its `void` return type (wouldn't compile), then gave the
`uploadTo` body: one `shader.setVec3`/`setFloat` call per field, using
`"pointLight.<fieldName>"` uniform-name strings to match the GLSL struct's
instance name, and reiterated the `shader.use()`-before-`set*` ordering.

---

**Prompt:**
> Can you suggest some default values for a PointLight object for testing?

**Result:** Explanation only, no files edited. Suggested position above the
cubes `(0, 3, 2)`, white color (isolates lighting-math correctness from
color-mixing correctness), `ambientStrength=0.15`/`diffuseStrength=0.8`/
`specularStrength=0.6`, and attenuation `(1.0, 0.35, 0.44)` (LearnOpenGL's
"range ~20" row) as a scene-scale-appropriate starting point.

---

**Prompt:**
> Okay, run the checks before I commit

**Result:** User had wired `PointLight` into `main.cpp` (with
`ambientStrength=5.f` instead of the suggested `0.15`) and written
`lit.vert`/`lit.frag` matching the walkthrough. Built clean and
screenshotted: real diffuse gradient shading visible on both cubes (not
flat color), FPS counter confirmed working in the title bar. Found one real
bug: `lit.frag`'s `viewPos` uniform was never set from `main.cpp`, silently
defaulting to GLSL's `(0,0,0)` — every specular highlight was being computed
as if the camera sat at the world origin instead of `camera.position`.
Noted `ambientStrength=5.f` happens to look fine at the light's current
distance due to attenuation, but would clip to white if the light moved
closer — not urgent, just flagged.

**Prompt:**
> Yes, add that

**Result:** Added `basicShader.setVec3("viewPos", camera.position);` to
`main.cpp`'s render callback, alongside the other per-frame uniforms.
Rebuilt clean (only the pre-existing unrelated `main.cpp` narrowing
warning remains).

---

**Prompt:**
> I added a sphere mesh and setup the scene in main for testing. We can
> move to physics so that the sphere falls down. When that works we will
> move to collisons. Can you help me setup the physics side?

**Result:** User added `geometry/Sphere.h/.cpp` and a ground cube +
sphere `GameObject` in `main.cpp` themselves. Implemented the physics
scaffold (no collision yet, by design — that's the explicitly-agreed next
step):
- `GameObject::updateModelMatrix()` — extracted the constructor's TRS math
  into a reusable method (same pattern as `Camera::updateViewMatrix()`,
  which had been flagged as a prerequisite back when `GameObject`'s
  `modelMatrix`-caching was first reviewed).
- `pgk::RigidBody` (`include/pgk/physics/RigidBody.h` / `src/physics/RigidBody.cpp`)
  — references a `GameObject`, holds `velocity`/`useGravity`, `step(dt)`
  applies constant gravity (`-9.81`) to velocity, integrates position, and
  calls `updateModelMatrix()`.
- `pgk::PhysicsWorld` (`include/pgk/physics/PhysicsWorld.h` / `src/physics/PhysicsWorld.cpp`)
  — holds non-owning `RigidBody*` pointers (bodies stay owned wherever
  they're constructed, matching the project's existing ownership style),
  `step(dt)` iterates and steps each one.
- Wired into `main.cpp`: only the sphere gets a `RigidBody` (the ground
  cube stays static/never moves); `physicsWorld.step(deltaSeconds)` added
  to the update callback.

Verified with real screenshots, not just by reading the code: at 300/450/600ms
after launch the sphere is visibly sinking deeper into the ground cube frame
by frame (as expected — no `Collider` exists yet, so it passes straight
through); left running longer it falls out of frame entirely, consistent
with real quadratic gravity acceleration over ~1 second. Confirms the
integration math is correct; collision response (stopping/bouncing off the
ground) is the deliberately-deferred next step.

---

**Prompt:**
> Yes, we will implement the AABB collisions. Start the work on that

**Result:** Implemented AABB-vs-AABB collision detection + response (every
object, including the ball, approximated as a box collider — matches the
user's explicit choice of AABB over exact sphere geometry):
- `pgk::Collider` (`include/pgk/physics/Collider.h` / `src/physics/Collider.cpp`)
  — references a `GameObject`, stores `halfExtents`, computes `min()`/`max()`
  live from the object's current position (doesn't rotate with the object —
  the standard AABB trade-off). Free function `intersects(a, b)`.
- `RigidBody` extended to also reference a `Collider&`, plus
  `resolveCollision(const Collider& other)`: computes overlap on all three
  axes, and if genuinely overlapping, pushes the body out along whichever
  axis needs the smallest correction and reflects that axis's velocity
  component scaled by a new `restitution` field (default `0.6`, added now
  since "simple physics to bounce the ball" needs it, not spec­ulative).
- `PhysicsWorld` extended with `addStaticCollider(Collider&)` and a
  `m_staticColliders` list; `step()` now checks each dynamic body's
  collider against every static one after integrating it, and calls
  `resolveCollision` on overlap.
- Wired into `main.cpp`: `groundCollider` (half-extents `1.5,0.25,1.5`,
  matching the ground cube's scale) registered as static; `sphereCollider`
  (half-extents `1,1,1`, matching the sphere mesh's actual radius of `1.0`)
  attached to the sphere's `RigidBody`.

Verified with real screenshots at the user's actual scene setup (drop
height `y=0.8`): sphere settles right on top of the ground instead of
passing through. Since that drop height only produces a small, hard-to-see
bounce, additionally ran a temporary local-only test (dropped from `y=4.0`,
reverted after) to confirm actual velocity reflection works, not just
penetration correction — captured frames clearly showing the sphere land,
visibly bounce back up off the surface with real separation, then re-settle
with reduced amplitude on the next bounce, consistent with `restitution=0.6`
damping. Reverted the test change; `main.cpp`'s actual drop height is
untouched at `0.8`.

---

**Prompt:**
> Can you add a bounds field in the colider? I want to create an addForce
> function that will take the contact point at a colined to apply force at

**Result:** Asked a clarifying question first, since it was a real fork:
should `addForce` induce rotation (torque) at the contact point, or just
change linear velocity? User chose linear-only. Implemented:
- `Collider` — added a `Bounds{min, max}` struct and a `bounds()` accessor
  returning both together (existing `min()`/`max()` kept as-is).
- `RigidBody` — added a `mass` field (default `1.0`, re-added since forces
  need mass to become velocity changes — this is exactly the "not needed
  yet" case flagged when `mass` was deliberately dropped from the class
  earlier) and `addForce(const glm::vec3& force, const glm::vec3&
  contactPoint)`, which applies `force` as an instantaneous impulse
  (`velocity += force / mass`). `contactPoint` is accepted (parameter name
  commented out in the `.cpp` to avoid an unused-parameter warning) but has
  no effect yet, documented in the header as future torque-system-only,
  since `RigidBody` has no angular velocity/orientation state.
Rebuilt clean, no new warnings.

---

**Prompt:**
> Can you redo the changes in the rigidbody.h. I lost them.

**Result:** User's local edits to `RigidBody.h` had reverted the `mass`
field and mangled the `addForce` declaration into `void addForce(float
force, );` (wrong type, missing `contactPoint`, trailing comma —
non-compiling). `RigidBody.cpp` still had the correct implementation
from before, untouched. Restored the header to match: `mass` field back,
`addForce(const glm::vec3& force, const glm::vec3& contactPoint)` with its
explanatory comment. Rebuilt clean.

---

**Prompt:**
> Now inside the input class I want to apply the addForce function uinsg
> right mouse button to an object that I look at at the contact point that
> is an intersecion of a ray coming out of a camera center and the object.
> The force should only be applied if the distance from a camera is less
> than a set value. While you work on that I will prepare the scen in the
> main function.

**Result:** Implemented raycast-push in `pgk::Input`:
- `PhysicsWorld` — added a `bodies()` accessor (previously private-only)
  so `Input` can iterate dynamic bodies to raycast against.
- `Input::update()` signature extended to take `PhysicsWorld&`. Kept the
  existing right-mouse-drag camera-rotate behavior unchanged, and added a
  new **edge-triggered** action (fires only on the frame RMB transitions
  from up to down, not every frame it's held, reusing the existing
  `m_wasRotating` flag) — a private `tryPushObjectUnderCursor()` casts a
  ray from `camera.position` along `camera.front` (i.e. screen-center /
  "what you're looking at", not the OS mouse cursor position — the camera
  has no locked/hidden cursor, so "look direction" is the only sensible
  reading of "an object that I look at" here), tests it against every
  dynamic body's `Collider::bounds()` via a standard slab-method ray/AABB
  intersection (new private helper in `Input.cpp`), finds the nearest hit,
  and if that hit is within a new `m_pushMaxDistance` (default `10`), calls
  `body->addForce(camera.front * m_pushForceMagnitude, contactPoint)` with
  `m_pushForceMagnitude` defaulting to `15`. Both constants are private
  tunables on `Input`.
- Updated `main.cpp`'s one existing call site
  (`input.update(camera, deltaSeconds)` → `input.update(camera, physicsWorld,
  deltaSeconds)`) since that's just call-site wiring for the new parameter,
  not scene content — left everything else in `main.cpp` untouched since
  the user said they'd handle the scene setup themselves.

Verified with simulated OS input (quick right-click at screen center, no
drag): sphere visibly launched away from the camera across three frames,
shrinking into the distance — confirms the raycast hit, distance check
passed, and `addForce` fired correctly. Also noted (informational, not
fixed): the editor's clangd diagnostics show a stale error on
`main.cpp`/`Sphere.cpp` even though the real `cmake --build` compiles
clean — likely a clangd config/include-path mismatch, unrelated to the
actual MSVC build.

---

**Prompt:**
> Sorry, I mean that the add force is at left mouse button. The right
> button should be as it was

**Result:** Moved the push trigger from right-click to left-click:
`Input` gained its own `m_wasLeftMouseDown` edge-detection state
(separate from `m_wasRotating`, which right-click still uses only for
camera rotation), and `tryPushObjectUnderCursor()` is now called only on
the frame `GLFW_MOUSE_BUTTON_LEFT` transitions from up to down. Right-click
drag no longer triggers a push at all. Rebuilt clean and re-verified with
simulated input: a right-click-drag now only rotates the camera (sphere
stays physically in place, just shifts on screen as the view angle
changes), while a left-click launches the sphere away as before.

---

**Prompt:**
> Can you modify the lit.frag shader to accept multiple lights?

**Result:** User had added a wall `GameObject` + three `PointLight`s
(`pointLight1/2/3`) in `main.cpp`, each calling `uploadTo(basicShader)`.
Diagnosed why that wasn't going to work as-is before making any change:
`uploadTo` always wrote to the same `"pointLight.*"` uniform names
regardless of which `PointLight` called it, so all three calls overwrote
each other and only the last one (`pointLight3`) ever had any visual
effect. Fixed by making the shader and upload path array-aware:
- `lit.frag` — replaced the single `uniform PointLight pointLight;` with
  `uniform PointLight pointLights[MAX_POINT_LIGHTS]` (`MAX_POINT_LIGHTS`
  `#define`d to 4) + `uniform int numPointLights;`, factored the per-light
  Blinn-Phong math into a `calcPointLight()` function, and `main()` now
  sums `calcPointLight()` over `pointLights[0..numPointLights)` before
  multiplying by `uColor` once.
- `PointLight::uploadTo` — added an `int index` parameter; builds the
  uniform name prefix as `"pointLights[" + index + "]."` so each light
  writes to its own array slot instead of colliding.
- `main.cpp` — updated the three (now mechanically necessary) call sites to
  `uploadTo(basicShader, 0/1/2)`, and added
  `basicShader.setInt("numPointLights", 3)` alongside them.

Verified: launched the exe and confirmed it stayed running/responsive
(a GLSL compile error would throw immediately, caught by `main.cpp`'s
try/catch, and exit within milliseconds — it didn't), and the resulting
screenshot shows a broad, bright illumination spread across the ground
consistent with all three lights (spread across `x=-5..5`) contributing,
not just one. Noted for later: a 4th light would need `MAX_POINT_LIGHTS`
bumped in `lit.frag` too, since GLSL array uniforms are fixed-size.

---

**Prompt:**
> When the force is applied, the collision no longer works - the sphere
> falls through the floor

**Result:** Root-caused before changing anything: the user had resized the
ground `GameObject` to scale `(20, 0.3, 20)` and added a wall at
`(15, 10, 0.3)`, but `groundCollider`/`wallCollider` in `main.cpp` still
used the old `(1.5, 0.25, 1.5)` half-extents (a stale leftover, and the
wall's collider was just a copy-paste of the ground's, matching neither
shape). At rest near the origin the sphere stayed within that stale 3x3
footprint, so collision looked fine — but any push sending it sideways past
`|x|>1.5` or `|z|>1.5` exited the *collider's* tiny footprint while still
visually inside the real 20x20 floor, so it fell through empty
collision-less space from then on. Fixed by matching half-extents to the
actual current scales (half-extent = scale x 0.5, since the cube mesh spans
+-0.5): `groundCollider` -> `(10, 0.15, 10)`, `wallCollider` -> `(7.5, 5,
0.15)`. Verified post-fix: pushed the sphere and tracked it over ~4 seconds
of screenshots — it stays visible near the ground/sky horizon line as it
flies away and shrinks with distance, consistent with still resting near
ground height rather than sinking through (a real fall-through would drop
it far below y=0 almost immediately, and the now-large floor would occlude
it from view within a frame or two).

---

**Prompt:**
> you can update the add force function to actually use the contact point

**Result:** Noticed and cleaned up a leftover from the user's own edits
first: `RigidBody.h` had both a dead public `mass` field and a private
`m_mass` set via a constructor parameter the user had added — `addForce`
already used `m_mass`, so changing the public `mass` field did nothing.
Removed the dead public field. Then implemented torque:
- `RigidBody` — added `angularVelocity` (degrees/sec, public) and
  `momentOfInertia` (simplified single-scalar resistance to rotation, not
  a full inertia tensor, default `1.0`). `addForce` now also computes an
  angular impulse from the lever arm (`contactPoint - position`) crossed
  with `force`, divided by `momentOfInertia`, added to `angularVelocity`.
  `step()` now integrates `GameObject::rotation += angularVelocity *
  deltaSeconds` alongside the existing position integration.

Verified numerically, not just visually (a plain untextured sphere can't
show rotation on screen either way): temporarily added a debug print of
`sphereObject.rotation`/`sphereBody.angularVelocity` each frame, rebuilt,
launched with stdout redirected to a log file, triggered a left-click, and
inspected the log. Confirmed `angularVelocity` jumped to `(0, 0, 11.08)`
the instant the force landed — physically correct, since the ray hits the
sphere below its center height while pushing forward, producing top-spin
around Z — and `rotation.z` climbed steadily frame by frame afterward
(0.008 deg to 3.85 deg over ~300ms), confirming the integration in
`step()` works. Reverted the debug print; `main.cpp` is back to the
user's actual content.
