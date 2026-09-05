#include "pgk/lighting/PointLight.h"

#include <string>

namespace pgk{
    PointLight::PointLight(glm::vec3 position, glm::vec3 color,
        glm::float32 ambientStrength, glm::float32 diffuseStrength, glm::float32 specularStrength,
        glm::float32 constant, glm::float32 linear, glm::float32 quadratic){

            PointLight::position = position;
            PointLight::color = color;

            PointLight::ambientStrength = ambientStrength;
            PointLight::diffuseStrength = diffuseStrength;
            PointLight::specularStrength = specularStrength;

            PointLight::constant = constant;
            PointLight::linear = linear;
            PointLight::quadratic = quadratic;
        }

        void PointLight::uploadTo(Shader& shader, int index){
            const std::string prefix = "pointLights[" + std::to_string(index) + "].";

            shader.use();

            shader.setVec3(prefix + "position", position);
            shader.setVec3(prefix + "color", color);

            shader.setFloat(prefix + "ambientStrength", ambientStrength);
            shader.setFloat(prefix + "diffuseStrength", diffuseStrength);
            shader.setFloat(prefix + "specularStrength", specularStrength);

            shader.setFloat(prefix + "constant", constant);
            shader.setFloat(prefix + "linear", linear);
            shader.setFloat(prefix + "quadratic", quadratic);
        }
};
