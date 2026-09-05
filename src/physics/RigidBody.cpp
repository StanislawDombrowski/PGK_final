#include "pgk/physics/RigidBody.h"

#include <algorithm>

namespace pgk {

namespace {
constexpr float kGravity = -9.81f; // world units per second^2, along Y
}

RigidBody::RigidBody(GameObject& gameObject, Collider& collider, float mass)
    : m_gameObject(gameObject)
    , m_collider(collider)
    , m_mass(mass)
{
}

void RigidBody::step(float deltaSeconds)
{
    if (useGravity) {
        velocity.y += kGravity * deltaSeconds;
    }

    m_gameObject.position += velocity * deltaSeconds;
    m_gameObject.rotation += angularVelocity * deltaSeconds;
    m_gameObject.updateModelMatrix();
}

void RigidBody::addForce(const glm::vec3& force, const glm::vec3& contactPoint)
{
    velocity += force / m_mass;

    const glm::vec3 leverArm = contactPoint - m_gameObject.position;
    const glm::vec3 angularImpulse = glm::cross(leverArm, force);
    angularVelocity += angularImpulse / momentOfInertia;
}

void RigidBody::resolveCollision(const Collider& other)
{
    const glm::vec3 aMin = m_collider.min();
    const glm::vec3 aMax = m_collider.max();
    const glm::vec3 bMin = other.min();
    const glm::vec3 bMax = other.max();

    const float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
    const float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
    const float overlapZ = std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z);

    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) {
        return; // not actually overlapping
    }

    // Push out along whichever axis needs the smallest correction, and
    // bounce the velocity component on that same axis.
    if (overlapX <= overlapY && overlapX <= overlapZ) {
        const float direction = (m_gameObject.position.x < other.gameObject().position.x) ? -1.0f : 1.0f;
        m_gameObject.position.x += direction * overlapX;
        velocity.x = -velocity.x * restitution;
    } else if (overlapY <= overlapX && overlapY <= overlapZ) {
        const float direction = (m_gameObject.position.y < other.gameObject().position.y) ? -1.0f : 1.0f;
        m_gameObject.position.y += direction * overlapY;
        velocity.y = -velocity.y * restitution;
    } else {
        const float direction = (m_gameObject.position.z < other.gameObject().position.z) ? -1.0f : 1.0f;
        m_gameObject.position.z += direction * overlapZ;
        velocity.z = -velocity.z * restitution;
    }

    m_gameObject.updateModelMatrix();
}

} // namespace pgk
