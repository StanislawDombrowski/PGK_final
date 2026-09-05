#pragma once

#include <vector>

#include "pgk/physics/Collider.h"
#include "pgk/physics/RigidBody.h"

namespace pgk {

// Owns no bodies/colliders (they're owned wherever they're constructed, e.g.
// main.cpp) — just steps each registered RigidBody and resolves it against
// every registered static Collider.
class PhysicsWorld {
public:
    void addBody(RigidBody& body);
    void addStaticCollider(Collider& collider);

    void step(float deltaSeconds);

    const std::vector<RigidBody*>& bodies() const { return m_bodies; }

private:
    std::vector<RigidBody*> m_bodies;
    std::vector<Collider*> m_staticColliders;
};

} // namespace pgk
