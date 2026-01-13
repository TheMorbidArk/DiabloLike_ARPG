#include "tic80.h"
#include <math.h>   // 用于 sqrtf, fabs
#include <stdlib.h> // 用于 abs

// ==========================================
// 1. 全局配置与常量
// ==========================================

#define MAP_SIZE 32     // 逻辑地图大小 (界内区域)
#define TILE_SIZE 8     

// 等轴测网格步长 (16宽, 8高)
#define ISO_W 16        
#define ISO_H 8         

// 摄像机跟随速度 (0.0 ~ 1.0)
// X轴稍快，Y轴稍慢，以适应视觉比例，手感更顺滑
float cam_speed_x = 0.15f; 
float cam_speed_y = 0.12f; 

// 资源 ID 定义 (16+ 为程序化生成的组合图块)
#define ID_EMPTY     0
#define ID_GRASS     16  // 界内地面
#define ID_WALL      18  // 界内墙壁
#define ID_OUTSIDE   20  // 界外地板 (无限填充)
#define ID_PLAYER    256 // 玩家 Sprite (位于 Bank 1)

// 调色板颜色索引
#define COL_TRANSPARENT 0
#define COL_HIGHLIGHT   12 // 高光
#define COL_GRASS_TOP   6  // 浅绿
#define COL_GRASS_SIDE  5  // 深绿
#define COL_WALL_TOP    15 // 白墙
#define COL_WALL_SIDE   13 // 灰墙
#define COL_OUT_TOP     2  // 界外深紫
#define COL_OUT_SIDE    1  // 界外暗紫
#define COL_PLAYER      14 // 玩家色

// ==========================================
// 2. 数据结构
// ==========================================

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    Vec2 pos;   // 逻辑位置 (Grid Coordinates)
    float z;    // 高度
    int dir;    // 方向
} Entity;

// 玩家初始化
Entity player = {{16.0f, 16.0f}, 0.0f, 1};

// 摄像机位置
float cam_x = 0, cam_y = 0;

// 透明映射
uint8_t trans_map[] = {0};

// ==========================================
// 3. 资产生成工厂 (Asset Factory)
// ==========================================

void set_raw_pixel(int tile_id, int x, int y, int color) {
    if(x<0 || x>7 || y<0 || y>7) return;
    int addr = tile_id * 32 + (y * 4) + (x / 2);
    uint8_t* ptr = TILES + addr;
    if (x % 2 == 0) *ptr = (*ptr & 0xF0) | (uint8_t)(color & 0x0F);
    else            *ptr = (*ptr & 0x0F) | (uint8_t)((color & 0x0F) << 4);
}

void set_composite_pixel(int base_id, int lx, int ly, int color) {
    if (lx < 0 || lx >= 16 || ly < 0 || ly >= 16) return;
    int tx = lx / 8;
    int ty = ly / 8;
    int real_id = base_id + tx + (ty * 16);
    int sub_x = lx % 8;
    int sub_y = ly % 8;
    set_raw_pixel(real_id, sub_x, sub_y, color);
}

// 生成等轴测菱形方块
void gen_block_16(int id, int c_top, int c_side_l, int c_side_r, int height_px) {
    for(int y=0; y<16; y++)
        for(int x=0; x<16; x++)
            set_composite_pixel(id, x, y, COL_TRANSPARENT);

    for(int y=0; y<16; y++) {
        for(int x=0; x<16; x++) {
            int col = COL_TRANSPARENT;
            int dx = abs(x - 7) - (x > 7 ? 1 : 0); 
            
            bool in_top = false;
            if (y <= 7) {
                int half_w = (y <= 3) ? (y * 2 + 1) : ((7-y) * 2 + 1);
                if (dx <= half_w) in_top = true;
            }

            bool in_side = false;
            if (!in_top && y >= 4 && y < (4 + height_px)) {
                 int half_w = 7; 
                 if (dx <= half_w) in_side = true;
            }

            if (in_top) {
                col = c_top;
                if (dx == (y <= 3 ? y*2+1 : (7-y)*2+1)) col = COL_HIGHLIGHT; 
            } else if (in_side) {
                col = (x < 8) ? c_side_l : c_side_r;
                if (x == 7 || x == 8) col = c_side_l + 1; 
            }

            if (col != COL_TRANSPARENT) set_composite_pixel(id, x, y, col);
        }
    }
}

