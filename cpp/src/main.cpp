#include "tic80/Tic80.hpp"
#include "core/Config.hpp"
#include "core/Types.hpp"
#include "core/Context.hpp"
#include "core/Entity/EntityManager.hpp"
#include "rendering/Assets/Assets.hpp"
#include "rendering/Map/Map.hpp"
#include "rendering/Camera/Camera.hpp"
#include "rendering/Renderer/Renderer.hpp"
#include "gameplay/Player/Player.hpp"
#include "gameplay/Battle/Battle.hpp"
#include "gameplay/Scene/SceneManager.hpp"
#include "gameplay/Scene/CollisionSystem.hpp"

extern "C" {

void scene_init();
void scene_update();
void scene_render();

WASM_EXPORT("BOOT")
void BOOT() {
    rendering::Assets::get().init();
    rendering::Map::get().init();
    scene_init();
    core::EntityManager::get().init();
    
    const float startX = static_cast<float>(core::MAP_SIZE / 2);
    const float startY = static_cast<float>(core::MAP_SIZE / 2);
    core::EntityID playerId = core::EntityManager::get().add(core::EntityType::Player, startX, startY, 0.0f, static_cast<uint16_t>(core::ID_PLAYER));
    core::Context::get().setPlayerID(playerId);
    core::EntityManager::get().add(core::EntityType::RedBall, 48.0f, 40.0f, 0.0f, static_cast<uint16_t>(core::ID_RED_BALL));

    rendering::Map::get().generate(777, true);
    
    auto playerData = core::EntityManager::get().getPlayer();
    if (playerData.has_value()) {
        gameplay::Player::get().setPosition(
            playerData->get().entity.pos.x,
            playerData->get().entity.pos.y,
            playerData->get().entity.pos.z
        );
    }
    
    rendering::Camera::get().init(0.0f, 0.0f);
}

WASM_EXPORT("TIC")
void TIC() {
    scene_update();
    scene_render();
}

}

int g_currentScene = 0;
int g_battleCooldown = 0;

extern "C" {

void scene_init() {
    gameplay::CollisionSystem::get().init();
    gameplay::sceneInit();
}

void scene_update() {
    auto playerData = core::EntityManager::get().getPlayer();
    if (!playerData.has_value()) return;

    auto& player = playerData->get().entity;

    if (tic80::btn(0)) gameplay::Player::get().move(-1.0f, 0.0f);
    if (tic80::btn(1)) gameplay::Player::get().move(1.0f, 0.0f);
    if (tic80::btn(2)) gameplay::Player::get().move(0.0f, -1.0f);
    if (tic80::btn(3)) gameplay::Player::get().move(0.0f, 1.0f);
    if (tic80::btnp(4)) gameplay::Player::get().jump();

    gameplay::Player::get().update();

    float newX{}, newY{}, newZ{};
    gameplay::Player::get().getPosition(&newX, &newY, &newZ);
    player.pos.x = newX;
    player.pos.y = newY;
    player.pos.z = newZ;

    if (g_battleCooldown > 0) {
        g_battleCooldown--;
    }

    if (g_battleCooldown == 0 && g_currentScene == 0) {
        int collision = gameplay::CollisionSystem::get().checkAnyEntity();
        if (collision >= 0) {
            auto entities = core::EntityManager::get().getArray();
            if (entities[static_cast<size_t>(collision)].type == core::EntityType::RedBall) {
                g_currentScene = 1;
                g_battleCooldown = 120;
            }
        }
    }

    rendering::Camera::get().update(&playerData->get().entity);
}

void scene_render() {
    auto playerData = core::EntityManager::get().getPlayer();
    if (!playerData.has_value()) {
        tic80::trace("NO PLAYER", 0);
        return;
    }
    
    if (g_currentScene == 0) {
        tic80::print("TEST", 100, 60, core::COLOR_WHITE, 0, 1, 0);
        rendering::Renderer::get().renderScene(&playerData->get().entity);
    } else {
        tic80::cls(core::COLOR_DARK_BLUE);
        tic80::print("BATTLE SCENE", 80, 60, core::COLOR_WHITE, 0, 1, 0);
        tic80::print("Press Z to fight", 70, 80, core::COLOR_YELLOW, 0, 1, 0);
        
        if (tic80::btnp(4)) {
            g_currentScene = 0;
        }
    }
}

}
