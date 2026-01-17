#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "../types.h"
#include <stdbool.h>

#define ENTITY_ID_PLAYER 0
#define MAX_ENTITIES 64

typedef int EntityID;

typedef struct {
    Entity entity;
    EntityType type;
    int tile_id;
} EntityData;

void entity_manager_init();

EntityID entity_add(EntityType type, float x, float y, float z, int tile_id);
EntityID entity_get_player();
EntityData* entity_get(EntityID id);
EntityData* entity_get_array();
int entity_get_count();

void entity_set_position(EntityID id, float x, float y, float z);
void entity_get_position(EntityID id, float* x, float* y, float* z);

void entity_remove(EntityID id);
void entity_clear();

#endif
