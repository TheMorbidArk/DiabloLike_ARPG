#pragma once

#include "core/Config.hpp"
#include "core/Types.hpp"
#include "core/Context.hpp"
#include <cstddef>
#include <cstdint>

namespace core {

enum class GameScene : uint8_t {
    Exploration = 0,
    Battle = 1,
    FreeWalk = 2
};

struct SceneContextData {
    uint8_t tiles[MAP_SIZE * MAP_SIZE]{};
};

struct SceneData {
    float playerX{};
    float playerY{};
    int fromScene{};
    int battleResult{};
    void* sceneData{nullptr};
    SceneContextData contextData;
};

}

namespace gameplay {

class IScene {
public:
    virtual ~IScene() = default;
    virtual void init(core::SceneData* ctx) = 0;
    virtual void enter(core::SceneData* ctx) = 0;
    virtual void update(core::SceneData* ctx) = 0;
    virtual void render(core::SceneData* ctx) = 0;
    virtual void exit(core::SceneData* ctx) = 0;
    virtual void cleanup(core::SceneData* ctx) = 0;
};

}
