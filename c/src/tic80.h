#pragma once
#include <stdbool.h>
#include <stdint.h>

#define WASM_EXPORT(name) __attribute__((export_name(name)))
#define WASM_IMPORT(name) __attribute__((import_name(name)))

#define TILE_SIZE 8
#define WIDTH 240
#define HEIGHT 136
#define BPP 4

typedef struct {
    uint8_t SCREEN[WIDTH * HEIGHT * BPP / 8];
    uint8_t PALETTE[48];
    uint8_t PALETTE_MAP[8];
    uint8_t BORDER_COLOR_AND_OVR_TRANSPARENCY;
    int8_t SCREEN_OFFSET_X;
    int8_t SCREEN_OFFSET_Y;
    int8_t MOUSE_CURSOR;
    uint8_t BLIT_SEGMENT;
    uint8_t RESERVED[3];
} VRAM;

typedef struct {
    int16_t x; int16_t y;
    int8_t scrollx; int8_t scrolly;
    bool left; bool middle; bool right;
} Mouse;

// 声明为 extern，防止重复符号错误
extern VRAM* FRAMEBUFFER;
extern uint8_t* TILES;
extern uint8_t* SPRITES;
extern uint8_t* MAP;
extern uint8_t* GAMEPADS;
extern uint8_t* MOUSE;
extern uint8_t* KEYBOARD;

WASM_IMPORT("cls") void cls(int8_t color);
WASM_IMPORT("spr") void spr(int32_t id, int32_t x, int32_t y, uint8_t* trans_colors, int8_t color_count, int32_t scale, int32_t flip, int32_t rotate, int32_t w, int32_t h);
WASM_IMPORT("mget") int32_t mget(int32_t x, int32_t y);
WASM_IMPORT("mset") void mset(int32_t x, int32_t y, int32_t value);
WASM_IMPORT("btn") int32_t btn(int32_t index);
WASM_IMPORT("print") int32_t print(const char* text, int32_t x, int32_t y, int8_t color, int8_t fixed, int32_t scale, int8_t alt);