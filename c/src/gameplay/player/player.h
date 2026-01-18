#ifndef PLAYER_H
#define PLAYER_H

#include "../../core/types.h"

// 定义Player相关结构体
typedef struct {
    int id;
    float health;
} PlayerCore;

typedef struct {
    PlayerCore core;
    float hp;  // 战斗玩法扩展
} PlayerGameplay;

typedef struct {
    PlayerGameplay gameplay;
    Vec2 pos;
    float speed;  // 移动玩法扩展
} PlayerMovement;

// 定义Player结构体
typedef struct {
    Vec2 pos;
    float z;
    float speed;
    int id;
    float health;
    float hp;
} Player;

extern Player player;

void player_update();

// 上下文保存/恢复
void player_save_movement(PlayerMovement* state);
void player_restore_movement(const PlayerMovement* state);

#endif