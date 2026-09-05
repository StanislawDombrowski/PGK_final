#pragma once

#include "glm/fwd.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "pgk/graphics/Mesh.h"
#include "pgk/graphics/Shader.h"

namespace pgk{

    enum class ColliderShape {
        None,
        Box,
        Sphere,
        Cone,
    };

    class GameObject{
        public:
            glm::vec3 position, rotation, scale;
            glm::vec3 color;

            pgk::Mesh* mesh;
            glm::mat4 modelMatrix;

            // Collision/physics setup, read by Scene's constructor to decide
            // what Collider/RigidBody (if any) to create for this object.
            // Defaults mean "no collision, purely visual."
            ColliderShape colliderShape = ColliderShape::None;
            glm::vec3 colliderHalfExtents{0.5f};  // Box only
            glm::vec3 colliderCenterOffset{0.0f}; // Box only
            float colliderRadius = 0.5f;          // Sphere radius, or Cone base radius
            float colliderHeight = 1.0f;          // Cone only

            // If colliderShape != None: true gets a RigidBody (falls, can be
            // pushed, collides with other dynamic objects); false registers
            // it as a static obstacle instead (must be colliderShape::Box —
            // that's all PhysicsWorld's static list currently supports).
            bool isDynamic = false;
            bool useGravity = true;
            float mass = 1.0f;
            float restitution = 0.4f;
            bool canBePushed = true; // if false, left-click push ignores this object

            GameObject(pgk::Mesh& mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 material);
            ~GameObject() = default;

            void draw(pgk::Shader& shader);

            // Recomputes modelMatrix from position/rotation/scale. Call after
            // anything (e.g. physics) mutates them.
            void updateModelMatrix();
    };
};
