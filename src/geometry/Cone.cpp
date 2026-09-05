#include "pgk/geometry/Cone.h"

namespace pgk{
    pgk::Mesh buildConeMesh(glm::uint32_t segments){
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float radius = 1.0f;
        float height = 1.0f;

        // Pre-calculate angle increment
        float angleStep = (2.0f * glm::pi<float>()) / static_cast<float>(segments);

        // --- 1. GENERATE SIDE GEOMETRY ---
        // Calculate side normal tilt factor
        float slantHeight = std::sqrt(radius * radius + height * height);
        float cosAlpha = radius / slantHeight;
        float sinAlpha = height / slantHeight;

        // Base loop for side vertices
        for (uint32_t i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            float c = std::cos(angle);
            float s = std::sin(angle);

            // Position on the circular base ring
            glm::vec3 basePos(radius * c, 0.0f, radius * s);

            // Exact normal vector pointing outward perpendicular to the flank
            glm::vec3 sideNormal(c * sinAlpha, cosAlpha, s * sinAlpha);

            // U texture coordinate wraps from 0 to 1 around the cone
            float u = static_cast<float>(i) / segments;

            // Add the apex vertex for this segment (each segment needs its own apex for distinct normals)
            Vertex apexVert;
            apexVert.position  = glm::vec3(0.0f, height, 0.0f);
            apexVert.normal    = sideNormal;
            apexVert.texCoords = glm::vec2(u, 1.0f);
            vertices.push_back(apexVert);

            // Add the base vertex for this segment
            Vertex baseVert;
            baseVert.position  = basePos;
            baseVert.normal    = sideNormal;
            baseVert.texCoords = glm::vec2(u, 0.0f);
            vertices.push_back(baseVert);
        }

        // Build side indices (Triangle Strip pattern using individual triangles)
        for (uint32_t i = 0; i < segments; ++i) {
            uint32_t currentApex = i * 2;
            uint32_t currentBase = i * 2 + 1;
            uint32_t nextBase    = (i + 1) * 2 + 1;

            indices.push_back(currentApex);
            indices.push_back(currentBase);
            indices.push_back(nextBase);
        }

        // --- 2. GENERATE BOTTOM CAP GEOMETRY ---
        uint32_t capCenterIndex = static_cast<uint32_t>(vertices.size());

        // Center point of the bottom cap
        Vertex centerVert;
        centerVert.position  = glm::vec3(0.0f, 0.0f, 0.0f);
        centerVert.normal    = glm::vec3(0.0f, -1.0f, 0.0f); // Points straight down
        centerVert.texCoords = glm::vec2(0.5f, 0.5f);
        vertices.push_back(centerVert);

        uint32_t capRingStartIndex = static_cast<uint32_t>(vertices.size());

        // Bottom cap ring vertices
        for (uint32_t i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            float c = std::cos(angle);
            float s = std::sin(angle);

            Vertex capVert;
            capVert.position  = glm::vec3(radius * c, 0.0f, radius * s);
            capVert.normal    = glm::vec3(0.0f, -1.0f, 0.0f);
            // Map circle coordinates to a flat [0,1] texture plane
            capVert.texCoords = glm::vec2(c * 0.5f + 0.5f, s * 0.5f + 0.5f);
            vertices.push_back(capVert);
        }

        // Build bottom cap indices (Clockwise/Counter-clockwise standard winding order)
        for (uint32_t i = 0; i < segments; ++i) {
            indices.push_back(capCenterIndex);
            indices.push_back(capRingStartIndex + i + 1);
            indices.push_back(capRingStartIndex + i);
        }

        return Mesh(vertices, indices);

    }
};