void gen_player_16() {
    for(int y=0; y<16; y++)
        for(int x=0; x<16; x++) {
            set_composite_pixel(ID_PLAYER, x, y, 0); 
            int dx = x - 8;
            int dy = y - 8;
            if (dx*dx + dy*dy < 20) 
                set_composite_pixel(ID_PLAYER, x, y-4, COL_PLAYER);
        }
}

// ==========================================
// 4. 核心数学 (Math)
// ==========================================

// 坐标转换: Grid -> Screen
// 注意：这个函数主要用于渲染，因为它包含了cam偏移
void world_to_screen(float wx, float wy, float wz, int* sx, int* sy) {
    *sx = (int)((wx - wy) * (ISO_W / 2)) + (WIDTH / 2);
    *sy = (int)((wx + wy) * (ISO_H / 2));
    *sy -= (int)(wz * 16.0f); 
    *sx -= (int)cam_x;
    *sy -= (int)cam_y;
    *sx -= 8; // Sprite Center Offset
    *sy -= 4; 
}

// 碰撞检测
bool is_solid(float x, float y) {
    int ix = (int)x;
    int iy = (int)y;
    // 界外无碰撞，界内查ID
    if (ix >= 0 && ix < MAP_SIZE && iy >= 0 && iy < MAP_SIZE) {
        if (mget(ix, iy) == ID_WALL) return true;
    }
    return false;
}

// ==========================================
// 5. 生命周期 (Life Cycle)
// ==========================================

WASM_EXPORT("BOOT")
void BOOT() {
    // 生成资源
    gen_block_16(ID_GRASS, COL_GRASS_TOP, COL_GRASS_SIDE, COL_GRASS_SIDE, 4);
    gen_block_16(ID_WALL, COL_WALL_TOP, COL_WALL_SIDE, COL_WALL_SIDE - 1, 12);
    gen_block_16(ID_OUTSIDE, COL_OUT_TOP, COL_OUT_SIDE, COL_OUT_SIDE, 3);
    gen_player_16();

    // 生成地图
    for(int y=0; y<MAP_SIZE; y++) {
        for(int x=0; x<MAP_SIZE; x++) {
            int t = ID_GRASS;
            // 边缘围墙
            if (x == 0 || x == MAP_SIZE-1 || y == 0 || y == MAP_SIZE-1) t = ID_WALL;
            // 内部随机柱子
            else if ((x * y) % 11 == 0 && (x+y)%2 == 0) t = ID_WALL;
            mset(x, y, t);
        }
    }
}

