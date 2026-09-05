#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "pgk/graphics/Mesh.h"

namespace pgk{
    pgk::Mesh buildSphereMesh(glm::uint32_t sectorCount = 32, glm::uint32_t stackCount = 18);
};
