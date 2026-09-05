#include "pgk/geometry/Sphere.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "pgk/graphics/Mesh.h"

namespace pgk{
    pgk::Mesh buildSphereMesh(glm::uint32_t sectorCount, glm::uint32_t stackCount){
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            float pi = glm::pi<float>();
            float radius = 1.0f;

            // 1. Generate Vertex Data
            for (uint32_t i = 0; i <= stackCount; ++i) {
                float phi = pi / 2.0f - (float)i * pi / (float)stackCount; // From pi/2 to -pi/2 (top to bottom)
                float xy = radius * std::cos(phi);
                float z = radius * std::sin(phi);

                for (uint32_t j = 0; j <= sectorCount; ++j) {
                    float theta = (float)j * 2.0f * pi / (float)sectorCount; // From 0 to 2*pi

                    Vertex vertex;

                    // Positions
                    vertex.position.x = xy * std::cos(theta);
                    vertex.position.y = xy * std::sin(theta);
                    vertex.position.z = z;

                    // Normals (normalized position for a sphere centered at origin)
                    vertex.normal = glm::normalize(vertex.position);

                    // Texture Coordinates (normalized mapping)
                    vertex.texCoords.x = (float)j / (float)sectorCount;
                    vertex.texCoords.y = (float)i / (float)stackCount;

                    vertices.push_back(vertex);
                }
            }

            // 2. Generate Index Data (Triangle list topology)
            for (uint32_t i = 0; i < stackCount; ++i) {
                uint32_t k1 = i * (sectorCount + 1);     // Beginning of current stack row
                uint32_t k2 = k1 + sectorCount + 1;      // Beginning of next stack row

                for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                    // Triangle 1: k1 -> k2 -> k1+1
                    if (i != 0) { // Skip top pole degenerated triangles (optional optimization, but keeps structure clean)
                        indices.push_back(k1);
                        indices.push_back(k2);
                        indices.push_back(k1 + 1);
                    }

                    // Triangle 2: k1+1 -> k2 -> k2+1
                    if (i != (stackCount - 1)) { // Skip bottom pole degenerated triangles
                        indices.push_back(k1 + 1);
                        indices.push_back(k2);
                        indices.push_back(k2 + 1);
                    }
                }
            }
            pgk::Mesh sphere(vertices, indices);
            return sphere;
    }
};
