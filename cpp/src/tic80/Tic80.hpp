#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <ctime>

#define WASM_EXPORT(name) __attribute__((export_name(name)))
#define WASM_IMPORT(name) __attribute__((import_name(name)))

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

inline int32_t btn(int32_t id) { return ::btn(id); }

inline bool btnp(int32_t id, int32_t hold = -1, int32_t period = -1) { return ::btnp(id, hold, period); }

inline int8_t peek(int32_t addr, int8_t bits = 8) { return ::peek(addr, bits); }

inline void poke(int32_t addr, int8_t value, int8_t bits = 8) { ::poke(addr, value, bits); }

inline void map(int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx, int32_t sy, uint8_t* trans_colors = nullptr, int8_t colorCount = 0, int32_t scale = 1, int32_t remap = 0) {
    ::map(x, y, w, h, sx, sy, trans_colors, colorCount, scale, remap);
}

inline int32_t mget(int32_t x, int32_t y) { return ::mget(x, y); }

inline void mset(int32_t x, int32_t y, int32_t value) { ::mset(x, y, value); }

inline void* memcpy(void* dest, const void* src, uint32_t n) { return ::tic_memcpy(dest, src, n); }

inline void* memset(void* dest, int8_t value, uint32_t n) { return ::tic_memset(dest, value, n); }

inline uint32_t ticTime() { return static_cast<uint32_t>(::tic_time() * 1000.0f); }

inline void exit() { ::tic_exit(); }

inline void trace(const char* text, int8_t color = 0) { ::trace(text, color); }

}
