#include "assets.h"
#include "../../tic80.h"
#include "../../core/config.h"
#include <stdlib.h>

static void set_raw_pixel(int tile_id, int x, int y, int color) {
    if(x<0 || x>7 || y<0 || y>7) return;
    int addr = tile_id * 32 + (y * 4) + (x / 2);
    uint8_t* ptr = TILES + addr;
    if (x % 2 == 0) *ptr = (*ptr & 0xF0) | (uint8_t)(color & 0x0F);
    else            *ptr = (*ptr & 0x0F) | (uint8_t)((color & 0x0F) << 4);
}

static void set_composite_pixel(int base_id, int lx, int ly, int color) {
    if (lx < 0 || lx >= 16 || ly < 0 || ly >= 16) return;
    int tx = lx / 8; int ty = ly / 8;
    int real_id = base_id + tx + (ty * 16);
    set_raw_pixel(real_id, lx % 8, ly % 8, color);
}

static void gen_block_16(int id, int c_top, int c_side_l, int c_side_r, int height_px) {
    for(int y=0; y<16; y++) for(int x=0; x<16; x++) {
        int col = COL_TRANSPARENT;
        int dx = abs(x - 7) - (x > 7 ? 1 : 0); 
        bool in_top = false;
        if (y <= 7) {
            int half_w = (y <= 3) ? (y * 2 + 1) : ((7-y) * 2 + 1);
            if (dx <= half_w) in_top = true;
        }
        bool in_side = false;
        if (!in_top && y >= 4 && y < (4 + height_px)) {
             int half_w = 7; if (dx <= half_w) in_side = true;
        }
        if (in_top) {
            col = c_top;
            if (dx == (y <= 3 ? y*2+1 : (7-y)*2+1)) col = COL_HIGHLIGHT; 
        } else if (in_side) {
            col = (x < 8) ? c_side_l : c_side_r;
            if (x == 7 || x == 8) col = c_side_l + 1; 
        }
        if (col != COL_TRANSPARENT) set_composite_pixel(id, x, y, col);
        else set_composite_pixel(id, x, y, COL_TRANSPARENT);
    }
}

void assets_init() {
    gen_block_16(ID_GRASS, COL_GRASS_TOP, COL_GRASS_SIDE, COL_GRASS_SIDE, 4);
    gen_block_16(ID_WALL, COL_WALL_TOP, COL_WALL_SIDE, COL_WALL_SIDE - 1, 12);
    gen_block_16(ID_OUTSIDE, COL_OUT_TOP, COL_OUT_SIDE, COL_OUT_SIDE, 3);
    for(int y=0; y<16; y++) for(int x=0; x<16; x++) {
        set_composite_pixel(ID_PLAYER, x, y, COLOR_BLACK);
        int dx = x - 8, dy = y - 8;
        if (dx*dx + dy*dy < 20) set_composite_pixel(ID_PLAYER, x, y-4, COL_PLAYER);
    }
    
    // 生成红色球
    for(int y=0; y<16; y++) for(int x=0; x<16; x++) {
        set_composite_pixel(ID_RED_BALL, x, y, COLOR_BLACK);
        int dx = x - 8, dy = y - 8;
        if (dx*dx + dy*dy < 25) {
            // 渐变效果：边缘较暗，中心较亮
            int dist = dx*dx + dy*dy;
            if (dist < 6) set_composite_pixel(ID_RED_BALL, x, y, COLOR_WHITE); // 高光
            else if (dist < 16) set_composite_pixel(ID_RED_BALL, x, y, COL_RED_BALL); // 主色
            else set_composite_pixel(ID_RED_BALL, x, y, COLOR_DARK_GREY); // 阴影
        }
    }
}