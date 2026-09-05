#include "pgk/scene/Room.h"

namespace pgk {

namespace {

GameObject makeWall(Mesh& cubeMesh, glm::vec3 position, glm::vec3 scale, glm::vec3 color)
{
    GameObject wall(cubeMesh, position, glm::vec3(0.0f), scale, color);
    wall.colliderShape = ColliderShape::Box;
    wall.colliderHalfExtents = scale * 0.5f;
    wall.isDynamic = false;
    return wall;
}

} // namespace

std::vector<GameObject> buildRoom(Mesh& cubeMesh, glm::vec3 size, glm::vec3 wallColor, float wallThickness)
{
    const float halfWidth = size.x * 0.5f;
    const float halfDepth = size.z * 0.5f;
    const float halfThickness = wallThickness * 0.5f;

    std::vector<GameObject> walls;
    walls.reserve(6);

    // Floor: interior surface at y=0.
    walls.push_back(makeWall(
        cubeMesh,
        glm::vec3(0.0f, -halfThickness, 0.0f),
        glm::vec3(size.x, wallThickness, size.z),
        wallColor));

    // Ceiling: interior surface at y=size.y.
    walls.push_back(makeWall(
        cubeMesh,
        glm::vec3(0.0f, size.y + halfThickness, 0.0f),
        glm::vec3(size.x, wallThickness, size.z),
        wallColor));

    // Back / front walls (perpendicular to Z).
    walls.push_back(makeWall(
        cubeMesh,
        glm::vec3(0.0f, size.y * 0.5f, -halfDepth - halfThickness),
        glm::vec3(size.x, size.y, wallThickness),
        wallColor));
    walls.push_back(makeWall(
        cubeMesh,
        glm::vec3(0.0f, size.y * 0.5f, halfDepth + halfThickness),
        glm::vec3(size.x, size.y, wallThickness),
        wallColor));

    // Left / right walls (perpendicular to X).
    walls.push_back(makeWall(
        cubeMesh,
        glm::vec3(-halfWidth - halfThickness, size.y * 0.5f, 0.0f),
        glm::vec3(wallThickness, size.y, size.z),
        wallColor));
    walls.push_back(makeWall(
        cubeMesh,
        glm::vec3(halfWidth + halfThickness, size.y * 0.5f, 0.0f),
        glm::vec3(wallThickness, size.y, size.z),
        wallColor));

    return walls;
}

} // namespace pgk
