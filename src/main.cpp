#include <cstdlib>
#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glm/fwd.hpp"
#include "pgk/camera/Camera.h"
#include "pgk/core/Application.h"
#include "pgk/core/Input.h"
#include "pgk/geometry/Cube.h"
#include "pgk/geometry/Sphere.h"
#include "pgk/graphics/Mesh.h"
#include "pgk/graphics/Shader.h"
#include "pgk/lighting/PointLight.h"
#include "pgk/physics/Collider.h"
#include "pgk/physics/PhysicsWorld.h"
#include "pgk/physics/RigidBody.h"
#include "pgk/scene/GameObject.h"
#include "pgk/utils/FpsCounter.h"

int main()
{
    try {
        glm::vec2 aspect(1280, 720);
        pgk::Application app(aspect.x, aspect.y, "PGK");
        glm::float32 aspect_ratio = aspect.x / aspect.y;

        // Smoke-test triangle: proves the window/context, buffer-upload, and
        // shader-compilation pipeline works end to end. Replace this block
        // with pgk::Scene once the scene graph, geometry, and room exist.

        pgk::Camera camera(glm::vec3(-3, 0, 0), 0, 0, aspect_ratio);
        pgk::Input input(app.window());
        pgk::FpsCounter fpsCounter;

        pgk::Mesh cube = pgk::buildCubeMesh();
        pgk::Mesh sphere = pgk::buildSphereMesh();

        pgk::GameObject cubeObject(cube, glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f), glm::vec3(20.f, 0.3f, 20.f), glm::vec3(1.0f, 0.5f, 0.2f));
        pgk::GameObject wallObject(cube, glm::vec3(0.f, 2.f, -11.f), glm::vec3(0.f), glm::vec3(15.f, 10.f, 0.3f), glm::vec3(1.0f, 0.5f, 0.2f));
        pgk::GameObject sphereObject(sphere, glm::vec3(0.f, 0.8f, 0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.2f, 0.32f, 0.82f));

        pgk::Collider groundCollider(cubeObject, glm::vec3(10.f, 0.15f, 10.f));
        pgk::Collider wallCollider(wallObject, glm::vec3(7.5f, 5.f, 0.15f));
        pgk::Collider sphereCollider(sphereObject, glm::vec3(1.f, 1.f, 1.f));

        pgk::PhysicsWorld physicsWorld;
        pgk::RigidBody sphereBody(sphereObject, sphereCollider);
        physicsWorld.addBody(sphereBody);
        physicsWorld.addStaticCollider(groundCollider);
        physicsWorld.addStaticCollider(wallCollider);

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


        pgk::Shader basicShader("assets/shaders/lit.vert", "assets/shaders/lit.frag");

        basicShader.use();
        basicShader.setMat4("projection", camera.projectionMatrix);

        app.run(
            [&](float deltaSeconds) {
                if (glfwGetKey(app.window().handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    app.window().setShouldClose(true);
                }
                input.update(camera, physicsWorld, deltaSeconds);
                physicsWorld.step(deltaSeconds);

                if (fpsCounter.update(deltaSeconds)) {
                    app.window().setTitle("PGK - " + std::to_string(static_cast<int>(fpsCounter.fps())) + " FPS");
                }
            },
            [&]() {
                basicShader.use();
                basicShader.setMat4("view", camera.viewMatrix);
                basicShader.setVec3("viewPos", camera.position);
                basicShader.setInt("numPointLights", 3);
                pointLight1.uploadTo(basicShader, 0);
                pointLight2.uploadTo(basicShader, 1);
                pointLight3.uploadTo(basicShader, 2);
                cubeObject.draw(basicShader);
                wallObject.draw(basicShader);
                sphereObject.draw(basicShader);
            });
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
