#ifndef TYPES_H
#define TYPES_H
typedef struct { float x; float y; } Vec2;
typedef struct { Vec2 pos; float z; int dir; } Entity;

// 实体类型枚举
typedef enum {
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_RED_BALL,
    ENTITY_TYPE_TRIGGER,
    ENTITY_TYPE_ENEMY,
    ENTITY_TYPE_NPC
} EntityType;

// 游戏实体结构
typedef struct {
    EntityType type;
    float x;
    float y;
    float z;
    int tile_id;
} GameEntity;

#define MAX_GAME_ENTITIES 32

#endif