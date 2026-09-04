#include "pgk/graphics/Renderer.h"

#include <glad/gl.h>

namespace pgk {

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.0f);
}

void Renderer::clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setViewport(int width, int height) const
{
    glViewport(0, 0, width, height);
}

void Renderer::setClearColor(const glm::vec3& color)
{
    m_clearColor = color;
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.0f);
}

} // namespace pgk
