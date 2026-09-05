#pragma once

#include "pgk/camera/Camera.h"
#include "pgk/core/Window.h"

namespace pgk {

// Polls WASD + right-mouse-drag state each frame and applies it directly to
// a Camera: W/A/S/D move relative to the camera's own front/right axes, and
// the mouse rotates the camera only while the right mouse button is held.
class Input {
public:
    explicit Input(Window& window);

    void update(Camera& camera, float deltaSeconds);

private:
    Window& m_window;

    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_wasRotating = false;

    float m_moveSpeed = 3.0f;        // world units per second
    float m_mouseSensitivity = 0.1f; // degrees per pixel of mouse movement
};

} // namespace pgk
