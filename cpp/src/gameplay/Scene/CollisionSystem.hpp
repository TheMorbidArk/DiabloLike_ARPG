#pragma once

#include "../../core/Types.hpp"
#include "../../core/Config.hpp"
#include "../../core/Entity/EntityManager.hpp"
#include "../../tic80/Tic80.hpp"
#include <cmath>

namespace gameplay {

struct CollisionResult {
    int entityIndex{-1};
    float distance{0.0f};
    core::EntityType entityType{core::EntityType::Player};
};

class CollisionSystem {
public:
    static CollisionSystem& get() {
        static CollisionSystem instance;
        return instance;
    }

    CollisionSystem(const CollisionSystem&) = delete;
    CollisionSystem& operator=(const CollisionSystem&) = delete;

    void init() {
    }

    auto checkEntityType(core::EntityType type) -> int {
        auto entities = core::EntityManager::get().getArray();
        const auto count = core::EntityManager::get().getCount();

        float playerX{}, playerY{}, playerZ{};
        const auto playerID = core::Context::get().getPlayerID();
        core::EntityManager::get().getPosition(playerID, &playerX, &playerY, &playerZ);

        for (size_t i = 0; i < count; ++i) {
            if (entities[i].type == type) {
                const float entityX = entities[i].entity.pos.x;
                const float entityY = entities[i].entity.pos.y;
                const float entityZ = entities[i].entity.pos.z;

                if (std::abs(playerX - entityX) <= 1.5f &&
                    std::abs(playerY - entityY) <= 1.5f &&
                    std::abs(playerZ - entityZ) < core::COLLISION_Z_THRESHOLD) {
                    return static_cast<int>(i);
                }
            }
        }
        return -1;
    }

    auto checkAnyEntity() -> int {
        auto entities = core::EntityManager::get().getArray();
        const auto count = core::EntityManager::get().getCount();

        float playerX{}, playerY{}, playerZ{};
        const auto playerID = core::Context::get().getPlayerID();
        core::EntityManager::get().getPosition(playerID, &playerX, &playerY, &playerZ);

        for (size_t i = 0; i < count; ++i) {
            if (entities[i].type == core::EntityType::Player) {
                continue;
            }

            const float entityX = entities[i].entity.pos.x;
            const float entityY = entities[i].entity.pos.y;
            const float entityZ = entities[i].entity.pos.z;

            if (std::abs(playerX - entityX) <= 1.5f &&
                std::abs(playerY - entityY) <= 1.5f &&
                std::abs(playerZ - entityZ) < core::COLLISION_Z_THRESHOLD) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    auto getDetailedInfo(int entityIndex) -> CollisionResult {
        CollisionResult result{-1, 0.0f, core::EntityType::Player};

        if (entityIndex < 0) return result;

        auto entities = core::EntityManager::get().getArray();
        const auto count = core::EntityManager::get().getCount();

        if (static_cast<size_t>(entityIndex) >= count) return result;

        float playerX{}, playerY{}, playerZ{};
        const auto playerID = core::Context::get().getPlayerID();
        core::EntityManager::get().getPosition(playerID, &playerX, &playerY, &playerZ);

        const auto& entity = entities[entityIndex];
        const float entityX = entity.entity.pos.x;
        const float entityY = entity.entity.pos.y;

        result.entityIndex = entityIndex;
        result.distance = calculateDistance(playerX, playerY, entityX, entityY);
        result.entityType = entity.type;

        return result;
    }

    auto checkInRange(float maxDistance) -> int {
        auto entities = core::EntityManager::get().getArray();
        const auto count = core::EntityManager::get().getCount();

        float playerX{}, playerY{}, playerZ{};
        const auto playerID = core::Context::get().getPlayerID();
        core::EntityManager::get().getPosition(playerID, &playerX, &playerY, &playerZ);

        for (size_t i = 0; i < count; ++i) {
            if (entities[i].type == core::EntityType::Player) continue;

            const float entityX = entities[i].entity.pos.x;
            const float entityY = entities[i].entity.pos.y;

            if (calculateDistance(playerX, playerY, entityX, entityY) <= maxDistance) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void debugInfo(int entityIndex) {
        if (entityIndex < 0) return;
    }

private:
    CollisionSystem() = default;

    static auto calculateDistance(float x1, float y1, float x2, float y2) -> float {
        const float dx = x1 - x2;
        const float dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }
};

}
