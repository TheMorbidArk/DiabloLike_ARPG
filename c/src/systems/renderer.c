#include "renderer.h"
#include "../tic80.h"
#include "../config.h"
#include "../utils/math_iso.h"
#include "../systems/map.h"
#include "../entities/entity_manager.h"
#include "../systems/camera.h"
#include <stdio.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))



static uint8_t trans_map[] = {0};

void render_minimap(Entity* player) {
    // 1. 小地图 UI 配置 (64x32)
    const int MM_W = 64;
    const int MM_H = 32;
    const int offset_x = WIDTH - MM_W - 8; // 稍微多留一点边距
    const int offset_y = 8;

    int cx = offset_x + MM_W / 2;
    int cy = offset_y + MM_H / 2;

    // 2. 渲染参数
    const int RADIUS = 18;
    int px = (int)player->pos.x;
    int py = (int)player->pos.y;

    // 3. 投影循环（内容层）
    for (int dy = -RADIUS; dy <= RADIUS; dy++) {
        for (int dx = -RADIUS; dx <= RADIUS; dx++) {
            int wx = px + dx;
            int wy = py + dy;

            if (wx >= 0 && wx < 96 && wy >= 0 && wy < 96) {
                uint8_t tid = (uint8_t)map_get_tile(wx, wy);
                if (tid == 0) continue;

                int mm_rel_x = (dx - dy);
                int mm_rel_y = (dx + dy) / 2;

                // --- 裁剪逻辑优化 ---
                // 稍微收紧裁剪范围（MM_W - 4），确保 2x2 的色块不会溢出到边框外
                if (ABS(mm_rel_x) + ABS(dx + dy) >= MM_W - 4) continue;

                int final_sx = cx + mm_rel_x;
                int final_sy = cy + mm_rel_y;

                uint8_t col = (tid == ID_WALL) ? COLOR_PURPLE : COLOR_ORANGE;
                rect(final_sx, final_sy, 2, 2, (int8_t)col);
            }
        }
    }

    // 4. 【关键修复】：扩大边框顶点坐标
    // 向外扩 2 像素水平，1 像素垂直，形成“包裹”感
    float b_top_y = (float)offset_y - 2.0f;
    float b_bottom_y = (float)(offset_y + MM_H) + 2.0f;
    float b_left_x = (float)offset_x - 4.0f;
    float b_right_x = (float)(offset_x + MM_W) + 6.0f;
    float mid_x = (float)cx;
    float mid_y = (float)cy;

    // 使用颜色 12 (或 15 白色) 绘制包裹线
    line(mid_x, b_top_y, b_right_x, mid_y, COLOR_WHITE);     // 右上
    line(b_right_x, mid_y, mid_x, b_bottom_y, COLOR_WHITE); // 右下
    line(mid_x, b_bottom_y, b_left_x, mid_y, COLOR_WHITE);  // 左下
    line(b_left_x, mid_y, mid_x, b_top_y, COLOR_WHITE);     // 左上

    // 5. 绘制玩家点 (最顶层)
    rect(cx - 1, cy - 1, 2, 2, COLOR_DARK_GREY);
}

void render_scene(Entity* player) {
    // 渲染场景（由调用者控制何时调用）

    cls(COLOR_BLACK);
    uint8_t trans = 0;

    // 获取相机位置
    Camera* cam = camera_get_main();
    float cam_x, cam_y;
    camera_get_position(cam, &cam_x, &cam_y);

    // 根据相机位置计算屏幕边界对应的世界坐标范围
    // 使用保守的估算来确保覆盖所有可见区域
    // 增加范围以确保相机漫游到边界时能看到足够的地图内容
    int range_x = 50;  // 水平方向更大的覆盖范围
    int range_y = 45;  // 垂直方向更大的覆盖范围

    // 计算相机中心对应的等轴坐标
    float center_iso_x = cam_x + WIDTH / 2.0f;
    float center_iso_y = cam_y + HEIGHT / 2.0f;

    // 反推相机中心大致对应的世界坐标
    float center_wx = (center_iso_x / (ISO_W / 2.0f) + center_iso_y / (ISO_H / 2.0f)) / 2.0f;
    float center_wy = (center_iso_y / (ISO_H / 2.0f) - center_iso_x / (ISO_W / 2.0f)) / 2.0f;

    int cx = (int)center_wx;
    int cy = (int)center_wy;

    for(int y = cy - range_y; y <= cy + range_y; y++) {
        for(int x = cx - range_x; x <= cx + range_x; x++) {
            int tid;
            if (!map_is_inside(x, y)) {
                // 地图边界外使用外部地板
                tid = ID_OUTSIDE;
            } else {
                tid = map_get_tile(x, y);
                if (tid == 0) continue;
            }

            int sx, sy;
            world_to_screen((float)x, (float)y, 0.0f, &sx, &sy);

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
            int player_tile_x = (int)player->pos.x;
            int player_tile_y = (int)player->pos.y;
            if (player_tile_x == x && player_tile_y == y) {
                int psx, psy;
                world_to_screen(player->pos.x, player->pos.y, player->z, &psx, &psy);
                // 玩家也需要向上抬起，否则会埋进土里
                // 假设玩家精灵高16像素，抬起 6-8 像素比较合适
                spr(ID_PLAYER, psx, psy - 8, &trans, 1, 1, 0, 0, 2, 2);
            }

            // --- 渲染游戏实体 ---
            EntityData* entities = entity_get_array();
            int entity_count = entity_get_count();

            for (int e = 0; e < entity_count; e++) {
                int entity_x = (int)entities[e].entity.pos.x;
                int entity_y = (int)entities[e].entity.pos.y;

                if (entity_x == x && entity_y == y && entities[e].type != ENTITY_TYPE_PLAYER) {
                    int esx, esy;
                    world_to_screen(entities[e].entity.pos.x, entities[e].entity.pos.y, entities[e].entity.z, &esx, &esy);
                    // 实体需要向上抬起，与玩家保持一致的高度
                    spr(entities[e].tile_id, esx, esy - 8, &trans, 1, 1, 0, 0, 2, 2);
                }
            }
        }
    }
    render_minimap(player);
    render_altimeter(player);
}

void render_altimeter(Entity* player) {
    // 渲染高度计（由调用者控制何时调用）

    // 绘制高度计背景框
    rect(ALTIMETER_X, ALTIMETER_Y,
         ALTIMETER_WIDTH, ALTIMETER_HEIGHT,
         (int8_t)ALTIMETER_BG_COLOR);

    // 计算填充高度（基于当前Z值）
    float z_percentage = player->z / MAX_Z_HEIGHT;
    int fill_height = (int)(z_percentage * (float)(ALTIMETER_HEIGHT - 4));

    if (fill_height > 0) {
        // 绘制填充部分（从底部开始）
        int fill_y = ALTIMETER_Y + ALTIMETER_HEIGHT - 2 - fill_height;
        rect(ALTIMETER_X + 2, fill_y,
             ALTIMETER_WIDTH - 4, fill_height,
             (int8_t)ALTIMETER_FILL_COLOR);
    }

    // 绘制边框
    rectb(ALTIMETER_X, ALTIMETER_Y,
          ALTIMETER_WIDTH, ALTIMETER_HEIGHT,
          (int8_t)ALTIMETER_BORDER_COLOR);

    // 显示当前Z值文本
    char z_str[16];
    sprintf(z_str, "%.1f", player->z);
    print(z_str, ALTIMETER_X, ALTIMETER_Y + ALTIMETER_HEIGHT + 2,
          (int8_t)COLOR_WHITE, 1, 1, 0);
}
