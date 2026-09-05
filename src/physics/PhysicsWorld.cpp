#include "pgk/physics/PhysicsWorld.h"

namespace pgk {

void PhysicsWorld::addBody(RigidBody& body)
{
    m_bodies.push_back(&body);
}

void PhysicsWorld::addStaticCollider(Collider& collider)
{
    m_staticColliders.push_back(&collider);
}

void PhysicsWorld::step(float deltaSeconds)
{
    for (RigidBody* body : m_bodies) {
        body->step(deltaSeconds);

        for (Collider* staticCollider : m_staticColliders) {
            if (intersects(body->collider(), *staticCollider)) {
                body->resolveCollision(*staticCollider);
            }
        }
    }
}

} // namespace pgk
