#pragma once

#include <glm/glm.hpp>

#include "pgk/scene/GameObject.h"

namespace pgk {

// A pure axis-aligned bounding box — does NOT rotate with its GameObject
// (that's the "axis-aligned" trade-off). min()/max() are computed live from
// the GameObject's current position each call, so it always tracks the
// object without needing to be refreshed manually.
struct Bounds {
    glm::vec3 min;
    glm::vec3 max;
};

class Collider {
public:
    Collider(GameObject& gameObject, glm::vec3 halfExtents);

    glm::vec3 min() const;
    glm::vec3 max() const;
    Bounds bounds() const;

    GameObject& gameObject() const { return m_gameObject; }

private:
    GameObject& m_gameObject;
    glm::vec3 m_halfExtents;
};

bool intersects(const Collider& a, const Collider& b);

} // namespace pgk
