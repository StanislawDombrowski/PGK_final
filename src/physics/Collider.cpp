#include "pgk/physics/Collider.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace pgk {

// ---------------------------------------------------------------- BoxCollider

BoxCollider::BoxCollider(GameObject& gameObject, glm::vec3 halfExtents, glm::vec3 centerOffset)
    : Collider(gameObject)
    , m_halfExtents(halfExtents)
    , m_centerOffset(centerOffset)
{
}

glm::vec3 BoxCollider::min() const
{
    return m_gameObject.position + m_centerOffset - m_halfExtents;
}

glm::vec3 BoxCollider::max() const
{
    return m_gameObject.position + m_centerOffset + m_halfExtents;
}

Bounds BoxCollider::bounds() const
{
    return {min(), max()};
}

bool BoxCollider::overlapsBox(const Bounds& box, glm::vec3& outPenetration) const
{
    const glm::vec3 aMin = min();
    const glm::vec3 aMax = max();

    const float overlapX = std::min(aMax.x, box.max.x) - std::max(aMin.x, box.min.x);
    const float overlapY = std::min(aMax.y, box.max.y) - std::max(aMin.y, box.min.y);
    const float overlapZ = std::min(aMax.z, box.max.z) - std::max(aMin.z, box.min.z);

    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) {
        return false;
    }

    const glm::vec3 center = (aMin + aMax) * 0.5f;
    const glm::vec3 otherCenter = (box.min + box.max) * 0.5f;

    if (overlapX <= overlapY && overlapX <= overlapZ) {
        outPenetration = glm::vec3((center.x < otherCenter.x ? -1.0f : 1.0f) * overlapX, 0.0f, 0.0f);
    } else if (overlapY <= overlapX && overlapY <= overlapZ) {
        outPenetration = glm::vec3(0.0f, (center.y < otherCenter.y ? -1.0f : 1.0f) * overlapY, 0.0f);
    } else {
        outPenetration = glm::vec3(0.0f, 0.0f, (center.z < otherCenter.z ? -1.0f : 1.0f) * overlapZ);
    }

    return true;
}

glm::vec3 BoxCollider::boundingSphereCenter() const
{
    return (min() + max()) * 0.5f;
}

float BoxCollider::boundingSphereRadius() const
{
    return glm::length(max() - min()) * 0.5f;
}

bool BoxCollider::raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const
{
    const glm::vec3 boxMin = min();
    const glm::vec3 boxMax = max();

    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    for (int axis = 0; axis < 3; ++axis) {
        const float invDir = 1.0f / direction[axis];
        float t0 = (boxMin[axis] - origin[axis]) * invDir;
        float t1 = (boxMax[axis] - origin[axis]) * invDir;
        if (invDir < 0.0f) {
            std::swap(t0, t1);
        }
        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);
        if (tMax <= tMin) {
            return false;
        }
    }

    tHit = tMin;
    return true;
}

// ------------------------------------------------------------- SphereCollider

SphereCollider::SphereCollider(GameObject& gameObject, float radius)
    : Collider(gameObject)
    , m_radius(radius)
{
}

bool SphereCollider::overlapsBox(const Bounds& box, glm::vec3& outPenetration) const
{
    const glm::vec3 c = center();
    const glm::vec3 closest = glm::clamp(c, box.min, box.max);
    const glm::vec3 delta = c - closest;
    const float distSq = glm::dot(delta, delta);

    if (distSq >= m_radius * m_radius) {
        return false;
    }

    if (distSq > 1e-8f) {
        const float dist = std::sqrt(distSq);
        outPenetration = (delta / dist) * (m_radius - dist);
        return true;
    }

    // Center is inside the box: push out along whichever face is nearest.
    const float faceDistances[6] = {
        c.x - box.min.x, box.max.x - c.x,
        c.y - box.min.y, box.max.y - c.y,
        c.z - box.min.z, box.max.z - c.z,
    };
    const glm::vec3 pushDirections[6] = {
        glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
    };

    int nearestFace = 0;
    for (int i = 1; i < 6; ++i) {
        if (faceDistances[i] < faceDistances[nearestFace]) {
            nearestFace = i;
        }
    }

    outPenetration = pushDirections[nearestFace] * (faceDistances[nearestFace] + m_radius);
    return true;
}

bool SphereCollider::raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const
{
    const glm::vec3 oc = origin - center();
    const float b = glm::dot(oc, direction);
    const float c = glm::dot(oc, oc) - m_radius * m_radius;
    const float discriminant = b * b - c;

    if (discriminant < 0.0f) {
        return false;
    }

    const float sqrtDisc = std::sqrt(discriminant);
    float t = -b - sqrtDisc;
    if (t < 0.0f) {
        t = -b + sqrtDisc;
    }
    if (t < 0.0f) {
        return false;
    }

    tHit = t;
    return true;
}

// --------------------------------------------------------------- ConeCollider

