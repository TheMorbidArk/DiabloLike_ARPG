#include "tic80.h"
VRAM* FRAMEBUFFER = (VRAM*)0;
uint8_t* TILES = (uint8_t*)0x04000;
uint8_t* SPRITES = (uint8_t*)0x06000;
uint8_t* MAP = (uint8_t*)0x08000;
uint8_t* GAMEPADS = (uint8_t*)0x0FF80;
uint8_t* MOUSE = (uint8_t*)0x0FF84;
uint8_t* KEYBOARD = (uint8_t*)0x0FF88;