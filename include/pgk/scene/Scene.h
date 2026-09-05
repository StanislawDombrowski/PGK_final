#pragma once

#include <memory>
#include <vector>

#include "pgk/physics/Collider.h"
#include "pgk/physics/PhysicsWorld.h"
#include "pgk/physics/RigidBody.h"
#include "pgk/scene/GameObject.h"

namespace pgk {

class Shader;

// Owns every GameObject handed to it (each stored behind a unique_ptr for a
// stable address, since Collider/RigidBody hold references into it) plus
// whichever Collider/RigidBody each object's flags call for, wiring them
// all into an internally-owned PhysicsWorld.
class Scene {
public:
    explicit Scene(std::vector<GameObject> objects);

    void update(float deltaSeconds);
    void draw(Shader& shader);

    PhysicsWorld& physicsWorld() { return m_physicsWorld; }

private:
    std::vector<std::unique_ptr<GameObject>> m_objects;
    std::vector<std::unique_ptr<Collider>> m_colliders;
    std::vector<std::unique_ptr<RigidBody>> m_rigidBodies;
    PhysicsWorld m_physicsWorld;
};

} // namespace pgk
