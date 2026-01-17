#include "entity_manager.h"

static EntityData entities[MAX_ENTITIES];
static int entity_count = 0;

void entity_manager_init() {
    entity_count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        entities[i].type = ENTITY_TYPE_PLAYER;
        entities[i].entity.pos.x = 0;
        entities[i].entity.pos.y = 0;
        entities[i].entity.z = 0;
        entities[i].tile_id = 0;
    }
}

EntityID entity_add(EntityType type, float x, float y, float z, int tile_id) {
    if (entity_count >= MAX_ENTITIES) return -1;
    
    entities[entity_count].type = type;
    entities[entity_count].entity.pos.x = x;
    entities[entity_count].entity.pos.y = y;
    entities[entity_count].entity.z = z;
    entities[entity_count].tile_id = tile_id;
    
    return entity_count++;
}

EntityID entity_get_player() {
    return ENTITY_ID_PLAYER;
}

EntityData* entity_get(EntityID id) {
    if (id >= 0 && id < entity_count) {
        return &entities[id];
    }
    return (EntityData*)0;
}

EntityData* entity_get_array() {
    return entities;
}

int entity_get_count() {
    return entity_count;
}

void entity_set_position(EntityID id, float x, float y, float z) {
    if (id >= 0 && id < entity_count) {
        entities[id].entity.pos.x = x;
        entities[id].entity.pos.y = y;
        entities[id].entity.z = z;
    }
}

void entity_get_position(EntityID id, float* x, float* y, float* z) {
    if (id >= 0 && id < entity_count) {
        *x = entities[id].entity.pos.x;
        *y = entities[id].entity.pos.y;
        *z = entities[id].entity.z;
    }
}

void entity_remove(EntityID id) {
    if (id >= 0 && id < entity_count) {
        entities[id].type = ENTITY_TYPE_PLAYER;
        entities[id].tile_id = 0;
        entities[id].entity.pos.x = -1.0f;
        entities[id].entity.pos.y = -1.0f;
        entities[id].entity.z = 0.0f;
    }
}

void entity_clear() {
    entity_count = 0;
}