WASM_EXPORT("TIC")
void TIC() {
    // -----------------------------------------------------------
    // 1. 输入与移动逻辑 (Vectorized Movement)
    // -----------------------------------------------------------
    Vec2 input = {0, 0};
    if (btn(0)) input.y -= 1; // 上
    if (btn(1)) input.y += 1; // 下
    if (btn(2)) input.x -= 1; // 左
    if (btn(3)) input.x += 1; // 右

    Vec2 move_vec = {0, 0};
    float speed = 0.15f;

    if (input.x != 0 || input.y != 0) {
        // A. 归一化输入方向
        float raw_len = sqrtf(input.x * input.x + input.y * input.y);
        if (raw_len > 0) {
            input.x /= raw_len;
            input.y /= raw_len;
        }

        // B. 【视觉补偿】各向异性速度
        // Y轴逻辑速度翻倍，以抵消视觉上的压缩
        input.x *= 1.0f;
        input.y *= 1.4f;

        // C. 旋转矩阵 (Screen Space -> Grid Space, -45 deg)
        float cos_val = 0.7071f;
        move_vec.x = (input.x + input.y) * cos_val;
        move_vec.y = (input.y - input.x) * cos_val;

        // D. 应用速度
        move_vec.x *= speed;
        move_vec.y *= speed;
    }

    // -----------------------------------------------------------
    // 2. 物理碰撞 (Sliding Physics)
    // -----------------------------------------------------------
    
    // X 轴独立移动
    if (move_vec.x != 0) {
        float margin = 0.3f; 
        float check_x = player.pos.x + move_vec.x + (move_vec.x > 0 ? margin : -margin);
        if (!is_solid(check_x, player.pos.y)) {
            player.pos.x += move_vec.x;
        }
    }

    // Y 轴独立移动
    if (move_vec.y != 0) {
        float margin = 0.3f;
        float check_y = player.pos.y + move_vec.y + (move_vec.y > 0 ? margin : -margin);
        if (!is_solid(player.pos.x, check_y)) {
            player.pos.y += move_vec.y;
        }
    }

    // 方向更新
    if (input.y < -0.1f) player.dir = 0;
    else if (input.y > 0.1f) player.dir = 1;
    else if (input.x < -0.1f) player.dir = 2;
    else if (input.x > 0.1f) player.dir = 3;

    // -----------------------------------------------------------
    // 3. 摄像机跟随 (Camera Follow) - 高精度逻辑
    // -----------------------------------------------------------
    
    // A. 计算绝对投影坐标 (不含摄像机偏移)
    float target_iso_x = (player.pos.x - player.pos.y) * (ISO_W / 2.0f);
    float target_iso_y = (player.pos.x + player.pos.y) * (ISO_H / 2.0f);
    
    // B. 应用高度和屏幕中心修正
    target_iso_y -= (player.z * 16.0f);
    target_iso_x += (WIDTH / 2.0f);
    
    // C. 对齐到 Sprite 中心 (-8, -4 是 world_to_screen 里的偏移)
    // 我们希望 Camera 锁定的是 Sprite 的身体中心
    float abs_target_x = target_iso_x - 8.0f;
    float abs_target_y = target_iso_y - 4.0f;

    // D. 计算理想摄像机位置 (让玩家居中)
    float ideal_cam_x = abs_target_x - (WIDTH / 2.0f);
    float ideal_cam_y = abs_target_y - (HEIGHT / 2.0f);

    // E. 独立轴平滑插值
    cam_x += (ideal_cam_x - cam_x) * cam_speed_x;
    cam_y += (ideal_cam_y - cam_y) * cam_speed_y;

    // -----------------------------------------------------------
    // 4. 渲染管线 (Render Pipeline)
    // -----------------------------------------------------------
    cls(1); // 背景色

    // 动态渲染范围
    int draw_dist = 24; 
    
    // 基于摄像机位置反推 Grid 区域 (近似)
    int base_x = (int)player.pos.x;
    int base_y = (int)player.pos.y;

    int min_x = base_x - draw_dist;
    int max_x = base_x + draw_dist;
    int min_y = base_y - draw_dist;
    int max_y = base_y + draw_dist + 8; // Y轴多画几行

    for(int y = min_y; y < max_y; y++) {
        for(int x = min_x; x < max_x; x++) {
            
            bool is_inside = (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE);
            int tid;
            float z_offset;

            if (is_inside) {
                // 界内
                tid = mget(x, y);
                z_offset = 0.0f;
            } else {
                // 界外：无限地板，下沉效果
                tid = ID_OUTSIDE;
                z_offset = -0.5f; 
            }

            if (tid == 0) continue;

            int sx, sy;
            world_to_screen((float)x, (float)y, z_offset, &sx, &sy);
            
            // 视口剔除 (宽容度)
            if (sx < -64 || sx > WIDTH + 64 || sy < -64 || sy > HEIGHT + 64) continue;

            // 绘制地块
            spr(tid, sx, sy, trans_map, 1, 1, 0, 0, 2, 2);

            // 绘制墙壁立体部分
            if (is_inside && tid == ID_WALL) {
                world_to_screen((float)x, (float)y, 0.7f, &sx, &sy); 
                spr(tid, sx, sy, trans_map, 1, 1, 0, 0, 2, 2);
            }

            // 绘制玩家 (深度排序)
            if ((int)(player.pos.x + 0.5f) == x && (int)(player.pos.y + 0.5f) == y) {
                world_to_screen(player.pos.x, player.pos.y, player.z, &sx, &sy);
                spr(ID_PLAYER, sx, sy - 6, trans_map, 1, 1, 0, 0, 2, 2);
            }
        }
    }
    
    print("FINAL ENGINE", 2, 2, 12, 0, 1, 0);
}