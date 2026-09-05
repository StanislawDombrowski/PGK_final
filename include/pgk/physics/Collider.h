#pragma once

#include <glm/glm.hpp>

#include "pgk/scene/GameObject.h"

namespace pgk {

struct Bounds {
    glm::vec3 min;
    glm::vec3 max;
};

// Abstract collision shape. Every concrete shape knows how to test itself
// against an axis-aligned box (the only shape static obstacles — the
// room's walls/floor — ever are) and against a ray (for click-to-push).
// None of these shapes rotate with their GameObject.
class Collider {
public:
    virtual ~Collider() = default;

    Collider(const Collider&) = delete;
    Collider& operator=(const Collider&) = delete;

    // If this shape overlaps `box`, returns true and sets outPenetration to
    // the world-space vector (direction * depth) that would push this
    // shape's owner out of the box along the shortest path.
    virtual bool overlapsBox(const Bounds& box, glm::vec3& outPenetration) const = 0;

    // If the ray hits this shape in front of the origin, returns true and
    // sets tHit to the distance along the ray (direction assumed normalized).
    virtual bool raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const = 0;

    // Conservative bounding sphere — used as the fallback overlap test in
    // overlapsShapes() for any pair of concrete shape types that doesn't
    // have a dedicated exact routine there.
    virtual glm::vec3 boundingSphereCenter() const = 0;
    virtual float boundingSphereRadius() const = 0;

    GameObject& gameObject() const { return m_gameObject; }

protected:
    explicit Collider(GameObject& gameObject) : m_gameObject(gameObject) {}

    GameObject& m_gameObject;
};

// Axis-aligned box. Fits meshes already centered on their own local origin
// (the cube); `centerOffset` handles meshes that aren't (e.g. a base-pivoted
// cone, though ConeCollider below is the exact fit for that shape now).
class BoxCollider : public Collider {
public:
    BoxCollider(GameObject& gameObject, glm::vec3 halfExtents, glm::vec3 centerOffset = glm::vec3(0.0f));

    glm::vec3 min() const;
    glm::vec3 max() const;
    Bounds bounds() const;

    bool overlapsBox(const Bounds& box, glm::vec3& outPenetration) const override;
    bool raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const override;
    glm::vec3 boundingSphereCenter() const override;
    float boundingSphereRadius() const override;

private:
    glm::vec3 m_halfExtents;
    glm::vec3 m_centerOffset;
};

// True sphere, exact fit for the sphere mesh (radius from its own origin).
class SphereCollider : public Collider {
public:
    SphereCollider(GameObject& gameObject, float radius);

    glm::vec3 center() const { return m_gameObject.position; }
    float radius() const { return m_radius; }

    bool overlapsBox(const Bounds& box, glm::vec3& outPenetration) const override;
    bool raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const override;
    glm::vec3 boundingSphereCenter() const override { return center(); }
    float boundingSphereRadius() const override { return m_radius; }

private:
    float m_radius;
};

// Exact tapered cone, pivoted at its base (matching buildConeMesh's local
// origin: base at gameObject.position, apex straight up at position.y + height).
class ConeCollider : public Collider {
public:
    ConeCollider(GameObject& gameObject, float baseRadius, float height);

    float baseRadius() const { return m_baseRadius; }
    float height() const { return m_height; }

    bool overlapsBox(const Bounds& box, glm::vec3& outPenetration) const override;
    bool raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const override;
    glm::vec3 boundingSphereCenter() const override;
    float boundingSphereRadius() const override;

private:
    float m_baseRadius;
    float m_height;
};

// Tests two dynamic shapes against each other (used for body-vs-body
// collision, unlike overlapsBox which is only ever against a static box).
// On overlap, outPenetration is the vector that would push `a` away from
// `b`. Has exact routines for sphere-sphere and sphere-cone; any other
// combination falls back to a conservative bounding-sphere check so shapes
// never silently pass through each other even without a dedicated test.
bool overlapsShapes(const Collider& a, const Collider& b, glm::vec3& outPenetration);

} // namespace pgk
