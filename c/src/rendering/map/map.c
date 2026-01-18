#include "map.h"
#include "../../core/config.h"
#include "../../tic80.h"
#include "../../core/entity/entity_manager.h"
#include <stdlib.h>
#include <string.h>

// 辅助函数：安全获取地图块，越界视为墙（为了封闭迷宫）
static int safe_mget(int x, int y) {
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) return ID_WALL;
    return mget(x, y);
}

// 上下文保存/恢复
void map_save_scene(MapScene* state) {
    memcpy(state->tiles, MAP, MAP_SIZE * MAP_SIZE);
}

void map_restore_scene(const MapScene* state) {
    memcpy(MAP, state->tiles, MAP_SIZE * MAP_SIZE);
}

void map_init() {
    memset(MAP, 0, 32768);
}

void map_generate(unsigned int seed, bool generate_walls) {
    srand(seed);
    // 使用静态数组或在栈外分配，避免 WASM 栈溢出
    static uint8_t temp_map[MAP_SIZE * MAP_SIZE];

    // --- 第一步：随机填充噪声 (迷宫需要更高的初始墙体比例) ---
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            // 边缘强制设为墙，形成封闭迷宫
            if (x == 0 || x == MAP_SIZE - 1 || y == 0 || y == MAP_SIZE - 1) {
                map_set_tile(x, y, ID_WALL);
            } else {
                // 如果 generate_walls 为 false，全部设为草地（用于测试）
                if (!generate_walls) {
                    map_set_tile(x, y, ID_GRASS);
                } else {
                    // 48% 的墙体比例通常能产生较好的洞穴迷宫效果
                    map_set_tile(x, y, (rand() % 100 < 48) ? ID_WALL : ID_GRASS);
                }
            }
        }
    }

    // --- 第二步：细胞自动机迭代 (运行 5 轮以平滑走廊) ---
    for (int iter = 0; iter < 5; iter++) {
        for (int y = 1; y < MAP_SIZE - 1; y++) {
            for (int x = 1; x < MAP_SIZE - 1; x++) {
                int walls = 0;
                for (int yy = -1; yy <= 1; yy++) {
                    for (int xx = -1; xx <= 1; xx++) {
                        if (safe_mget(x + xx, y + yy) == ID_WALL) walls++;
                    }
                }
                // 经典 4-5 规则：周围墙多则变墙，周围墙少则变草地（走廊）
                if (walls >= 5) temp_map[y * MAP_SIZE + x] = ID_WALL;
                else temp_map[y * MAP_SIZE + x] = ID_GRASS;
            }
        }
        // 同步回地图
        for (int y = 1; y < MAP_SIZE - 1; y++) {
            for (int x = 1; x < MAP_SIZE - 1; x++) {
                map_set_tile(x, y, temp_map[y * MAP_SIZE + x]);
            }
        }
    }

    // --- 第三步：BFS 确保全连通走廊，同时保护所有实体位置 ---
    // 访问标记位图
    memset(temp_map, 0, sizeof(temp_map));
    
    // 准备 BFS 队列
    static int qx[MAP_SIZE * MAP_SIZE];
    static int qy[MAP_SIZE * MAP_SIZE];
    int head = 0, tail = 0;

    // 获取所有实体
    EntityData* entities = entity_get_array();
    int entity_count = entity_get_count();

    // 为所有实体的3x3区域添加到BFS起点，确保连通性
    for (int e = 0; e < entity_count; e++) {
        int entity_x = (int)entities[e].entity.pos.x;
        int entity_y = (int)entities[e].entity.pos.y;
        
        // 强制实体和周围为草地
        for(int dy = -1; dy <= 1; dy++) {
            for(int dx = -1; dx <= 1; dx++) {
                int tx = entity_x + dx;
                int ty = entity_y + dy;
                if (tx >= 0 && tx < MAP_SIZE && ty >= 0 && ty < MAP_SIZE) {
                    map_set_tile(tx, ty, ID_GRASS);
                    
                    // 如果这个格子还未访问，添加到BFS队列
                    if (!temp_map[ty * MAP_SIZE + tx]) {
                        qx[tail] = tx;
                        qy[tail] = ty;
                        tail++;
                        temp_map[ty * MAP_SIZE + tx] = 1;
                    }
                }
            }
        }
    }

    while (head < tail) {
        int cx = qx[head];
        int cy = qy[head];
        head++;

        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (nx > 0 && nx < MAP_SIZE-1 && ny > 0 && ny < MAP_SIZE-1) {
                if (!temp_map[ny * MAP_SIZE + nx] && mget(nx, ny) == ID_GRASS) {
                    temp_map[ny * MAP_SIZE + nx] = 1;
                    qx[tail] = nx;
                    qy[tail] = ny;
                    tail++;
                }
            }
        }
    }

    // --- 第四步：消除死路和孤立空间 ---
    // 将所有无法到达的草地全部填成墙
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            if (map_get_tile(x, y) == ID_GRASS && !temp_map[y * MAP_SIZE + x]) {
                map_set_tile(x, y, ID_WALL);
            }
        }
    }

    // --- 第五步：确保实体区域与主连通区域保持连接 ---
    // 注意：在第三步中已经将所有实体的3x3区域都加入BFS，
    // 所以它们现在应该都是连通的。这里不需要额外操作。
}
bool map_is_inside(int x, int y) {
    return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
}

bool is_solid(float x, float y, float z) {
    int ix = (int)x; int iy = (int)y;
    if (ix >= 0 && ix < MAP_SIZE && iy >= 0 && iy < MAP_SIZE) {
        if (map_get_tile(ix, iy) == ID_WALL) {
            // 检查玩家Z高度是否低于墙体站立高度
            return z < WALL_STAND_HEIGHT;
        }
    }
    return false;
}

int map_get_tile(int x, int y) {
    if (!map_is_inside(x, y)) return ID_WALL;
    return mget(x, y);
}

void map_set_tile(int x, int y, int tile_id) {
    if (map_is_inside(x, y)) {
        mset(x, y, tile_id);
    }
}