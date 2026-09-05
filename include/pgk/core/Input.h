#pragma once

#include "pgk/camera/Camera.h"
#include "pgk/core/Window.h"
#include "pgk/physics/PhysicsWorld.h"

namespace pgk {

// Polls WASD + mouse state each frame and applies it directly to a Camera:
// W/A/S/D move relative to the camera's own front/right axes, the mouse
// rotates the camera only while the right mouse button is held, and on the
// frame the left mouse button is first pressed (not on every frame it's
// held), raycasts from the camera along its front vector and pushes
// whichever RigidBody it hits first, provided that hit is within
// m_pushMaxDistance.
class Input {
public:
    explicit Input(Window& window);

    void update(Camera& camera, PhysicsWorld& physicsWorld, float deltaSeconds);

private:
    void tryPushObjectUnderCursor(const Camera& camera, PhysicsWorld& physicsWorld) const;

    Window& m_window;

    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_wasRotating = false;
    bool m_wasLeftMouseDown = false;

    float m_moveSpeed = 3.0f;        // world units per second
    float m_mouseSensitivity = 0.1f; // degrees per pixel of mouse movement

    float m_pushForceMagnitude = 15.0f; // impulse strength applied along camera.front
    float m_pushMaxDistance = 10.0f;    // objects farther than this along the ray are ignored
};

} // namespace pgk
