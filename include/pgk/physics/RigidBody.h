#pragma once

#include <glm/glm.hpp>

#include "pgk/physics/Collider.h"
#include "pgk/scene/GameObject.h"

namespace pgk {

// Drives one GameObject's position/rotation via simple gravity + Euler
// integration, and can resolve an overlap against a static Collider by
// pushing itself out along the axis of least penetration and bouncing its
// velocity there.
class RigidBody {
public:
    RigidBody(GameObject& gameObject, Collider& collider, float mass = 1.0f);

    // Applies `force` as an instantaneous impulse to velocity via F=ma, and
    // — since contactPoint generally isn't at the object's center — also
    // applies an angular impulse from the lever arm (contactPoint minus the
    // object's position) crossed with force, scaled by momentOfInertia.
    // This is a simplified single-scalar moment of inertia, not a full
    // inertia tensor, and rotation integrates via Euler angles like
    // GameObject::rotation already does elsewhere in the project.
    void addForce(const glm::vec3& force, const glm::vec3& contactPoint);

    void step(float deltaSeconds);
    void resolveCollision(const Collider& other);

    glm::vec3 velocity{0.0f};
    glm::vec3 angularVelocity{0.0f}; // degrees per second, per axis
    bool useGravity = true;
    float restitution = 0.4f;      // bounciness: 0 = stops dead, 1 = perfectly elastic
    float momentOfInertia = 1.0f;  // resistance to rotation: higher = spins less per unit torque

    Collider& collider() const { return m_collider; }

private:
    GameObject& m_gameObject;
    Collider& m_collider;
    float m_mass;
};

} // namespace pgk
