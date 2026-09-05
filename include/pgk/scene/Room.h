#pragma once

#include <vector>

#include "pgk/scene/GameObject.h"

namespace pgk {

// Builds the six walls (floor, ceiling, and 4 side walls) of a hollow room
// as static, box-collided GameObjects — data only, ready to be handed to a
// Scene (typically combined with whatever other objects go in the room).
// `size` is the room's interior (width, height, depth); the room is
// centered on the X/Z origin with its floor surface at y=0.
// `cubeMesh` must stay alive at least as long as the returned GameObjects
// (and whatever Scene ends up owning them) — same non-owning-reference
// convention GameObject already uses for its mesh.
std::vector<GameObject> buildRoom(Mesh& cubeMesh, glm::vec3 size, glm::vec3 wallColor, float wallThickness = 0.3f);

} // namespace pgk
