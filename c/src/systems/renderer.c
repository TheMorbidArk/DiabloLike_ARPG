#include "renderer.h"
#include "../tic80.h"
#include "../config.h"
#include "../utils/math_iso.h"
#include "../systems/map.h"

static uint8_t trans_map[] = {0};

void render_scene(Entity* player) {
    cls(0); 
    uint8_t trans = 0;
    
    int range = 12;
    int px = (int)player->pos.x;
    int py = (int)player->pos.y;

    for(int y = py - range; y <= py + range; y++) {
        for(int x = px - range; x <= px + range; x++) {
            if (!map_is_inside(x, y)) continue;

            int tid = mget(x, y);
            if (tid == 0) continue;

            int sx, sy;
            world_to_screen((float)x, (float)y, 0.0f, &sx, &sy);
            
            if (sx < -16 || sx > WIDTH || sy < -16 || sy > HEIGHT) continue;

            // --- 关键修改：区分地板和墙的绘制 ---
            if (tid == ID_WALL) {
                // 1. 如果是墙，通常需要先在底下垫一个地板，防止墙体透明部分露出黑底
                // spr(ID_GRASS, sx, sy, &trans, 1, 1, 0, 0, 2, 2); 

                // 2. 将墙的渲染位置向上抬起 8 像素（一个高度单位）
                // 这样它的底部才会正好落在草地的位置上
                spr(tid, sx, sy - 8, &trans, 1, 1, 0, 0, 2, 2);
            } else {
                // 普通地板，直接按原始坐标画
                spr(tid, sx, sy, &trans, 1, 1, 0, 0, 2, 2);
            }

            // --- 渲染玩家 ---
            if (px == x && py == y) {
                int psx, psy;
                world_to_screen(player->pos.x, player->pos.y, player->z, &psx, &psy);
                // 玩家也需要向上抬起，否则会埋进土里
                // 假设玩家精灵高16像素，抬起 6-8 像素比较合适
                spr(ID_PLAYER, psx, psy - 8, &trans, 1, 1, 0, 0, 2, 2);
            }
        }
    }
}