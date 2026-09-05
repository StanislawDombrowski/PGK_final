#include "pgk/physics/Collider.h"

namespace pgk {

Collider::Collider(GameObject& gameObject, glm::vec3 halfExtents)
    : m_gameObject(gameObject)
    , m_halfExtents(halfExtents)
{
}

glm::vec3 Collider::min() const
{
    return m_gameObject.position - m_halfExtents;
}

glm::vec3 Collider::max() const
{
    return m_gameObject.position + m_halfExtents;
}

Bounds Collider::bounds() const
{
    return {min(), max()};
}

bool intersects(const Collider& a, const Collider& b)
{
    const glm::vec3 aMin = a.min();
    const glm::vec3 aMax = a.max();
    const glm::vec3 bMin = b.min();
    const glm::vec3 bMax = b.max();

    return aMin.x <= bMax.x && aMax.x >= bMin.x
        && aMin.y <= bMax.y && aMax.y >= bMin.y
        && aMin.z <= bMax.z && aMax.z >= bMin.z;
}

} // namespace pgk
