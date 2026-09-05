#include "pgk/physics/RigidBody.h"

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

void RigidBody::resolveCollision(const glm::vec3& penetration)
{
    m_gameObject.position += penetration;

    const float penetrationLength = glm::length(penetration);
    if (penetrationLength > 1e-8f) {
        const glm::vec3 normal = penetration / penetrationLength;
        const float velocityAlongNormal = glm::dot(velocity, normal);
        // Only reflect if still moving into the surface — otherwise it's
        // already separating and reflecting again would add energy.
        if (velocityAlongNormal < 0.0f) {
            velocity -= (1.0f + restitution) * velocityAlongNormal * normal;
        }
    }

    m_gameObject.updateModelMatrix();
}

} // namespace pgk
