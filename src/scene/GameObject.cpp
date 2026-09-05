#include "pgk/scene/GameObject.h"
#include "glm/ext/matrix_transform.hpp"

namespace pgk{

    GameObject::GameObject(pgk::Mesh& mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 material){
        GameObject::mesh = &mesh;

        GameObject::position = position;
        GameObject::rotation = rotation;
        GameObject::scale = scale;
        GameObject::color = material;

        GameObject::modelMatrix = glm::mat4(1.f);
        glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat(glm::radians(rotation)));
        modelMatrix = glm::translate(modelMatrix, position) * rotationMatrix * glm::scale(modelMatrix, scale);
    }

    void GameObject::draw(pgk::Shader& shader){
        shader.use();
        shader.setMat4("model", GameObject::modelMatrix);
        shader.setVec3("uColor", GameObject::color);

        mesh->draw();
    }

}
