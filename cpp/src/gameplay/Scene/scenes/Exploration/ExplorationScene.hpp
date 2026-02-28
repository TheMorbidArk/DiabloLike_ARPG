#pragma once

#include "../../../Scene/SceneInterface.hpp"
#include "../../Battle/Battle.hpp"
#include "../../Player/Player.hpp"
#include "../../../rendering/Renderer/Renderer.hpp"
#include "../../../rendering/Camera/Camera.hpp"
#include "../../../rendering/Map/Map.hpp"
#include "../../../core/Entity/EntityManager.hpp"

namespace gameplay {

class ExplorationScene : public IScene {
public:
    ExplorationScene() = default;
    ~ExplorationScene() override = default;

    void init(core::SceneData* ctx) override {
        (void)ctx;
        m_data.battleCooldown = 0;
    }

    void enter(core::SceneData* ctx) override {
        if (ctx->fromScene == 1) { // Battle
            m_data.battleCooldown = 120;
        } else {
            m_data.battleCooldown = 0;
        }

        if (ctx->contextData.tiles[0] == 0) {
            rendering::Map::get().generate(12345, true);
        }
    }

    void update(core::SceneData* ctx) override {
        (void)ctx;

        if (tic80::btnp(6, 60, 6)) {
            sceneSwitch(core::GameScene::FreeWalk);
            return;
        }

        if (!rendering::Camera::get().isTouring()) {
            gameplay::Player::get().update();
            
            auto playerData = core::EntityManager::get().getPlayer();
            if (playerData.has_value()) {
                float px, py, pz;
                gameplay::Player::get().getPosition(&px, &py, &pz);
                playerData->get().entity.pos.x = px;
                playerData->get().entity.pos.y = py;
                playerData->get().entity.pos.z = pz;
            }
        }

        if (m_data.battleCooldown > 0) {
            m_data.battleCooldown--;
        }

        if (m_data.battleCooldown == 0) {
            auto playerData = core::EntityManager::get().getPlayer();
            if (playerData.has_value()) {
                auto& player = playerData->get();
                for (size_t i = 0; i < core::EntityManager::get().getCount(); ++i) {
                    auto entities = core::EntityManager::get().getArray();
                    if (entities[i].entity.active && 
                        entities[i].type == core::EntityType::RedBall &&
                        entities[i].entity.pos.x == static_cast<int>(player.entity.pos.x) &&
                        entities[i].entity.pos.y == static_cast<int>(player.entity.pos.y)) {
                        triggerBattle(ctx, 0);
                        break;
                    }
                }
            }
        }
    }

    void render(core::SceneData* ctx) override {
        (void)ctx;
        
        auto playerData = core::EntityManager::get().getPlayer();
        if (playerData.has_value()) {
            rendering::Camera::get().setTarget(
                playerData->get().entity.pos.x * 16.0f - core::WIDTH / 2.0f,
                playerData->get().entity.pos.y * 8.0f - core::HEIGHT / 2.0f
            );
            rendering::Camera::get().update();
            rendering::Renderer::get().renderScene(&playerData->get().entity);
        }
    }

    void exit(core::SceneData* ctx) override {
        (void)ctx;
    }

    void cleanup(core::SceneData* ctx) override {
        (void)ctx;
    }

    void triggerBattle(core::SceneData* ctx, int enemyType) {
        (void)ctx;
        (void)enemyType;
        BattleSystem::get().startBattle();
        sceneSwitch(core::GameScene::Battle);
    }

private:
    struct Data {
        int battleCooldown{0};
    };
    Data m_data{};
};

}
