#include "pgk/scene/GameObject.h"

namespace pgk{

    GameObject::GameObject(pgk::Mesh& mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 material){
        GameObject::mesh = &mesh;

        GameObject::position = position;
        GameObject::rotation = rotation;
        GameObject::scale = scale;
        GameObject::color = material;

        updateModelMatrix();
    }

    void GameObject::updateModelMatrix(){
        glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat(glm::radians(rotation)));
        modelMatrix = glm::translate(glm::mat4(1.f), position) * rotationMatrix * glm::scale(glm::mat4(1.f), scale);
    }

    void GameObject::draw(pgk::Shader& shader){
        shader.use();
        shader.setMat4("model", GameObject::modelMatrix);
        shader.setVec3("uColor", GameObject::color);

        mesh->draw();
    }

};
