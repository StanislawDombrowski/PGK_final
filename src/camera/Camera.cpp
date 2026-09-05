#include "pgk/camera/Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

namespace pgk{
    Camera::Camera(glm::vec3 position, glm::float32 yaw, glm::float32 pitch, glm::float32 aspect){
        Camera::position = position;
        Camera::yaw = yaw;
        Camera::pitch = pitch;

        Camera::aspect = aspect;

        Camera::projectionMatrix = glm::perspective(glm::radians(60.f), aspect, 0.01f, 100.f);
        updateViewMatrix();
    }

    void Camera::updateViewMatrix()
    {
        front = glm::normalize(glm::vec3(
            glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
            glm::sin(glm::radians(pitch)),
            glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))));

        viewMatrix = glm::lookAt(position, position + front, glm::vec3(0, 1, 0));
    }
}
