#ifndef CONFIG_H
#define CONFIG_H

#define MAP_SIZE 96
#define TILE_SIZE 8
#define ISO_W 16
#define ISO_H 8

#define ID_EMPTY     0
#define ID_GRASS     16
#define ID_WALL      18
#define ID_OUTSIDE   20
#define ID_PLAYER    256
#define ID_RED_BALL  258

// Game Scene States
typedef enum {
    SCENE_EXPLORATION,
    SCENE_BATTLE,
    SCENE_DIALOG,
    SCENE_MENU
} GameScene;

// Battle States
typedef enum {
    BATTLE_PLAYER_TURN,
    BATTLE_ENEMY_TURN,
    BATTLE_MENU_SELECT,
    BATTLE_ANIMATION,
    BATTLE_RESULT
} BattleState;

// TIC-80 Color Palette
typedef enum {
    COLOR_BLACK = 0,       // #1A1C2C
    COLOR_PURPLE = 1,      // #5D275D
    COLOR_RED = 2,         // #B13E53
    COLOR_ORANGE = 3,      // #EF7D57
    COLOR_YELLOW = 4,      // #FFCD75
    COLOR_LIGHT_GREEN = 5, // #A7F070
    COLOR_GREEN = 6,       // #38B764
    COLOR_DARK_GREEN = 7,  // #257179
    COLOR_DARK_BLUE = 8,   // #29366F
    COLOR_BLUE = 9,        // #3B5DC9
    COLOR_LIGHT_BLUE = 10, // #41A6F6
    COLOR_CYAN = 11,       // #73EFF7
    COLOR_WHITE = 12,      // #F4F4F4
    COLOR_LIGHT_GREY = 13, // #94B0C2
    COLOR_GREY = 14,       // #566C86
    COLOR_DARK_GREY = 15   // #333C57
} TIC80Color;

// Game-specific color mappings
#define COL_TRANSPARENT 0
#define COL_HIGHLIGHT   COLOR_WHITE
#define COL_GRASS_TOP   COLOR_GREEN
#define COL_GRASS_SIDE  COLOR_LIGHT_GREEN
#define COL_WALL_TOP    COLOR_DARK_GREY
#define COL_WALL_SIDE   COLOR_LIGHT_GREY
#define COL_OUT_TOP     COLOR_RED
#define COL_OUT_SIDE    COLOR_PURPLE
#define COL_PLAYER      COLOR_GREY
#define COL_RED_BALL    COLOR_RED

// Battle Configuration
#define BATTLE_ENEMY_HP  50
#define BATTLE_ENEMY_ATK 8
#define BATTLE_PLAYER_ATK 10
#define BATTLE_MENU_X    10
#define BATTLE_MENU_Y    100
#define BATTLE_MENU_ITEM_H 12

// Z-axis Configuration
#define MAX_Z_HEIGHT 5.0f
#define Z_MOVE_SPEED 0.25f
#define COLLISION_Z_THRESHOLD 0.5f
#define WALL_BASE_HEIGHT 1.0f
#define MAX_WALL_HEIGHT 6
#define WALL_STAND_HEIGHT 0.8f
#define GRAVITY 0.01f
#define PLATFORM_SNAP_THRESHOLD 0.3f

// Altimeter UI Configuration
#define ALTIMETER_X 5
#define ALTIMETER_Y 20
#define ALTIMETER_WIDTH 8
#define ALTIMETER_HEIGHT 100
#define ALTIMETER_BORDER_COLOR COLOR_WHITE
#define ALTIMETER_FILL_COLOR COLOR_LIGHT_BLUE
#define ALTIMETER_BG_COLOR COLOR_DARK_GREY


#endif
