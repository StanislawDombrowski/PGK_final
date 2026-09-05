#include "pgk/core/Input.h"

#include <algorithm>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

namespace pgk {

Input::Input(Window& window)
    : m_window(window)
{
    glfwGetCursorPos(window.handle(), &m_lastMouseX, &m_lastMouseY);
}

void Input::update(Camera& camera, float deltaSeconds)
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

    m_wasRotating = isRotating;
    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    camera.updateViewMatrix();
}

} // namespace pgk
