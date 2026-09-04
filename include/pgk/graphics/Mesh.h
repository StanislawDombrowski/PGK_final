#pragma once

#include <cstdint>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace pgk {

// Common vertex layout for every mesh in the project (room walls, procedural
// primitives, ...). Matches the `layout(location = ...)` attributes expected
// by assets/shaders/*.vert.
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// Owns a VAO/VBO/(optional EBO) triple and uploads vertex/index data to the
// GPU on construction. Geometry classes (Cube, Sphere, Cone, ...) should
// build a Vertex/index list on the CPU and hand it to a Mesh.
class Mesh {
public:
    explicit Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw() const;

private:
    void release();

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLsizei m_vertexCount = 0;
    GLsizei m_indexCount = 0;
};

} // namespace pgk
