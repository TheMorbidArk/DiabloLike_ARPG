#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <optional>
#include <variant>
#include <concepts>
#include <string_view>

namespace core {

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
struct Vec2 {
    T x{};
    T y{};

    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}

    constexpr auto operator+(const Vec2& other) const -> Vec2 {
        return Vec2{x + other.x, y + other.y};
    }
    constexpr auto operator-(const Vec2& other) const -> Vec2 {
        return Vec2{x - other.x, y - other.y};
    }
    constexpr auto operator*(T scalar) const -> Vec2 {
        return Vec2{x * scalar, y * scalar};
    }
};

template <Numeric T>
struct Vec3 {
    T x{};
    T y{};
    T z{};

    constexpr Vec3() = default;
    constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    constexpr Vec3(const Vec2<T>& xy, T z) : x(xy.x), y(xy.y), z(z) {}

    constexpr auto operator+(const Vec3& other) const -> Vec3 {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }
    constexpr auto operator-(const Vec3& other) const -> Vec3 {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }
    constexpr auto operator*(T scalar) const -> Vec3 {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    constexpr auto xy() const -> Vec2<T> { return Vec2<T>{x, y}; }
};

using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec2i = Vec2<int32_t>;
using Vec3i = Vec3<int32_t>;

enum class EntityType : uint8_t {
    Player,
    RedBall,
    Enemy,
    Item,
    Wall,
    Grass,
    Outside
};

enum class SceneType : uint8_t {
    Exploration,
    Battle,
    FreeWalk
};

using EntityID = uint16_t;
constexpr EntityID INVALID_ENTITY_ID = 0xFFFF;

struct Position {
    float x{};
    float y{};
    float z{};

    constexpr Position() = default;
    constexpr Position(float x, float y, float z = 0.0f) : x(x), y(y), z(z) {}
    constexpr Position(const Vec3f& v) : x(v.x), y(v.y), z(v.z) {}

    constexpr auto toVec3() const -> Vec3f { return Vec3f{x, y, z}; }
};

struct Entity {
    Position pos;
    EntityType type{};
    uint16_t tileId{};
    bool active{false};
};

struct EntityData {
    Entity entity;
    EntityID id{};
    EntityType type{};
    uint16_t tileId{};
};

}
