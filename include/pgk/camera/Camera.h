#pragma once

#include "glm/fwd.hpp"
#include <cstdint>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace pgk{
    class Camera{
    public:
      glm::vec3 position;
      glm::vec3 front;
      glm::float32 yaw, pitch;
      glm::mat4 viewMatrix;
      glm::mat4 projectionMatrix;

      glm::float32 aspect;

      Camera(glm::vec3 position, glm::float32 yaw, glm::float32 pitch, glm::float32 aspect);
      ~Camera() = default;

      // Recomputes `front` (from yaw/pitch) and `viewMatrix` (from position
      // + front). Call after Input changes position/yaw/pitch.
      void updateViewMatrix();
    };
}
