#include "player.h"
#include "../tic80.h"
#include "../config.h"
#include "../systems/map.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Entity player = {{32.0f, 32.0f}, 0.0f, 1};

void player_update() {
    Vec2 input = {0, 0};
    if (btn(0)) input.y -= 1; if (btn(1)) input.y += 1;
    if (btn(2)) input.x -= 1; if (btn(3)) input.x += 1;

    // 调试：按 X 键 (btn 6) 重新生成地图
    if (btnp(6,60,6)) {
        map_generate((unsigned int)rand(), true);
    }

    // 调试：按 A 键 (btn 5) 输出当前坐标
    if (btnp(5,30,30)) {
        char coord_str[64];
        sprintf(coord_str, "Player pos: (%.2f, %.2f)", player.pos.x, player.pos.y);
        trace(coord_str, COLOR_WHITE);
    }

    Vec2 move_vec = {0, 0};
    float speed = 0.15f;

    if (input.x != 0 || input.y != 0) {
        float raw_len = sqrtf(input.x * input.x + input.y * input.y);
        input.x /= raw_len; input.y /= raw_len;
        input.y *= 1.4f; // 原版代码的补偿值
        float cos_val = 0.7071f;
        move_vec.x = (input.x + input.y) * cos_val * speed;
        move_vec.y = (input.y - input.x) * cos_val * speed;
    }

    if (move_vec.x != 0) {
        float margin = 0.3f;
        float check_x = player.pos.x + move_vec.x + (move_vec.x > 0 ? margin : -margin);
        if (!is_solid(check_x, player.pos.y)) player.pos.x += move_vec.x;
    }
    if (move_vec.y != 0) {
        float margin = 0.3f;
        float check_y = player.pos.y + move_vec.y + (move_vec.y > 0 ? margin : -margin);
        if (!is_solid(player.pos.x, check_y)) player.pos.y += move_vec.y;
    }


}