#include "player.h"
#include "../../tic80.h"
#include "../../core/config.h"
#include "../../rendering/map/map.h"
#include <math.h>
#include <stdio.h>


Entity player = {{0.0f, 0.0f}, 0.0f, 1};

void player_update() {
    Vec2 input = {0, 0};
    if (btn(0)) input.y -= 1; if (btn(1)) input.y += 1;
    if (btn(2)) input.x -= 1; if (btn(3)) input.x += 1;

    // 调试：按 X 键 (btn 6) 重新生成地图
    if (btnp(6,60,6)) {
        map_generate((unsigned int)tstamp(), true);
        // 实体位置同步已由主循环处理
    }

    // 调试：按 A 键 (btn 5) 输出当前坐标
    // if (btnp(5,30,30)) {
    //     char coord_str[64];
    //     sprintf(coord_str, "Player pos: (%.2f, %.2f)", player.pos.x, player.pos.y);
    //     trace(coord_str, COLOR_WHITE);
    // }

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
        if (!is_solid(check_x, player.pos.y, player.z)) player.pos.x += move_vec.x;
    }
    if (move_vec.y != 0) {
        float margin = 0.3f;
        float check_y = player.pos.y + move_vec.y + (move_vec.y > 0 ? margin : -margin);
        if (!is_solid(player.pos.x, check_y, player.z)) player.pos.y += move_vec.y;
    }

    // Z-axis movement with simple jump physics
    static float vertical_velocity = 0.0f;
    static int debug_timer = 0;
    debug_timer++;
    
    // Check if player is on ground or wall
    int is_grounded = 0;
    int tile_x = (int)player.pos.x;
    int tile_y = (int)player.pos.y;
    
    if (tile_x >= 0 && tile_x < MAP_SIZE && tile_y >= 0 && tile_y < MAP_SIZE) {
        // On ground
        if (player.z <= 0.01f) {
            is_grounded = 1;
        }
        // On wall (platform)
        else if (map_get_tile(tile_x, tile_y) == ID_WALL &&
                 player.z <= WALL_STAND_HEIGHT + 0.1f &&
                 player.z >= WALL_STAND_HEIGHT - 0.1f) {
            is_grounded = 1;
        }
    }
    
    // Jump/fly up with A button
    if (btn(4)) {
        if (is_grounded) {
            // Jump from ground or platform
            vertical_velocity = 0.8f;
        } else {
            // Continue flying up
            vertical_velocity += Z_MOVE_SPEED * 1.0f;
        }
        is_grounded = 0;
    }
    
    // Fast descend with B button
    if (btn(5)) {
        vertical_velocity -= Z_MOVE_SPEED * 1.5f;
        is_grounded = 0;
    }
    
    // Apply gravity - reduced when holding A button for smoother flight
    if (btn(4)) {
        vertical_velocity -= GRAVITY * 0.3f;
    } else {
        vertical_velocity -= GRAVITY;
    }
    
    // Apply vertical velocity
    player.z += vertical_velocity;
    
    // Ground collision
    if (player.z < 0.0f) {
        player.z = 0.0f;
        vertical_velocity = 0.0f;
        is_grounded = 1;
    }
    
    // Wall platform collision
    if (tile_x >= 0 && tile_x < MAP_SIZE && tile_y >= 0 && tile_y < MAP_SIZE) {
        if (map_get_tile(tile_x, tile_y) == ID_WALL) {
            if (player.z > WALL_STAND_HEIGHT - 0.2f && player.z < WALL_STAND_HEIGHT + 0.2f) {
                // Landing on wall platform
                player.z = WALL_STAND_HEIGHT;
                vertical_velocity = 0.0f;
                is_grounded = 1;
            } else if (player.z < WALL_STAND_HEIGHT) {
                // Hit wall from above
                player.z = WALL_STAND_HEIGHT;
                vertical_velocity = 0.0f;
                is_grounded = 1;
            }
        }
    }
    
    // Clamp maximum height
    if (player.z > MAX_Z_HEIGHT) {
        player.z = MAX_Z_HEIGHT;
        vertical_velocity = 0.0f;
    }
    
    // Debug output every 30 frames
    if (debug_timer % 30 == 0) {
        char debug_str[64];
        sprintf(debug_str, "Z=%.2f, v=%.2f, ground=%d", player.z, vertical_velocity, is_grounded);
        trace(debug_str, COLOR_CYAN);
    }
    
    // Damping when in air (optional)
    if (!is_grounded && !btn(4)) {
        vertical_velocity *= 0.98f;
    }
}
