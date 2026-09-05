#include "pgk/lighting/PointLight.h"

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

        void PointLight::uploadTo(Shader& shader){
            shader.use();

            shader.setVec3("pointLight.position", position);
            shader.setVec3("pointLight.color", color);

            shader.setFloat("pointLight.ambientStrength", ambientStrength);
            shader.setFloat("pointLight.diffuseStrength", diffuseStrength);
            shader.setFloat("pointLight.specularStrength", specularStrength);

            shader.setFloat("pointLight.constant", constant);
            shader.setFloat("pointLight.linear", linear);
            shader.setFloat("pointLight.quadratic", quadratic);
        }
};
