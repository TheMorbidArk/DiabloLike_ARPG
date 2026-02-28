#pragma once

#include "../../core/Config.hpp"
#include "../../core/Types.hpp"
#include "../../core/Context.hpp"
#include <cstddef>
#include <cstdint>

namespace core {

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
    virtual void init(::core::SceneData* ctx) = 0;
    virtual void enter(::core::SceneData* ctx) = 0;
    virtual void update(::core::SceneData* ctx) = 0;
    virtual void render(::core::SceneData* ctx) = 0;
    virtual void exit(::core::SceneData* ctx) = 0;
    virtual void cleanup(::core::SceneData* ctx) = 0;
};

class SceneManager {
public:
    static SceneManager& get() {
        static SceneManager instance;
        return instance;
    }

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    void init() {
        m_currentScene = ::core::GameScene::Exploration;
        m_previousScene = ::core::GameScene::Exploration;
        m_isTransitioning = false;
    }

    void switchTo(::core::GameScene newScene) {
        if (m_currentScene == newScene) return;

        m_isTransitioning = true;
        m_context.fromScene = static_cast<int>(m_currentScene);

        if (m_currentSceneImpl) {
            m_currentSceneImpl->exit(&m_context);
        }

        m_previousScene = m_currentScene;
        m_currentScene = newScene;

        if (m_currentSceneImpl) {
            m_currentSceneImpl->enter(&m_context);
        }

        m_isTransitioning = false;
    }

    void update() {
        if (m_isTransitioning) return;
        if (m_currentSceneImpl) {
            m_currentSceneImpl->update(&m_context);
        }
    }

    void render() {
        if (m_isTransitioning) return;
        if (m_currentSceneImpl) {
            m_currentSceneImpl->render(&m_context);
        }
    }

    [[nodiscard]] auto getCurrentScene() const -> ::core::GameScene { return m_currentScene; }
    [[nodiscard]] auto isTransitioning() const -> bool { return m_isTransitioning; }
    [[nodiscard]] auto getContext() -> ::core::SceneData& { return m_context; }

    void setSceneImpl(IScene* impl) { m_currentSceneImpl = impl; }

private:
    SceneManager() = default;

    ::core::GameScene m_currentScene{::core::GameScene::Exploration};
    ::core::GameScene m_previousScene{::core::GameScene::Exploration};
    bool m_isTransitioning{false};
    ::core::SceneData m_context{};
    IScene* m_currentSceneImpl{nullptr};
};

inline void sceneInit() {
    SceneManager::get().init();
}

inline void sceneUpdate() {
    SceneManager::get().update();
}

inline void sceneRender() {
    SceneManager::get().render();
}

inline void sceneSwitch(::core::GameScene type) {
    SceneManager::get().switchTo(type);
}

inline ::core::GameScene sceneGetCurrent() {
    return SceneManager::get().getCurrentScene();
}

}
