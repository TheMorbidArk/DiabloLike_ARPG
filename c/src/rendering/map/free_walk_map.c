#include "free_walk_map.h"
#include "../../core/config.h"
#include "../../tic80.h"
#include "../../utils/math_iso.h"
#include "../../core/entity/entity_manager.h"
#include "../camera/camera.h"
#include "../renderer/renderer.h"
#include <string.h>
#include <stdio.h>

void free_walk_map_init(FreeWalkMap* map) {
    memset(map->tiles, 0, FREE_WALK_MAP_SIZE);
}

void free_walk_map_generate(FreeWalkMap* map) {
    for (int y = 0; y < FREE_WALK_MAP_HEIGHT; y++) {
        for (int x = 0; x < FREE_WALK_MAP_WIDTH; x++) {
            if (x == 0 || x == FREE_WALK_MAP_WIDTH - 1 || y == 0 || y == FREE_WALK_MAP_HEIGHT - 1) {
                free_walk_map_set_tile(map, x, y, ID_WALL);
            } else {
                free_walk_map_set_tile(map, x, y, ID_GRASS);
            }
        }
    }
}

int free_walk_map_get_tile(const FreeWalkMap* map, int x, int y) {
    if (x < 0 || x >= FREE_WALK_MAP_WIDTH || y < 0 || y >= FREE_WALK_MAP_HEIGHT) {
        return ID_WALL;  // 越界视为墙
    }
    return map->tiles[y * FREE_WALK_MAP_WIDTH + x];
}

void free_walk_map_set_tile(FreeWalkMap* map, int x, int y, int tile_id) {
    if (x >= 0 && x < FREE_WALK_MAP_WIDTH && y >= 0 && y < FREE_WALK_MAP_HEIGHT) {
        map->tiles[y * FREE_WALK_MAP_WIDTH + x] = (uint8_t)tile_id;
    }
}

bool free_walk_map_is_solid(const FreeWalkMap* map, float x, float y, float z) {
    int ix = (int)x;
    int iy = (int)y;
    if (ix >= 0 && ix < FREE_WALK_MAP_WIDTH && iy >= 0 && iy < FREE_WALK_MAP_HEIGHT) {
        if (free_walk_map_get_tile(map, ix, iy) == ID_WALL) {
            return z < WALL_STAND_HEIGHT;
        }
    }
    return false;
}

void free_walk_map_render(const FreeWalkMap* map, Entity* player) {
    cls(COLOR_BLACK);
    uint8_t trans = 0;

    // 获取相机位置
    Camera* cam = camera_get_main();
    float cam_x, cam_y;
    camera_get_position(cam, &cam_x, &cam_y);

    // 计算渲染范围（简化版，基于相机中心）
    int range_x = 20;
    int range_y = 25;
    float center_wx = cam_x + WIDTH / 2.0f;
    float center_wy = cam_y + HEIGHT / 2.0f;
    int cx = (int)center_wx;
    int cy = (int)center_wy;

    for (int y = cy - range_y; y <= cy + range_y; y++) {
        for (int x = cx - range_x; x <= cx + range_x; x++) {
            int tid;
            if (x < 0 || x >= FREE_WALK_MAP_WIDTH || y < 0 || y >= FREE_WALK_MAP_HEIGHT) {
                tid = ID_OUTSIDE;
            } else {
                tid = free_walk_map_get_tile(map, x, y);
                if (tid == 0) continue;
            }

            // 调试：输出渲染的tile
            if (tid == ID_GRASS && x == 8 && y == 48) {  // 只在玩家位置调试
                char debug_msg[64];
                sprintf(debug_msg, "Rendering grass at (%d, %d)", x, y);
                trace(debug_msg, COLOR_WHITE);
            }

            int sx, sy;
            world_to_screen((float)x, (float)y, 0.0f, &sx, &sy);

            // 渲染地板或墙
            spr(tid, sx, sy, &trans, 1, 1, 0, 0, 2, 2);

            // 渲染玩家
            int player_tile_x = (int)player->pos.x;
            int player_tile_y = (int)player->pos.y;
            if (player_tile_x == x && player_tile_y == y) {
                int psx, psy;
                world_to_screen(player->pos.x, player->pos.y, player->z, &psx, &psy);
                spr(ID_PLAYER, psx, psy - 8, &trans, 1, 1, 0, 0, 2, 2);
            }

            // 渲染实体（简化，无实体）
        }
    }

    // 渲染高度计
    render_altimeter(player);
}