#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include <stdint.h>
#include "../core/config.h"
#include "../core/entity/entity_manager.h"
#include "../core/types.h"
#include "../gameplay/player/player.h"
#include "../rendering/camera/camera.h"

// 核心基类（绝对通用）
typedef struct {
    float x, y;
} CameraCore;

typedef struct {
    EntityData entities[MAX_ENTITIES];
    int count;
} EntityFramework;



typedef struct {
    EntityFramework framework;
    int collision_type;  // 玩法扩展
} EntityGameplay;

// 场景子类（完全特定）
typedef struct {
    uint8_t tiles[MAP_SIZE * MAP_SIZE];
} MapScene;

typedef struct {
    EntityGameplay gameplay;
    // 场景特定实体（如果需要扩展）
} EntityScene;

// 完整上下文
typedef struct {
    MapScene map;
    EntityScene entities;
    PlayerMovement player;
    CameraGameplay camera;
} SceneContextData;

#endif