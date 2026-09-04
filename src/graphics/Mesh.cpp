#include "pgk/graphics/Mesh.h"

#include <cstddef>
#include <utility>

namespace pgk {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_vertexCount(static_cast<GLsizei>(vertices.size()))
    , m_indexCount(static_cast<GLsizei>(indices.size()))
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(),
        GL_STATIC_DRAW);

    if (!indices.empty()) {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)),
            indices.data(),
            GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

Mesh::~Mesh()
{
    release();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other) {
        release();

        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::draw() const
{
    glBindVertexArray(m_vao);
    if (m_indexCount > 0) {
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
    glBindVertexArray(0);
}

void Mesh::release()
{
    if (m_ebo) {
        glDeleteBuffers(1, &m_ebo);
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
    }
}

} // namespace pgk
