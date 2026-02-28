#pragma once

#include "Types.hpp"

namespace core {

class Context {
public:
    static auto get() -> Context& {
        static Context instance;
        return instance;
    }

    Context(const Context&) = delete;
    auto operator=(const Context&) -> Context& = delete;

    void setPlayerID(EntityID id) { m_playerID = id; }
    [[nodiscard]] auto getPlayerID() const -> EntityID { return m_playerID; }

    void setCurrentScene(SceneType type) { m_currentScene = type; }
    [[nodiscard]] auto getCurrentScene() const -> SceneType { return m_currentScene; }

    void setMapSeed(uint32_t seed) { m_mapSeed = seed; }
    [[nodiscard]] auto getMapSeed() const -> uint32_t { return m_mapSeed; }

    void setGameRunning(bool running) { m_gameRunning = running; }
    [[nodiscard]] auto isGameRunning() const -> bool { return m_gameRunning; }

private:
    Context() = default;

    EntityID m_playerID{INVALID_ENTITY_ID};
    SceneType m_currentScene{SceneType::Exploration};
    uint32_t m_mapSeed{777};
    bool m_gameRunning{true};
};

}
