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
#include "pgk/graphics/Mesh.h"
#include "pgk/graphics/Shader.h"
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

        pgk::GameObject cubeObject1(cube, glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(1.0f, 0.5f, 0.2f));
        pgk::GameObject cubeObject2(cube, glm::vec3(0.f, 0.f, 2.f), glm::vec3(5.f, 2.f, 0.f), glm::vec3(1.f), glm::vec3(0.2f, 0.32f, 0.82f));

        pgk::Shader basicShader("assets/shaders/basic_model.vert", "assets/shaders/basic.frag");

        basicShader.use();
        basicShader.setMat4("projection", camera.projectionMatrix);

        app.run(
            [&](float deltaSeconds) {
                if (glfwGetKey(app.window().handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    app.window().setShouldClose(true);
                }
                input.update(camera, deltaSeconds);

                if (fpsCounter.update(deltaSeconds)) {
                    app.window().setTitle("PGK - " + std::to_string(static_cast<int>(fpsCounter.fps())) + " FPS");
                }
            },
            [&]() {
                basicShader.use();
                basicShader.setMat4("view", camera.viewMatrix);
                cubeObject1.draw(basicShader);
                cubeObject2.draw(basicShader);
            });
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
