#pragma once

#include "Types.hpp"

namespace core {

constexpr int WIDTH = 240;
constexpr int HEIGHT = 136;
constexpr int MAP_SIZE = 96;
constexpr int TILE_SIZE = 8;
constexpr int ISO_W = 16;
constexpr int ISO_H = 8;
constexpr int MAX_ENTITIES = 32;

constexpr int ID_EMPTY = 0;
constexpr int ID_GRASS = 16;
constexpr int ID_WALL = 18;
constexpr int ID_OUTSIDE = 20;
constexpr int ID_PLAYER = 256;
constexpr int ID_RED_BALL = 258;

constexpr float MAX_Z_HEIGHT = 5.0f;
constexpr float Z_MOVE_SPEED = 0.25f;
constexpr float COLLISION_Z_THRESHOLD = 0.5f;
constexpr float WALL_BASE_HEIGHT = 1.0f;
constexpr int MAX_WALL_HEIGHT = 6;
constexpr float WALL_STAND_HEIGHT = 0.8f;
constexpr float GRAVITY = 0.01f;
constexpr float PLATFORM_SNAP_THRESHOLD = 0.3f;

enum Color : uint8_t {
    COLOR_BLACK = 0,
    COLOR_PURPLE = 1,
    COLOR_RED = 2,
    COLOR_ORANGE = 3,
    COLOR_YELLOW = 4,
    COLOR_LIGHT_GREEN = 5,
    COLOR_GREEN = 6,
    COLOR_DARK_GREEN = 7,
    COLOR_DARK_BLUE = 8,
    COLOR_BLUE = 9,
    COLOR_LIGHT_BLUE = 10,
    COLOR_CYAN = 11,
    COLOR_WHITE = 12,
    COLOR_LIGHT_GREY = 13,
    COLOR_GREY = 14,
    COLOR_DARK_GREY = 15
};

constexpr int COL_TRANSPARENT = 0;
constexpr int COL_HIGHLIGHT = COLOR_WHITE;
constexpr int COL_GRASS_TOP = COLOR_GREEN;
constexpr int COL_GRASS_SIDE = COLOR_LIGHT_GREEN;
constexpr int COL_WALL_TOP = COLOR_DARK_GREY;
constexpr int COL_WALL_SIDE = COLOR_LIGHT_GREY;
constexpr int COL_OUT_TOP = COLOR_RED;
constexpr int COL_OUT_SIDE = COLOR_PURPLE;
constexpr int COL_PLAYER = COLOR_GREY;
constexpr int COL_RED_BALL = COLOR_RED;

enum class GameScene : uint8_t {
    Exploration = 0,
    Battle = 1,
    Dialog = 2,
    Menu = 3,
    FreeWalk = 4
};

enum BattleState : uint8_t {
    BATTLE_PLAYER_TURN = 0,
    BATTLE_ENEMY_TURN = 1,
    BATTLE_MENU_SELECT = 2,
    BATTLE_ANIMATION = 3,
    BATTLE_RESULT = 4
};

constexpr int BATTLE_ENEMY_HP = 50;
constexpr int BATTLE_ENEMY_ATK = 8;
constexpr int BATTLE_PLAYER_ATK = 10;
constexpr int BATTLE_MENU_X = 10;
constexpr int BATTLE_MENU_Y = 100;
constexpr int BATTLE_MENU_ITEM_H = 12;

constexpr int ALTIMETER_X = 5;
constexpr int ALTIMETER_Y = 20;
constexpr int ALTIMETER_WIDTH = 8;
constexpr int ALTIMETER_HEIGHT = 100;
constexpr int ALTIMETER_BORDER_COLOR = COLOR_WHITE;
constexpr int ALTIMETER_FILL_COLOR = COLOR_LIGHT_BLUE;
constexpr int ALTIMETER_BG_COLOR = COLOR_DARK_GREY;

}
