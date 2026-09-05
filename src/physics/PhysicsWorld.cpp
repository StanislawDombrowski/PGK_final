#include "pgk/physics/PhysicsWorld.h"

namespace pgk {

void PhysicsWorld::addBody(RigidBody& body)
{
    m_bodies.push_back(&body);
}

void PhysicsWorld::addStaticCollider(BoxCollider& collider)
{
    m_staticColliders.push_back(&collider);
}

void PhysicsWorld::step(float deltaSeconds)
{
    for (RigidBody* body : m_bodies) {
        body->step(deltaSeconds);

        for (BoxCollider* box : m_staticColliders) {
            glm::vec3 penetration;
            if (body->collider().overlapsBox(box->bounds(), penetration)) {
                body->resolveCollision(penetration);
            }
        }
    }

    // Dynamic-vs-dynamic: every unique pair, checked once each.
    for (size_t i = 0; i < m_bodies.size(); ++i) {
        for (size_t j = i + 1; j < m_bodies.size(); ++j) {
            glm::vec3 penetration; // pushes bodies[i] away from bodies[j]
            if (overlapsShapes(m_bodies[i]->collider(), m_bodies[j]->collider(), penetration)) {
                m_bodies[i]->resolveCollision(penetration * 0.5f);
                m_bodies[j]->resolveCollision(penetration * -0.5f);
            }
        }
    }
}

} // namespace pgk
