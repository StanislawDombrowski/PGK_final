#include "pgk/scene/Scene.h"

#include <utility>

namespace pgk {

Scene::Scene(std::vector<GameObject> objects)
{
    m_objects.reserve(objects.size());

    for (GameObject& object : objects) {
        auto ownedObject = std::make_unique<GameObject>(std::move(object));
        GameObject& gameObject = *ownedObject;
        m_objects.push_back(std::move(ownedObject));

        if (gameObject.colliderShape == ColliderShape::None) {
            continue;
        }

        std::unique_ptr<Collider> collider;
        switch (gameObject.colliderShape) {
        case ColliderShape::Box:
            collider = std::make_unique<BoxCollider>(gameObject, gameObject.colliderHalfExtents, gameObject.colliderCenterOffset);
            break;
        case ColliderShape::Sphere:
            collider = std::make_unique<SphereCollider>(gameObject, gameObject.colliderRadius);
            break;
        case ColliderShape::Cone:
            collider = std::make_unique<ConeCollider>(gameObject, gameObject.colliderRadius, gameObject.colliderHeight);
            break;
        case ColliderShape::None:
            break;
        }

        Collider& colliderRef = *collider;
        m_colliders.push_back(std::move(collider));

        if (gameObject.isDynamic) {
            auto rigidBody = std::make_unique<RigidBody>(gameObject, colliderRef, gameObject.mass);
            rigidBody->useGravity = gameObject.useGravity;
            rigidBody->restitution = gameObject.restitution;
            rigidBody->pushable = gameObject.canBePushed;

            RigidBody& bodyRef = *rigidBody;
            m_rigidBodies.push_back(std::move(rigidBody));
            m_physicsWorld.addBody(bodyRef);
        } else if (auto* boxCollider = dynamic_cast<BoxCollider*>(&colliderRef)) {
            m_physicsWorld.addStaticCollider(*boxCollider);
        }
        // Static Sphere/Cone colliders aren't registered anywhere yet —
        // PhysicsWorld's static obstacle list only supports boxes, and
        // nothing in this project currently needs a static sphere/cone.
    }
}

void Scene::update(float deltaSeconds)
{
    m_physicsWorld.step(deltaSeconds);
}

void Scene::draw(Shader& shader)
{
    for (auto& object : m_objects) {
        object->draw(shader);
    }
}

} // namespace pgk
