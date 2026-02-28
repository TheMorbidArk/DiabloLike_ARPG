#pragma once

#include "core/Types.hpp"
#include "core/Config.hpp"
#include <cmath>

namespace utils {

constexpr float ISO_W = 16.0f;
constexpr float ISO_H = 8.0f;
constexpr float HALF_ISO_W = ISO_W / 2.0f;
constexpr float HALF_ISO_H = ISO_H / 2.0f;

constexpr auto worldToScreen(float wx, float wy, float wz, int& sx, int& sy) -> void {
    sx = static_cast<int>((wx - wy) * HALF_ISO_W + core::WIDTH / 2);
    sy = static_cast<int>((wx + wy) * HALF_ISO_H - wz + core::HEIGHT / 2);
}

constexpr auto screenToWorld(int sx, int sy, float wz, float& wx, float& wy) -> void {
    const float adjX = static_cast<float>(sx - core::WIDTH / 2);
    const float adjY = static_cast<float>(sy - core::HEIGHT / 2) + wz;
    wx = (adjX / HALF_ISO_W + adjY / HALF_ISO_H) / 2.0f;
    wy = (adjY / HALF_ISO_H - adjX / HALF_ISO_W) / 2.0f;
}

constexpr auto worldToScreenVec(const core::Vec3f& world) -> core::Vec2i {
    int sx, sy;
    worldToScreen(world.x, world.y, world.z, sx, sy);
    return core::Vec2i{static_cast<int32_t>(sx), static_cast<int32_t>(sy)};
}

constexpr auto screenToWorldVec(const core::Vec2i& screen, float wz = 0.0f) -> core::Vec3f {
    float wx, wy;
    screenToWorld(screen.x, screen.y, wz, wx, wy);
    return core::Vec3f{wx, wy, wz};
}

constexpr auto distance2D(float x1, float y1, float x2, float y2) -> float {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    return dx * dx + dy * dy;
}

constexpr auto distance3D(float x1, float y1, float z1, float x2, float y2, float z2) -> float {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float dz = z2 - z1;
    return dx * dx + dy * dy + dz * dz;
}

constexpr auto distance(float x1, float y1, float x2, float y2) -> float {
    return distance2D(x1, y1, x2, y2);
}

constexpr auto distance(float x1, float y1, float z1, float x2, float y2, float z2) -> float {
    return distance3D(x1, y1, z1, x2, y2, z2);
}

}
