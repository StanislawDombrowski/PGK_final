#pragma once

#include "glm/fwd.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>


#include "pgk/graphics/Mesh.h"
#include "pgk/graphics/Shader.h"

namespace pgk{
    class GameObject{
        public:
            glm::vec3 position, rotation, scale;
            glm::vec3 color;

            pgk::Mesh* mesh;
            glm::mat4 modelMatrix;

            GameObject(pgk::Mesh& mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 material);
            ~GameObject() = default;

            void draw(pgk::Shader& shader);
    };
}
