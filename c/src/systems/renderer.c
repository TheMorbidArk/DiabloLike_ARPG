#include "renderer.h"
#include "../tic80.h"
#include "../config.h"
#include "../utils/math_iso.h"

static uint8_t trans_map[] = {0};

void render_scene(Entity* player) {
    cls(1);
    int draw_dist = 24; 
    int base_x = (int)player->pos.x;
    int base_y = (int)player->pos.y;
    int min_x = base_x - draw_dist;
    int max_x = base_x + draw_dist;
    int min_y = base_y - draw_dist;
    int max_y = base_y + draw_dist + 8;

    for(int y = min_y; y < max_y; y++) {
        for(int x = min_x; x < max_x; x++) {
            bool is_inside = (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE);
            int tid; float z_offset;
            if (is_inside) { tid = mget(x, y); z_offset = 0.0f; }
            else { tid = ID_OUTSIDE; z_offset = -0.5f; }
            if (tid == 0) continue;

            int sx, sy;
            world_to_screen((float)x, (float)y, z_offset, &sx, &sy);
            if (sx < -64 || sx > WIDTH + 64 || sy < -64 || sy > HEIGHT + 64) continue;

            spr(tid, sx, sy, trans_map, 1, 1, 0, 0, 2, 2);
            if (is_inside && tid == ID_WALL) {
                world_to_screen((float)x, (float)y, 0.7f, &sx, &sy); 
                spr(tid, sx, sy, trans_map, 1, 1, 0, 0, 2, 2);
            }
            if ((int)(player->pos.x + 0.5f) == x && (int)(player->pos.y + 0.5f) == y) {
                world_to_screen(player->pos.x, player->pos.y, player->z, &sx, &sy);
                spr(ID_PLAYER, sx, sy - 6, trans_map, 1, 1, 0, 0, 2, 2);
            }
        }
    }
    print("FINAL ENGINE MODULAR", 2, 2, 12, 0, 1, 0);
}