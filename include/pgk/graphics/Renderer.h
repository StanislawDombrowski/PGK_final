#pragma once

#include <glm/glm.hpp>

namespace pgk {

class Renderer {
public:
    void init();
    void clear() const;
    void setViewport(int width, int height) const;
    void setClearColor(const glm::vec3& color);

private:
    glm::vec3 m_clearColor{0.05f, 0.07f, 0.1f};
};

} // namespace pgk
