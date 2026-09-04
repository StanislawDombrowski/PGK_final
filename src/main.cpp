#include <cstdlib>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "pgk/core/Application.h"
#include "pgk/graphics/Mesh.h"
#include "pgk/graphics/Shader.h"

int main()
{
    try {
        pgk::Application app(1280, 720, "PGK");

        // Smoke-test triangle: proves the window/context, buffer-upload, and
        // shader-compilation pipeline works end to end. Replace this block
        // with pgk::Scene once the scene graph, geometry, and room exist.
        const std::vector<pgk::Vertex> triangleVertices = {
            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        };
        pgk::Mesh triangle(triangleVertices);
        pgk::Shader basicShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

        app.run(
            [&app](float /*deltaSeconds*/) {
                if (glfwGetKey(app.window().handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    app.window().setShouldClose(true);
                }
            },
            [&]() {
                basicShader.use();
                basicShader.setVec3("uColor", glm::vec3(1.0f, 0.5f, 0.2f));
                triangle.draw();
            });
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
