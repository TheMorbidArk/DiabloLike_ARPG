#include "tic80.h"

// --- 配置 ---
#define MAP_SIZE 64
#define MAX_ENTITIES 50
#define MAX_RENDER_ITEMS 1024 // 增加缓冲区防止崩溃

// Tile ID
#define TID_FLOOR 1
#define TID_WALL_SIDE 2
#define TID_WALL_TOP 3
#define TID_HERO 16
#define TID_ENEMY 18

// 投影参数 (体素风格，紧密排列)
// 8x8 像素的 Sprite，重叠一半绘制
#define ISO_STEP_X 4
#define ISO_STEP_Y 2

// --- 结构体 ---
typedef struct {
    bool active;
    int type; // 0: Player, 1: Enemy
    float x, y; // Map Grid 坐标
    float z;    // 跳跃高度
    int hp;
    int sprite;
    bool flip;
    int anim_timer;
} Entity;

typedef struct {
    int depth_val; // 排序键值
    int y_screen_sort; // 辅助排序
    int sx, sy;
    int sprite_id;
    bool flip;
    int color_override;
} RenderItem;

// --- 全局变量 ---
Entity entities[MAX_ENTITIES];
RenderItem render_queue[MAX_RENDER_ITEMS];
int render_count = 0;
int player_id = 0;
uint8_t trans_colors[] = {0};

// --- 数学与核心 ---

// 紧密等轴测投影
void iso_project(float mx, float my, float mz, int *sx, int *sy) {
    // 核心公式：旋转45度并压扁
    // 这里的 magic numbers 4 和 2 是为了适配 8x8 像素块
    *sx = (int)((mx - my) * ISO_STEP_X);
    *sy = (int)((mx + my) * ISO_STEP_Y) - (int)mz;
}

bool is_solid(float x, float y) {
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) return true;
    return mget((int)x, (int)y) == 1; // 1号地块是墙
}

// --- 渲染系统 ---

void push_render(float mx, float my, float mz, int sprite, bool flip, int color) {
    if (render_count >= MAX_RENDER_ITEMS) return; // 安全检查

    int sx, sy;
    iso_project(mx, my, mz, &sx, &sy);

    RenderItem *it = &render_queue[render_count++];
    it->sx = sx; 
    it->sy = sy;
    it->sprite_id = sprite;
    it->flip = flip;
    it->color_override = color;
    
    // 深度排序关键：
    // 屏幕上 Y 越靠下的物体应该越晚绘制 (遮挡上面的)
    // 在 Iso 视角中，(x+y) 越大，物体越靠前(下)
    it->depth_val = (int)((mx + my) * 10); 
    // 同一位置，Z 越高越晚画 (比如人站在地上，人覆盖地)
    it->y_screen_sort = (int)mz;
}

void flush_render(int cam_offset_x, int cam_offset_y) {
    // 冒泡排序 (性能尚可，保持稳定排序)
    for(int i=0; i<render_count-1; i++) {
        for(int j=0; j<render_count-i-1; j++) {
            RenderItem *a = &render_queue[j];
            RenderItem *b = &render_queue[j+1];
            
            bool swap = false;
            if (a->depth_val > b->depth_val) swap = true;
            else if (a->depth_val == b->depth_val) {
                if (a->y_screen_sort < b->y_screen_sort) swap = true;
            }
            
            if (swap) {
                RenderItem temp = *a;
                *a = *b;
                *b = temp;
            }
        }
    }

    // 绘制
    for(int i=0; i<render_count; i++) {
        RenderItem *it = &render_queue[i];
        int final_x = it->sx + cam_offset_x;
        int final_y = it->sy + cam_offset_y;

        // 简单的视锥剔除
        if (final_x > -8 && final_x < 240 && final_y > -8 && final_y < 136) {
            if (it->color_override > 0) {
                 // 简易受伤闪白：先画精灵，再画个白圈示意
                 spr(it->sprite_id, final_x, final_y, trans_colors, 1, 1, it->flip?1:0, 0, 1, 1);
                 rectb(final_x, final_y, 8, 8, 12);
            } else {
                 spr(it->sprite_id, final_x, final_y, trans_colors, 1, 1, it->flip?1:0, 0, 1, 1);
            }
        }
    }
    render_count = 0;
}

// --- 游戏逻辑 ---

