#include "pgk/core/Input.h"

#include <algorithm>
#include <limits>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

namespace pgk {

namespace {

// Slab-method ray/AABB intersection. Returns true and sets tHit (distance
// along the ray) if the ray hits the box in front of the origin.
bool rayIntersectsBounds(const glm::vec3& origin, const glm::vec3& direction, const Bounds& bounds, float& tHit)
{
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    for (int axis = 0; axis < 3; ++axis) {
        const float invDir = 1.0f / direction[axis];
        float t0 = (bounds.min[axis] - origin[axis]) * invDir;
        float t1 = (bounds.max[axis] - origin[axis]) * invDir;
        if (invDir < 0.0f) {
            std::swap(t0, t1);
        }
        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);
        if (tMax <= tMin) {
            return false;
        }
    }

    tHit = tMin;
    return true;
}

} // namespace

Input::Input(Window& window)
    : m_window(window)
{
    glfwGetCursorPos(window.handle(), &m_lastMouseX, &m_lastMouseY);
}

void Input::tryPushObjectUnderCursor(const Camera& camera, PhysicsWorld& physicsWorld) const
{
    RigidBody* nearestBody = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (RigidBody* body : physicsWorld.bodies()) {
        float tHit = 0.0f;
        if (rayIntersectsBounds(camera.position, camera.front, body->collider().bounds(), tHit) && tHit < nearestDistance) {
            nearestDistance = tHit;
            nearestBody = body;
        }
    }

    if (nearestBody && nearestDistance <= m_pushMaxDistance) {
        const glm::vec3 contactPoint = camera.position + camera.front * nearestDistance;
        nearestBody->addForce(camera.front * m_pushForceMagnitude, contactPoint);
    }
}

void Input::update(Camera& camera, PhysicsWorld& physicsWorld, float deltaSeconds)
{
    GLFWwindow* handle = m_window.handle();

    // --- WASD movement, relative to the camera's own front/right axes ---
    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    const glm::vec3 right = glm::normalize(glm::cross(camera.front, worldUp));

    glm::vec3 moveDir(0.0f);
    if (glfwGetKey(handle, GLFW_KEY_W) == GLFW_PRESS) moveDir += camera.front;
    if (glfwGetKey(handle, GLFW_KEY_S) == GLFW_PRESS) moveDir -= camera.front;
    if (glfwGetKey(handle, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;
    if (glfwGetKey(handle, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;

    if (glm::length(moveDir) > 0.0f) {
        camera.position += glm::normalize(moveDir) * m_moveSpeed * deltaSeconds;
    }

    // --- Right-mouse-drag look ---
    double mouseX = m_lastMouseX;
    double mouseY = m_lastMouseY;
    glfwGetCursorPos(handle, &mouseX, &mouseY);

    const bool isRotating = glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    // Only apply a delta once we know both this frame and last frame had the
    // button held, so the very first frame of a drag doesn't jump using a
    // stale mouse position from while the button was up.
    if (isRotating && m_wasRotating) {
        const float deltaX = static_cast<float>(mouseX - m_lastMouseX) * m_mouseSensitivity;
        const float deltaY = static_cast<float>(m_lastMouseY - mouseY) * m_mouseSensitivity;

        camera.yaw += deltaX;
        camera.pitch = std::clamp(camera.pitch + deltaY, -89.0f, 89.0f);
    }

    // --- Left-click push ---
    const bool isLeftMouseDown = glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Push whatever's under the crosshair on the moment the button goes
    // down (not every frame it's held), so holding it down doesn't spam force.
    if (isLeftMouseDown && !m_wasLeftMouseDown) {
        tryPushObjectUnderCursor(camera, physicsWorld);
    }

    m_wasLeftMouseDown = isLeftMouseDown;
    m_wasRotating = isRotating;
    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    camera.updateViewMatrix();
}

} // namespace pgk