ConeCollider::ConeCollider(GameObject& gameObject, float baseRadius, float height)
    : Collider(gameObject)
    , m_baseRadius(baseRadius)
    , m_height(height)
{
}

bool ConeCollider::overlapsBox(const Bounds& box, glm::vec3& outPenetration) const
{
    const glm::vec3 base = m_gameObject.position;
    const float coneBottom = base.y;
    const float coneTop = base.y + m_height;

    const float yOverlapMin = std::max(coneBottom, box.min.y);
    const float yOverlapMax = std::min(coneTop, box.max.y);
    if (yOverlapMin >= yOverlapMax) {
        return false;
    }

    // The cone narrows toward its apex, so the bottom of the overlapping
    // height range has the largest (most permissive) cross-section radius.
    const float heightAboveBase = yOverlapMin - coneBottom;
    const float sliceRadius = m_baseRadius * (1.0f - heightAboveBase / m_height);
    if (sliceRadius <= 0.0f) {
        return false;
    }

    const glm::vec2 center2D(base.x, base.z);
    const glm::vec2 closest2D = glm::clamp(center2D, glm::vec2(box.min.x, box.min.z), glm::vec2(box.max.x, box.max.z));
    const glm::vec2 delta2D = center2D - closest2D;
    const float dist2D = glm::length(delta2D);

    if (dist2D >= sliceRadius) {
        return false;
    }

    const float yPenetration = yOverlapMax - yOverlapMin;
    const float radialPenetration = sliceRadius - dist2D;

    if (yPenetration < radialPenetration) {
        const float coneMid = base.y + m_height * 0.5f;
        const float boxMid = (box.min.y + box.max.y) * 0.5f;
        const float direction = (coneMid < boxMid) ? -1.0f : 1.0f;
        outPenetration = glm::vec3(0.0f, direction * yPenetration, 0.0f);
    } else if (dist2D > 1e-6f) {
        const glm::vec2 pushDir2D = delta2D / dist2D;
        outPenetration = glm::vec3(pushDir2D.x, 0.0f, pushDir2D.y) * radialPenetration;
    } else {
        outPenetration = glm::vec3(0.0f, radialPenetration, 0.0f);
    }

    return true;
}

bool ConeCollider::raycast(const glm::vec3& origin, const glm::vec3& direction, float& tHit) const
{
    const glm::vec3 base = m_gameObject.position;
    const glm::vec3 o = origin - base; // ray origin in cone-local space (base at local origin)

    const float k = m_baseRadius / m_height; // slope: radius shrinks by k per unit height
    float bestT = -1.0f;

    // --- Lateral (tapered) surface: x^2 + z^2 = k^2 * (height - y)^2 ---
    {
        const float H = m_height - o.y;
        const float A = direction.x * direction.x + direction.z * direction.z - k * k * direction.y * direction.y;
        const float B = 2.0f * (o.x * direction.x + o.z * direction.z + k * k * H * direction.y);
        const float C = o.x * o.x + o.z * o.z - k * k * H * H;

        if (std::abs(A) > 1e-8f) {
            const float discriminant = B * B - 4.0f * A * C;
            if (discriminant >= 0.0f) {
                const float sqrtDisc = std::sqrt(discriminant);
                const float candidates[2] = {(-B - sqrtDisc) / (2.0f * A), (-B + sqrtDisc) / (2.0f * A)};

                for (float t : candidates) {
                    if (t < 0.0f) {
                        continue;
                    }
                    const float y = o.y + t * direction.y;
                    if (y < 0.0f || y > m_height) {
                        continue;
                    }
                    if (bestT < 0.0f || t < bestT) {
                        bestT = t;
                    }
                }
            }
        }
    }

    // --- Base cap: flat disk at local y=0, radius baseRadius ---
    if (std::abs(direction.y) > 1e-8f) {
        const float t = -o.y / direction.y;
        if (t >= 0.0f && (bestT < 0.0f || t < bestT)) {
            const float x = o.x + t * direction.x;
            const float z = o.z + t * direction.z;
            if (x * x + z * z <= m_baseRadius * m_baseRadius) {
                bestT = t;
            }
        }
    }

    if (bestT < 0.0f) {
        return false;
    }

    tHit = bestT;
    return true;
}

glm::vec3 ConeCollider::boundingSphereCenter() const
{
    return m_gameObject.position + glm::vec3(0.0f, m_height * 0.5f, 0.0f);
}

float ConeCollider::boundingSphereRadius() const
{
    const float halfHeight = m_height * 0.5f;
    return std::sqrt(m_baseRadius * m_baseRadius + halfHeight * halfHeight);
}

// ------------------------------------------------------- body-vs-body tests

