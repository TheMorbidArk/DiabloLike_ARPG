#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <array>

#define WASM_EXPORT(name) __attribute__((export_name(name)))
#define WASM_IMPORT(name) __attribute__((import_name(name)))

namespace tic80 {

constexpr int32_t TILE_SIZE = 8;
constexpr int32_t WIDTH = 240;
constexpr int32_t HEIGHT = 136;
constexpr int32_t WIDTH_TILES = WIDTH / TILE_SIZE;
constexpr int32_t HEIGHT_TILES = HEIGHT / TILE_SIZE;
constexpr int32_t BPP = 4;

constexpr uint32_t TILES_SIZE = 0x2000;
constexpr uint32_t SPRITES_SIZE = 0x2000;
constexpr uint32_t MAP_SIZE_BASE = 32640;
constexpr uint32_t GAMEPADS_SIZE = 4;
constexpr uint32_t MOUSE_SIZE = 4;
constexpr uint32_t KEYBOARD_SIZE = 4;
constexpr uint32_t PERSISTENT_MEMORY_SIZE = 1024;
constexpr uint32_t SPRITE_FLAGS_SIZE = 512;
constexpr uint32_t WASM_FREE_RAM_SIZE = 163840;

struct Mouse {
    int16_t x;
    int16_t y;
    int8_t scrollx;
    int8_t scrolly;
    bool left;
    bool middle;
    bool right;
};

}

extern "C" {

WASM_IMPORT("circ") void circ(int32_t x, int32_t y, int32_t radius, int8_t color);
WASM_IMPORT("circb") void circb(int32_t x, int32_t y, int32_t radius, int8_t color);
WASM_IMPORT("clip") void clip(int32_t x, int32_t y, int32_t width, int32_t height);
WASM_IMPORT("cls") void cls(int8_t color);
WASM_IMPORT("elli") void elli(int32_t x, int32_t y, int32_t a, int32_t b, int8_t color);
WASM_IMPORT("ellib") void ellib(int32_t x, int32_t y, int32_t a, int32_t b, int8_t color);
WASM_IMPORT("font") int8_t font(const char* text, int32_t x, int32_t y, uint8_t* trans_colors, int8_t trans_count, int8_t char_width, int8_t char_height, bool fixed, int32_t scale, bool alt);
WASM_IMPORT("line") void line(float x0, float y0, float x1, float y1, int8_t color);
WASM_IMPORT("map") void map(int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx, int32_t sy, uint8_t* trans_colors, int8_t colorCount, int32_t scale, int32_t remap);
WASM_IMPORT("pix") uint8_t pix(int32_t x, int32_t y, int8_t color);
WASM_IMPORT("print") int32_t print(const char* text, int32_t x, int32_t y, int8_t color, int8_t fixed, int32_t scale, bool alt);
WASM_IMPORT("rect") void rect(int32_t x, int32_t y, int32_t w, int32_t h, int8_t color);
WASM_IMPORT("rectb") void rectb(int32_t x, int32_t y, int32_t w, int32_t h, int8_t color);
WASM_IMPORT("spr") void spr(int32_t id, int32_t x, int32_t y, uint8_t* trans_colors, int8_t color_count, int32_t scale, int32_t flip, int32_t rotate, int32_t w, int32_t h);
WASM_IMPORT("tri") void tri(float x1, float y1, float x2, float y2, float x3, float y3, int8_t color);
WASM_IMPORT("trib") void trib(float x1, float y1, float x2, float y2, float x3, float y3, int8_t color);
WASM_IMPORT("ttri") void ttri(float x1, float y1, float x2, float y2, float x3, float y3, float u1, float v1, float u2, float v2, float u3, float v3, int32_t texsrc, uint8_t* trans_colors, int8_t color_count, float z1, float z2, float z3, bool depth);

WASM_IMPORT("btn") int32_t btn(int32_t index);
WASM_IMPORT("btnp") bool btnp(int32_t index, int32_t hold, int32_t period);
WASM_IMPORT("key") int32_t key(int32_t x);
WASM_IMPORT("keyp") int32_t keyp(int8_t x, int32_t hold, int32_t period);
WASM_IMPORT("mouse") void mouse(tic80::Mouse* mouse_ptr_addy);

WASM_IMPORT("music") void music(int32_t track, int32_t frame, int32_t row, bool loop, bool sustain, int32_t tempo, int32_t speed);
WASM_IMPORT("sfx") void sfx(int32_t sfx_id, int32_t note, int32_t octave, int32_t duration, int32_t channel, int32_t volume_left, int32_t volume_right, int32_t speed);

WASM_IMPORT("pmem") uint32_t pmem(int32_t address, int64_t value);
WASM_IMPORT("peek") int8_t peek(int32_t address, int8_t bits);
WASM_IMPORT("peek1") int8_t peek1(int32_t address);
WASM_IMPORT("peek2") int8_t peek2(int32_t address);
WASM_IMPORT("peek4") int8_t peek4(int32_t address);
WASM_IMPORT("poke") void poke(int32_t address, int8_t value, int8_t bits);
WASM_IMPORT("poke1") void poke1(int32_t address, int8_t value);
WASM_IMPORT("poke2") void poke2(int32_t address, int8_t value);
WASM_IMPORT("poke4") void poke4(int32_t address, int8_t value);
WASM_IMPORT("sync") void sync(int32_t mask, int8_t bank, int8_t to_cart);
WASM_IMPORT("vbank") int8_t vbank(int8_t bank);

WASM_IMPORT("fget") bool fget(int32_t sprite_index, int8_t flag);
WASM_IMPORT("fset") bool fset(int32_t sprite_index, int8_t flag, bool value);
WASM_IMPORT("mget") int32_t mget(int32_t x, int32_t y);
WASM_IMPORT("mset") void mset(int32_t x, int32_t y, int32_t value);

WASM_IMPORT("exit") void tic_exit();
WASM_IMPORT("time") float tic_time();
WASM_IMPORT("tstamp") uint32_t tstamp();
WASM_IMPORT("trace") void trace(const char* text, int8_t color);

WASM_IMPORT("memcpy") void* tic_memcpy(void* dest, const void* src, uint32_t n);
WASM_IMPORT("memset") void* tic_memset(void* dest, int8_t value, uint32_t n);

}

