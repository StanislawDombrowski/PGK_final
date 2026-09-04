#pragma once

#include <functional>
#include <string>

#include "pgk/core/Window.h"
#include "pgk/graphics/Renderer.h"

namespace pgk {

class Application {
public:
    using UpdateCallback = std::function<void(float deltaSeconds)>;
    using RenderCallback = std::function<void()>;

    Application(int width, int height, const std::string& title);

    Window& window() { return m_window; }
    Renderer& renderer() { return m_renderer; }

    // Runs the main loop: poll input, call onUpdate(dt), clear the frame,
    // call onRender(), swap buffers. Returns once the window is closed.
    void run(const UpdateCallback& onUpdate, const RenderCallback& onRender);

private:
    Window m_window;
    Renderer m_renderer;
};

} // namespace pgk