namespace {

glm::vec2 closestPointOnSegment2D(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
{
    const glm::vec2 ab = b - a;
    const float abLenSq = glm::dot(ab, ab);
    if (abLenSq < 1e-12f) {
        return a;
    }
    const float t = glm::clamp(glm::dot(p - a, ab) / abLenSq, 0.0f, 1.0f);
    return a + t * ab;
}

bool sphereVsSphere(const SphereCollider& a, const SphereCollider& b, glm::vec3& outPenetration)
{
    const glm::vec3 delta = a.center() - b.center();
    const float dist = glm::length(delta);
    const float radiusSum = a.radius() + b.radius();

    if (dist >= radiusSum) {
        return false;
    }

    const glm::vec3 normal = (dist > 1e-6f) ? (delta / dist) : glm::vec3(0.0f, 1.0f, 0.0f);
    outPenetration = normal * (radiusSum - dist);
    return true;
}

// Penetration that would push `sphere` out of `cone`. Reduces to a 2D
// point-vs-triangle-edges problem in the cone's meridian (radius, height)
// half-plane, exploiting its rotational symmetry — the same idea as
// ConeCollider::overlapsBox's height-slice trick, generalized to a proper
// closest-point query since the sphere isn't axis-aligned like a box.
bool sphereVsCone(const SphereCollider& sphere, const ConeCollider& cone, glm::vec3& outPenetration)
{
    const glm::vec3 base = cone.gameObject().position;
    const glm::vec3 p = sphere.center() - base;

    const float r = std::sqrt(p.x * p.x + p.z * p.z);
    const float y = p.y;
    const float R = cone.baseRadius();
    const float H = cone.height();

    const glm::vec2 q(r, y);
    const glm::vec2 baseClosest = closestPointOnSegment2D(q, glm::vec2(0.0f, 0.0f), glm::vec2(R, 0.0f));
    const glm::vec2 slantClosest = closestPointOnSegment2D(q, glm::vec2(R, 0.0f), glm::vec2(0.0f, H));

    const float distBase = glm::length(q - baseClosest);
    const float distSlant = glm::length(q - slantClosest);
    const bool useBase = distBase <= distSlant;
    const glm::vec2 closest2D = useBase ? baseClosest : slantClosest;
    const float dist2D = useBase ? distBase : distSlant;

    const bool inside = (y >= 0.0f && y <= H) && (r <= R * (1.0f - y / H));
    const float depth = inside ? (dist2D + sphere.radius()) : (sphere.radius() - dist2D);
    if (depth <= 0.0f) {
        return false;
    }

    // Direction from the nearest boundary point through the query point
    // (and, for points inside, on out the other side) is the shortest exit.
    glm::vec2 dir2D = q - closest2D;
    dir2D = (glm::length(dir2D) > 1e-6f) ? glm::normalize(dir2D) : glm::vec2(0.0f, 1.0f);

    const glm::vec2 radialXZ = (r > 1e-6f) ? glm::vec2(p.x, p.z) / r : glm::vec2(1.0f, 0.0f);

    outPenetration = glm::vec3(radialXZ.x * dir2D.x, dir2D.y, radialXZ.y * dir2D.x) * depth;
    return true;
}

} // namespace

bool overlapsShapes(const Collider& a, const Collider& b, glm::vec3& outPenetration)
{
    const auto* sphereA = dynamic_cast<const SphereCollider*>(&a);
    const auto* sphereB = dynamic_cast<const SphereCollider*>(&b);
    const auto* coneA = dynamic_cast<const ConeCollider*>(&a);
    const auto* coneB = dynamic_cast<const ConeCollider*>(&b);
    const auto* boxA = dynamic_cast<const BoxCollider*>(&a);
    const auto* boxB = dynamic_cast<const BoxCollider*>(&b);

    if (sphereA && sphereB) {
        return sphereVsSphere(*sphereA, *sphereB, outPenetration);
    }
    if (sphereA && coneB) {
        return sphereVsCone(*sphereA, *coneB, outPenetration);
    }
    if (coneA && sphereB) {
        const bool hit = sphereVsCone(*sphereB, *coneA, outPenetration);
        if (hit) {
            outPenetration = -outPenetration;
        }
        return hit;
    }
    if (boxA && boxB) {
        return boxA->overlapsBox(boxB->bounds(), outPenetration);
    }

    // No dedicated exact test for this shape combination (e.g. two cones,
    // or a dynamic box against a sphere/cone) — fall back to a conservative
    // bounding-sphere check so objects never silently pass through each
    // other just because a pairwise routine wasn't written for them.
    const glm::vec3 delta = a.boundingSphereCenter() - b.boundingSphereCenter();
    const float dist = glm::length(delta);
    const float radiusSum = a.boundingSphereRadius() + b.boundingSphereRadius();

    if (dist >= radiusSum) {
        return false;
    }

    const glm::vec3 normal = (dist > 1e-6f) ? (delta / dist) : glm::vec3(0.0f, 1.0f, 0.0f);
    outPenetration = normal * (radiusSum - dist);
    return true;
}

} // namespace pgk
