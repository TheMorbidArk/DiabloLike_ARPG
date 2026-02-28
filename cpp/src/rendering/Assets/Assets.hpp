#pragma once

#include "core/Config.hpp"
#include "tic80/Tic80.hpp"
#include <array>
#include <cmath>

namespace rendering {

namespace detail {

inline void setRawPixel(int tileId, int x, int y, int color) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return;
    int addr = tileId * 32 + (y * 4) + (x / 2);
    uint8_t* ptr = reinterpret_cast<uint8_t*>(0x04000) + addr;
    if (x % 2 == 0) {
        *ptr = (*ptr & 0xF0) | static_cast<uint8_t>(color & 0x0F);
    } else {
        *ptr = (*ptr & 0x0F) | static_cast<uint8_t>((color & 0x0F) << 4);
    }
}

inline void setCompositePixel(int baseId, int lx, int ly, int color) {
    if (lx < 0 || lx >= 16 || ly < 0 || ly >= 16) return;
    int tx = lx / 8;
    int ty = ly / 8;
    int realId = baseId + tx + (ty * 16);
    setRawPixel(realId, lx % 8, ly % 8, color);
}

inline void genBlock16(int id, int cTop, int cSideL, int cSideR, int heightPx) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            int col = core::COL_TRANSPARENT;
            int dx = std::abs(x - 7) - (x > 7 ? 1 : 0);
            bool inTop = false;
            if (y <= 7) {
                int halfW = (y <= 3) ? (y * 2 + 1) : ((7 - y) * 2 + 1);
                if (dx <= halfW) inTop = true;
            }
            bool inSide = false;
            if (!inTop && y >= 4 && y < (4 + heightPx)) {
                int halfW = 7;
                if (dx <= halfW) inSide = true;
            }
            if (inTop) {
                col = cTop;
                if (dx == (y <= 3 ? y * 2 + 1 : (7 - y) * 2 + 1)) col = core::COL_HIGHLIGHT;
            } else if (inSide) {
                col = (x < 8) ? cSideL : cSideR;
                if (x == 7 || x == 8) col = cSideL + 1;
            }
            if (col != core::COL_TRANSPARENT) {
                setCompositePixel(id, x, y, col);
            } else {
                setCompositePixel(id, x, y, core::COL_TRANSPARENT);
            }
        }
    }
}

inline void genPlayer(int id) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            setCompositePixel(id, x, y, static_cast<int>(core::COLOR_BLACK));
            int dx = x - 8;
            int dy = y - 8;
            if (dx * dx + dy * dy < 20) {
                setCompositePixel(id, x, y - 4, core::COL_PLAYER);
            }
        }
    }
}

inline void genRedBall(int id) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            setCompositePixel(id, x, y, static_cast<int>(core::COLOR_BLACK));
            int dx = x - 8;
            int dy = y - 8;
            if (dx * dx + dy * dy < 25) {
                int dist = dx * dx + dy * dy;
                if (dist < 6) {
                    setCompositePixel(id, x, y, static_cast<int>(core::COLOR_WHITE));
                } else if (dist < 16) {
                    setCompositePixel(id, x, y, core::COL_RED_BALL);
                } else {
                    setCompositePixel(id, x, y, static_cast<int>(core::COLOR_DARK_GREY));
                }
            }
        }
    }
}

}

class Assets {
public:
    static auto get() -> Assets& {
        static Assets instance;
        return instance;
    }

    Assets(const Assets&) = delete;
    auto operator=(const Assets&) -> Assets& = delete;

    void init() {
        detail::genBlock16(core::ID_GRASS, core::COL_GRASS_TOP, core::COL_GRASS_SIDE, core::COL_GRASS_SIDE, 4);
        detail::genBlock16(core::ID_WALL, core::COL_WALL_TOP, core::COL_WALL_SIDE, core::COL_WALL_SIDE - 1, 12);
        detail::genBlock16(core::ID_OUTSIDE, core::COL_OUT_TOP, core::COL_OUT_SIDE, core::COL_OUT_SIDE, 3);
        detail::genPlayer(core::ID_PLAYER);
        detail::genRedBall(core::ID_RED_BALL);
    }

private:
    Assets() = default;
};

}
