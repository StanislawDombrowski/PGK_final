#include "pgk/core/Application.h"

#include <GLFW/glfw3.h>

namespace pgk {

Application::Application(int width, int height, const std::string& title)
    : m_window(width, height, title)
{
    m_renderer.init();
    m_renderer.setViewport(width, height);
    m_window.setResizeCallback([this](int w, int h) { m_renderer.setViewport(w, h); });
}

void Application::run(const UpdateCallback& onUpdate, const RenderCallback& onRender)
{
    float lastTime = static_cast<float>(glfwGetTime());

    while (!m_window.shouldClose()) {
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaSeconds = currentTime - lastTime;
        lastTime = currentTime;

        Window::pollEvents();

        if (onUpdate) {
            onUpdate(deltaSeconds);
        }

        m_renderer.clear();
        if (onRender) {
            onRender();
        }

        m_window.swapBuffers();
    }
}

} // namespace pgk
