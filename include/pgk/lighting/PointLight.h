#pragma once

#include "glm/fwd.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "pgk/graphics/Shader.h"

namespace pgk{
    class PointLight{
    public:
        glm::vec3 position, color;
        glm::float32 ambientStrength, diffuseStrength, specularStrength;

        // Distance fall of values (Attentuation)
        glm::float32 constant, linear, quadratic;

        PointLight(glm::vec3 position, glm::vec3 color,
            glm::float32 ambientStrength, glm::float32 diffuseStrength, glm::float32 specularStrength,
            glm::float32 constant, glm::float32 linear, glm::float32 quadratic);
        ~PointLight() = default;

        // `index` selects which element of the shader's pointLights[] array
        // this light writes to (must be < MAX_POINT_LIGHTS in lit.frag).
        void uploadTo(Shader& shader, int index);
    };
};
