#pragma once

#include "core/Types.hpp"
#include "core/Config.hpp"
#include <array>
#include <span>
#include <optional>
#include <functional>

namespace core {

class EntityManager {
public:
    static auto get() -> EntityManager& {
        static EntityManager instance;
        return instance;
    }

    EntityManager(const EntityManager&) = delete;
    auto operator=(const EntityManager&) -> EntityManager& = delete;

    void init() {
        m_entities.fill(EntityData{});
        m_count = 0;
        m_nextID = 1;
    }

    [[nodiscard]] auto add(EntityType type, float x, float y, float z, uint16_t tileId) -> EntityID {
        if (m_count >= core::MAX_ENTITIES) {
            return INVALID_ENTITY_ID;
        }

        const EntityID id = m_nextID++;
        auto& data = m_entities[m_count];
        data.id = id;
        data.type = type;
        data.tileId = tileId;
        data.entity = Entity{
            .pos = Position{x, y, z},
            .type = type,
            .tileId = tileId,
            .active = true
        };
        m_count++;

        if (type == EntityType::Player) {
            Context::get().setPlayerID(id);
        }

        return id;
    }

    [[nodiscard]] auto get(EntityID id) -> std::optional<std::reference_wrapper<EntityData>> {
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entities[i].id == id && m_entities[i].entity.active) {
                return m_entities[i];
            }
        }
        return std::nullopt;
    }

    void remove(EntityID id) {
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entities[i].id == id) {
                m_entities[i].entity.active = false;
                return;
            }
        }
    }

    [[nodiscard]] auto getPlayer() -> std::optional<std::reference_wrapper<EntityData>> {
        return get(Context::get().getPlayerID());
    }

    [[nodiscard]] auto getCount() const -> size_t { return m_count; }

    [[nodiscard]] auto getArray() -> std::span<EntityData> {
        return std::span<EntityData>(m_entities.data(), m_count);
    }

    void getPosition(EntityID id, float* x, float* y, float* z) const {
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entities[i].id == id) {
                *x = m_entities[i].entity.pos.x;
                *y = m_entities[i].entity.pos.y;
                *z = m_entities[i].entity.pos.z;
                return;
            }
        }
    }

    void setPosition(EntityID id, float x, float y, float z) {
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entities[i].id == id) {
                m_entities[i].entity.pos.x = x;
                m_entities[i].entity.pos.y = y;
                m_entities[i].entity.pos.z = z;
                return;
            }
        }
    }

private:
    EntityManager() = default;

    std::array<EntityData, core::MAX_ENTITIES> m_entities{};
    size_t m_count{0};
    EntityID m_nextID{1};
};

}
