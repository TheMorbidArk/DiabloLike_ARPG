#include "map.h"
#include "../config.h"
#include "../tic80.h"
#include <stdlib.h>
#include <string.h>

// 辅助函数：安全获取地图块，越界视为墙（为了封闭迷宫）
static int safe_mget(int x, int y) {
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) return ID_WALL;
    return mget(x, y);
}

void map_init() {
    memset(MAP, 0, 32768); 
}

void map_generate(unsigned int seed) {
    srand(seed);
    // 使用静态数组或在栈外分配，避免 WASM 栈溢出
    static uint8_t temp_map[MAP_SIZE * MAP_SIZE];

    // --- 第一步：随机填充噪声 (迷宫需要更高的初始墙体比例) ---
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            // 边缘强制设为墙，形成封闭迷宫
            if (x == 0 || x == MAP_SIZE - 1 || y == 0 || y == MAP_SIZE - 1) {
                mset(x, y, ID_WALL);
            } else {
                // 48% 的墙体比例通常能产生较好的洞穴迷宫效果
                mset(x, y, (rand() % 100 < 48) ? ID_WALL : ID_GRASS);
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
                mset(x, y, temp_map[y * MAP_SIZE + x]);
            }
        }
    }

    // --- 第三步：BFS 确保全连通走廊 ---
    int start_x = MAP_SIZE / 2;
    int start_y = MAP_SIZE / 2;
    
    // 强制起点和周围为草地
    for(int i=-1; i<=1; i++) for(int j=-1; j<=1; j++) mset(start_x+i, start_y+j, ID_GRASS);

    // 访问标记位图
    memset(temp_map, 0, sizeof(temp_map));
    
    // 准备 BFS 队列
    static int qx[MAP_SIZE * MAP_SIZE];
    static int qy[MAP_SIZE * MAP_SIZE];
    int head = 0, tail = 0;

    qx[tail] = start_x;
    qy[tail] = start_y;
    tail++;
    temp_map[start_y * MAP_SIZE + start_x] = 1;

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
            if (mget(x, y) == ID_GRASS && !temp_map[y * MAP_SIZE + x]) {
                mset(x, y, ID_WALL); 
            }
        }
    }
}
bool map_is_inside(int x, int y) {
    return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
}

bool is_solid(float x, float y) {
    int ix = (int)x; int iy = (int)y;
    if (ix >= 0 && ix < MAP_SIZE && iy >= 0 && iy < MAP_SIZE) {
        if (mget(ix, iy) == ID_WALL) return true;
    }
    return false;
}