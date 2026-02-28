#pragma once

#include "core/Types.hpp"
#include "core/Config.hpp"
#include "core/Entity/EntityManager.hpp"
#include "utils/MathIso.hpp"
#include "rendering/Camera/Camera.hpp"
#include "rendering/Map/Map.hpp"
#include "tic80/Tic80.hpp"

namespace rendering {

class Renderer {
public:
    static Renderer& get() {
        static Renderer instance;
        return instance;
    }

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init() {
    }

    void renderScene(const core::Entity* player) {
        tic80::cls(core::COLOR_BLACK);

        auto* cam = &Camera::get();
        float camX, camY;
        cam->getPosition(&camX, &camY);

        const int rangeX = 50;
        const int rangeY = 45;

        const float centerIsoX = camX + static_cast<float>(core::WIDTH) / 2.0f;
        const float centerIsoY = camY + static_cast<float>(core::HEIGHT) / 2.0f;

        const float centerWx = (centerIsoX / 16.0f + centerIsoY / 8.0f) / 2.0f;
        const float centerWy = (centerIsoY / 8.0f - centerIsoX / 16.0f) / 2.0f;

        const int cx = static_cast<int>(centerWx);
        const int cy = static_cast<int>(centerWy);

        uint8_t trans = 0;

        for (int y = cy - rangeY; y <= cy + rangeY; y++) {
            for (int x = cx - rangeX; x <= cx + rangeX; x++) {
                uint8_t tid;
                if (!Map::get().isInside(x, y)) {
                    tid = static_cast<uint8_t>(core::ID_OUTSIDE);
                } else {
                    tid = Map::get().getTile(x, y);
                    if (tid == 0) continue;
                }

                int sx, sy;
                utils::worldToScreen(static_cast<float>(x), static_cast<float>(y), 0.0f, sx, sy);

                if (tid == static_cast<uint8_t>(core::ID_WALL)) {
                    tic80::spr(static_cast<int32_t>(tid), sx, sy - 8, &trans, 1, 1, 0, 0, 2, 2);
                } else {
                    tic80::spr(static_cast<int32_t>(tid), sx, sy, &trans, 1, 1, 0, 0, 2, 2);
                }
            }
        }

        renderMinimap(player);
    }

    void renderMinimap([[maybe_unused]] const core::Entity* player) {
        constexpr int kMMW = 64;
        constexpr int kMMH = 32;
        const int offsetX = core::WIDTH - kMMW - 8;
        const int offsetY = 8;

        const int cx = offsetX + kMMW / 2;
        const int cy = offsetY + kMMH / 2;

        for (int dy = 0; dy < kMMH; dy++) {
            for (int dx = 0; dx < kMMW; dx++) {
                const int wx = cx - kMMW / 2 + (dx - dy);
                const int wy = (cx + cy - kMMW / 2 + kMMH / 2) - (dx + dy) / 2;

                if (wx >= 0 && wx < core::MAP_SIZE && wy >= 0 && wy < core::MAP_SIZE) {
                    const uint8_t tid = Map::get().getTile(wx, wy);
                    if (tid == 0) continue;

                    const int mmRelX = dx - dy;
                    const int mmRelY = (dx + dy) / 2;

                    if (mmRelX * mmRelX + mmRelY * mmRelY >= (kMMW / 2) * (kMMW / 2)) continue;

                    const int finalSx = cx + mmRelX;
                    const int finalSy = cy + mmRelY;

                    const uint8_t col = (tid == static_cast<uint8_t>(core::ID_WALL)) ? static_cast<uint8_t>(core::COLOR_LIGHT_GREY) : static_cast<uint8_t>(core::COLOR_ORANGE);
                    tic80::rect(finalSx, finalSy, 2, 2, static_cast<int8_t>(col));
                }
            }
        }

        const float bTopY = static_cast<float>(offsetY) - 2.0f;
        const float bBottomY = static_cast<float>(offsetY + kMMH) + 2.0f;
        const float bLeftX = static_cast<float>(offsetX) - 4.0f;
        const float bRightX = static_cast<float>(offsetX + kMMW) + 6.0f;
        const float midX = static_cast<float>(cx);
        const float midY = static_cast<float>(cy);

        tic80::line(static_cast<int>(midX), static_cast<int>(bTopY), static_cast<int>(bRightX), static_cast<int>(midY), core::COLOR_WHITE);
        tic80::line(static_cast<int>(bRightX), static_cast<int>(midY), static_cast<int>(midX), static_cast<int>(bBottomY), core::COLOR_WHITE);
        tic80::line(static_cast<int>(midX), static_cast<int>(bBottomY), static_cast<int>(bLeftX), static_cast<int>(midY), core::COLOR_WHITE);
        tic80::line(static_cast<int>(bLeftX), static_cast<int>(midY), static_cast<int>(midX), static_cast<int>(bTopY), core::COLOR_WHITE);

        tic80::rect(cx - 1, cy - 1, 2, 2, core::COLOR_DARK_GREY);
    }

private:
    Renderer() = default;
};

}
