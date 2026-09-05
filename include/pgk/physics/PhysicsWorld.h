#pragma once

#include <vector>

#include "pgk/physics/Collider.h"
#include "pgk/physics/RigidBody.h"

namespace pgk {

// Owns no bodies/colliders (they're owned wherever they're constructed, e.g.
// main.cpp) — just steps each registered RigidBody and resolves it against
// every registered static collider. Static obstacles (room walls/floor) are
// always boxes, so RigidBody's Collider (any shape) only needs to know how
// to test itself against a Bounds, not against arbitrary other shapes.
class PhysicsWorld {
public:
    void addBody(RigidBody& body);
    void addStaticCollider(BoxCollider& collider);

    void step(float deltaSeconds);

    const std::vector<RigidBody*>& bodies() const { return m_bodies; }

private:
    std::vector<RigidBody*> m_bodies;
    std::vector<BoxCollider*> m_staticColliders;
};

} // namespace pgk