void init_game() {
    // 生成地图数据 (1=Wall, 0=Floor)
    for(int y=0; y<MAP_SIZE; y++) {
        for(int x=0; x<MAP_SIZE; x++) {
            if (x==0 || x==MAP_SIZE-1 || y==0 || y==MAP_SIZE-1 || (x%4==0 && y%4==0)) 
                mset(x, y, 1);
            else 
                mset(x, y, 0);
        }
    }

    // 玩家
    entities[0].active = true;
    entities[0].type = 0;
    entities[0].x = 5.0f; entities[0].y = 5.0f;
    entities[0].sprite = TID_HERO;
    entities[0].hp = 100;
    player_id = 0;

    // 敌人
    entities[1].active = true;
    entities[1].type = 1;
    entities[1].x = 8.0f; entities[1].y = 8.0f;
    entities[1].sprite = TID_ENEMY;
    entities[1].hp = 30;
}

WASM_EXPORT("BOOT")
void BOOT() {
    init_game();
}

WASM_EXPORT("TIC")
void TIC() {
    // 1. 更新逻辑
    Entity *p = &entities[player_id];
    float speed = 0.15f;
    
    // 玩家移动
    float dx = 0, dy = 0;
    if (btn(0)) { dx -= speed; dy -= speed; } // Up
    if (btn(1)) { dx += speed; dy += speed; } // Down
    if (btn(2)) { dx -= speed; dy += speed; p->flip = true; } // Left
    if (btn(3)) { dx += speed; dy -= speed; p->flip = false; }
    
    // 简单的滑动碰撞处理
    if (!is_solid(p->x + dx, p->y)) p->x += dx;
    if (!is_solid(p->x, p->y + dy)) p->y += dy;

    // 攻击测试
    if (btnp(4, -1, -1)) {
        p->anim_timer = 10;
        if (entities[1].active) {
            float ex = entities[1].x - p->x;
            float ey = entities[1].y - p->y;
            if (ex*ex + ey*ey < 4.0f) {
                entities[1].hp -= 10;
                entities[1].anim_timer = 10;
                if(entities[1].hp <= 0) entities[1].active = false;
            }
        }
    }
    if (p->anim_timer > 0) p->anim_timer--;
    if (entities[1].anim_timer > 0) entities[1].anim_timer--;

    // 2. 计算相机中心
    int cam_center_sx, cam_center_sy;
    iso_project(p->x, p->y, 0, &cam_center_sx, &cam_center_sy);
    
    // 锁定屏幕中心 (120, 68)
    int offset_x = 120 - cam_center_sx - 4; 
    int offset_y = 68 - cam_center_sy - 4;

    // 3. 渲染循环
    cls(13); // 深蓝背景
    render_count = 0; 

    // --- 修复点开始 ---
    // 视距范围
    int draw_rad = 14; // 稍微加大一点，防止屏幕边缘突然消失
    
    int min_x = (int)p->x - draw_rad; 
    if(min_x < 0) min_x = 0;
    
    int max_x = (int)p->x + draw_rad; 
    if(max_x >= MAP_SIZE) max_x = MAP_SIZE;
    
    int min_y = (int)p->y - draw_rad; 
    if(min_y < 0) min_y = 0;
    
    int max_y = (int)p->y + draw_rad; 
    if(max_y >= MAP_SIZE) max_y = MAP_SIZE;

    // 正确的循环：y 对比 max_y，x 对比 max_x
    for(int y = min_y; y < max_y; y++) {
        for(int x = min_x; x < max_x; x++) {
            
            // 地板
            push_render((float)x, (float)y, 0, TID_FLOOR, false, 0);

            // 墙壁
            if (mget(x, y) == 1) {
                // Wall Base (z=0)
                push_render((float)x, (float)y, 0, TID_WALL_SIDE, false, 0);
                // Wall Mid (z=4)
                push_render((float)x, (float)y, 4, TID_WALL_SIDE, false, 0);
                // Wall Top (z=8)
                push_render((float)x, (float)y, 8, TID_WALL_TOP, false, 0);
            }
        }
    }
    // --- 修复点结束 ---

    // 实体渲染
    for(int i=0; i<MAX_ENTITIES; i++) {
        if(entities[i].active) {
            push_render(entities[i].x, entities[i].y, 1, 
                       entities[i].sprite, entities[i].flip, 
                       entities[i].anim_timer);
        }
    }

    // 4. 提交绘制
    flush_render(offset_x, offset_y);

    // UI
    print("FIXED VIEW", 5, 5, 12, 0, 1, 0);
}