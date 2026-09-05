#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glm/fwd.hpp"
#include "pgk/camera/Camera.h"
#include "pgk/core/Application.h"
#include "pgk/core/Input.h"
#include "pgk/geometry/Cube.h"
#include "pgk/geometry/Sphere.h"
#include "pgk/geometry/Cone.h"
#include "pgk/graphics/Mesh.h"
#include "pgk/graphics/Shader.h"
#include "pgk/lighting/PointLight.h"
#include "pgk/scene/GameObject.h"
#include "pgk/scene/Room.h"
#include "pgk/scene/Scene.h"
#include "pgk/utils/FpsCounter.h"

int main()
{
    try {
        const int windowWidth = 1280;
        const int windowHeight = 720;
        pgk::Application app(windowWidth, windowHeight, "PGK");
        glm::float32 aspect_ratio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        pgk::Camera camera(glm::vec3(-3, 1.5f, 0), 0, 0, aspect_ratio);
        pgk::Input input(app.window());
        pgk::FpsCounter fpsCounter;

        pgk::Mesh cube = pgk::buildCubeMesh();
        pgk::Mesh sphere = pgk::buildSphereMesh();
        pgk::Mesh cone = pgk::buildConeMesh(24);

        // Room walls, ready-flagged as static box colliders.
        std::vector<pgk::GameObject> sceneObjects =
            pgk::buildRoom(cube, glm::vec3(20.f, 10.f, 20.f), glm::vec3(1.0f, 0.5f, 0.2f));

        pgk::GameObject sphereObject(sphere, glm::vec3(0.f, 0.8f, 0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.2f, 0.32f, 0.82f));
        sphereObject.colliderShape = pgk::ColliderShape::Sphere;
        sphereObject.colliderRadius = 1.f;
        sphereObject.isDynamic = true;
        sceneObjects.push_back(sphereObject);

        pgk::GameObject coneObject(cone, glm::vec3(2.f, 0.8f, 1.f), glm::vec3(0.f), glm::vec3(1.f, 2.f, 1.f), glm::vec3(0.58f, 0.14f, 0.57f));
        coneObject.colliderShape = pgk::ColliderShape::Cone;
        coneObject.colliderRadius = 1.f;
        coneObject.colliderHeight = 2.f;
        coneObject.isDynamic = true;
        coneObject.canBePushed = false;
        sceneObjects.push_back(coneObject);

        pgk::GameObject sphereObject2(sphere, glm::vec3(-3.f, 3.f, -2.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.85f, 0.2f, 0.2f));
        sphereObject2.colliderShape = pgk::ColliderShape::Sphere;
        sphereObject2.colliderRadius = 1.f;
        sphereObject2.isDynamic = true;
        sceneObjects.push_back(sphereObject2);

        pgk::GameObject sphereObject3(sphere, glm::vec3(3.f, 4.f, -2.f), glm::vec3(0.f), glm::vec3(0.75f), glm::vec3(0.2f, 0.8f, 0.3f));
        sphereObject3.colliderShape = pgk::ColliderShape::Sphere;
        sphereObject3.colliderRadius = 0.75f;
        sphereObject3.isDynamic = true;
        sceneObjects.push_back(sphereObject3);

        pgk::GameObject sphereObject4(sphere, glm::vec3(-2.f, 5.f, 3.f), glm::vec3(0.f), glm::vec3(1.25f), glm::vec3(0.9f, 0.75f, 0.15f));
        sphereObject4.colliderShape = pgk::ColliderShape::Sphere;
        sphereObject4.colliderRadius = 1.25f;
        sphereObject4.isDynamic = true;
        sceneObjects.push_back(sphereObject4);

        pgk::Scene scene(std::move(sceneObjects));

        pgk::PointLight pointLight1(
            glm::vec3(2.0f, 3.f, 0.f),   // position
            glm::vec3(1.0f, 1.0f, 1.0f),   // color
            5.f,   // ambientStrength
            1.f,    // diffuseStrength
            0.8f,    // specularStrength
            1.0f,    // constant
            0.35f,   // linear
            0.44f    // quadratic
        );

        pgk::PointLight pointLight2(
            glm::vec3(-5.0f, 7.f, -5.0f),   // position
            glm::vec3(1.0f, 1.0f, 1.0f),   // color
            5.f,   // ambientStrength
            1.f,    // diffuseStrength
            0.8f,    // specularStrength
            1.0f,    // constant
            0.35f,   // linear
            0.44f    // quadratic
        );

        pgk::PointLight pointLight3(
            glm::vec3(5.0f, 7.f, -5.0f),   // position
            glm::vec3(1.0f, 1.0f, 1.0f),   // color
            5.f,   // ambientStrength
            1.f,    // diffuseStrength
            0.8f,    // specularStrength
            1.0f,    // constant
            0.35f,   // linear
            0.44f    // quadratic
        );

        pgk::PointLight pointLight4(
            glm::vec3(-5.0f, 7.f, 5.0f),    // position
            glm::vec3(1.0f, 0.6f, 0.3f),    // color: warm accent
            5.f,   // ambientStrength
            1.f,    // diffuseStrength
            0.8f,    // specularStrength
            1.0f,    // constant
            0.35f,   // linear
            0.44f    // quadratic
        );

        pgk::PointLight pointLight5(
            glm::vec3(5.0f, 7.f, 5.0f),     // position
            glm::vec3(0.3f, 0.6f, 1.0f),    // color: cool accent
            5.f,   // ambientStrength
            1.f,    // diffuseStrength
            0.8f,    // specularStrength
            1.0f,    // constant
            0.35f,   // linear
            0.44f    // quadratic
        );


        pgk::Shader basicShader("assets/shaders/lit.vert", "assets/shaders/lit.frag");

        basicShader.use();
        basicShader.setMat4("projection", camera.projectionMatrix);

        app.run(
            [&](float deltaSeconds) {
                if (glfwGetKey(app.window().handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    app.window().setShouldClose(true);
                }
                input.update(camera, scene.physicsWorld(), deltaSeconds);
                scene.update(deltaSeconds);

                if (fpsCounter.update(deltaSeconds)) {
                    app.window().setTitle("PGK - " + std::to_string(static_cast<int>(fpsCounter.fps())) + " FPS");
                }
            },
            [&]() {
                basicShader.use();
                basicShader.setMat4("view", camera.viewMatrix);
                basicShader.setVec3("viewPos", camera.position);
                basicShader.setInt("numPointLights", 5);
                pointLight1.uploadTo(basicShader, 0);
                pointLight2.uploadTo(basicShader, 1);
                pointLight3.uploadTo(basicShader, 2);
                pointLight4.uploadTo(basicShader, 3);
                pointLight5.uploadTo(basicShader, 4);
                scene.draw(basicShader);
            });
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