namespace tic80 {

inline void cls(int8_t color) { ::cls(color); }

inline int32_t print(const char* text, int32_t x, int32_t y, int8_t color, int8_t fixed = 0, int32_t scale = 1, bool alt = false) { 
    return ::print(text, x, y, color, fixed, scale, alt); 
}

inline void rect(int32_t x, int32_t y, int32_t w, int32_t h, int8_t color) { ::rect(x, y, w, h, color); }

inline void rectb(int32_t x, int32_t y, int32_t w, int32_t h, int8_t color) { ::rectb(x, y, w, h, color); }

inline void line(float x1, float y1, float x2, float y2, int8_t color) { ::line(x1, y1, x2, y2, color); }

inline uint8_t pix(int32_t x, int32_t y, int8_t color) { return ::pix(x, y, color); }

inline void spr(int32_t id, int32_t x, int32_t y, uint8_t* trans_colors = nullptr, int8_t color_count = 0, int32_t scale = 1, int32_t flip = 0, int32_t rotate = 0, int32_t w = 1, int32_t h = 1) {
    ::spr(id, x, y, trans_colors, color_count, scale, flip, rotate, w, h);
}

inline void circ(int32_t x, int32_t y, int32_t radius, int8_t color) { ::circ(x, y, radius, color); }

inline void circb(int32_t x, int32_t y, int32_t radius, int8_t color) { ::circb(x, y, radius, color); }

inline void clip(int32_t x, int32_t y, int32_t width, int32_t height) { ::clip(x, y, width, height); }

inline void elli(int32_t x, int32_t y, int32_t a, int32_t b, int8_t color) { ::elli(x, y, a, b, color); }

inline void ellib(int32_t x, int32_t y, int32_t a, int32_t b, int8_t color) { ::ellib(x, y, a, b, color); }

inline int8_t font(const char* text, int32_t x, int32_t y, uint8_t* trans_colors = nullptr, int8_t trans_count = 0, int8_t char_width = 0, int8_t char_height = 0, bool fixed = false, int32_t scale = 1, bool alt = false) {
    return ::font(text, x, y, trans_colors, trans_count, char_width, char_height, fixed, scale, alt);
}

inline void tri(float x1, float y1, float x2, float y2, float x3, float y3, int8_t color) { ::tri(x1, y1, x2, y2, x3, y3, color); }

inline void trib(float x1, float y1, float x2, float y2, float x3, float y3, int8_t color) { ::trib(x1, y1, x2, y2, x3, y3, color); }

inline void ttri(float x1, float y1, float x2, float y2, float x3, float y3, float u1, float v1, float u2, float v2, float u3, float v3, int32_t texsrc = 0, uint8_t* trans_colors = nullptr, int8_t color_count = 0, float z1 = 1.0f, float z2 = 1.0f, float z3 = 1.0f, bool depth = false) {
    ::ttri(x1, y1, x2, y2, x3, y3, u1, v1, u2, v2, u3, v3, texsrc, trans_colors, color_count, z1, z2, z3, depth);
}

inline int32_t btn(int32_t id) { return ::btn(id); }

inline bool btnp(int32_t id, int32_t hold = -1, int32_t period = -1) { return ::btnp(id, hold, period); }

inline int32_t key(int32_t x) { return ::key(x); }

inline int32_t keyp(int8_t x, int32_t hold = -1, int32_t period = -1) { return ::keyp(x, hold, period); }

inline void mouse(Mouse* mouse_ptr) { ::mouse(mouse_ptr); }

inline void music(int32_t track = -1, int32_t frame = 0, int32_t row = 0, bool loop = true, bool sustain = false, int32_t tempo = -1, int32_t speed = -1) {
    ::music(track, frame, row, loop, sustain, tempo, speed);
}

inline void sfx(int32_t sfx_id = -1, int32_t note = 0, int32_t octave = 0, int32_t duration = -1, int32_t channel = 0, int32_t volume_left = 7, int32_t volume_right = 7, int32_t speed = 0) {
    ::sfx(sfx_id, note, octave, duration, channel, volume_left, volume_right, speed);
}

inline uint32_t pmem(int32_t address, int64_t value = 0) { return ::pmem(address, value); }

inline int8_t peek(int32_t addr, int8_t bits = 8) { return ::peek(addr, bits); }

inline int8_t peek1(int32_t addr) { return ::peek1(addr); }

inline int8_t peek2(int32_t addr) { return ::peek2(addr); }

inline int8_t peek4(int32_t addr) { return ::peek4(addr); }

inline void poke(int32_t addr, int8_t value, int8_t bits = 8) { ::poke(addr, value, bits); }

inline void poke1(int32_t addr, int8_t value) { ::poke1(addr, value); }

inline void poke2(int32_t addr, int8_t value) { ::poke2(addr, value); }

inline void poke4(int32_t addr, int8_t value) { ::poke4(addr, value); }

inline void sync(int32_t mask = 0, int8_t bank = 0, int8_t to_cart = 0) { ::sync(mask, bank, to_cart); }

inline int8_t vbank(int8_t bank = 0) { return ::vbank(bank); }

inline bool fget(int32_t sprite_index, int8_t flag) { return ::fget(sprite_index, flag); }

inline bool fset(int32_t sprite_index, int8_t flag, bool value = true) { return ::fset(sprite_index, flag, value); }

inline int32_t mget(int32_t x, int32_t y) { return ::mget(x, y); }

inline void mset(int32_t x, int32_t y, int32_t value) { ::mset(x, y, value); }

inline void* memcpy(void* dest, const void* src, uint32_t n) { return ::tic_memcpy(dest, src, n); }

inline void* memset(void* dest, int8_t value, uint32_t n) { return ::tic_memset(dest, value, n); }

inline uint32_t ticTime() { return static_cast<uint32_t>(::tic_time() * 1000.0f); }

inline uint32_t timestamp() { return ::tstamp(); }

inline void exit() { ::tic_exit(); }

inline void trace(const char* text, int8_t color = 0) { ::trace(text, color); }

}
