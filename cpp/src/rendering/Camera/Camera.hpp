#pragma once

#include "core/Types.hpp"
#include "core/Config.hpp"
#include "core/Context.hpp"
#include "core/Entity/EntityManager.hpp"
#include "utils/MathIso.hpp"
#include <array>

namespace rendering {

class Camera {
public:
    static Camera& get() {
        static Camera instance;
        return instance;
    }

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    void init(float x = 0.0f, float y = 0.0f) {
        m_x = x;
        m_y = y;
        m_speedX = 0.15f;
        m_speedY = 0.12f;
        m_touring = false;
        m_tourIndex = 0;
    }

    void setPosition(float x, float y) {
        m_x = x;
        m_y = y;
    }

    void getPosition(float* x, float* y) const {
        *x = m_x;
        *y = m_y;
    }

    void update(const core::Entity* player) {
        if (m_touring) {
            return;
        }

        const float targetIsoX = (player->pos.x - player->pos.y) * (utils::ISO_W / 2.0f) + (core::WIDTH / 2.0f);
        const float targetIsoY = (player->pos.x + player->pos.y) * (utils::ISO_H / 2.0f);
        const float adjustedIsoY = targetIsoY - (player->pos.z * 16.0f);
        const float absTargetX = targetIsoX - 8.0f;
        const float absTargetY = adjustedIsoY - 4.0f;
        const float idealCamX = absTargetX - (core::WIDTH / 2.0f);
        const float idealCamY = absTargetY - (core::HEIGHT / 2.0f);

        m_x += (idealCamX - m_x) * m_speedX;
        m_y += (idealCamY - m_y) * m_speedY;
    }

    void startTour() {
        m_touring = true;
        m_tourIndex = 0;
    }

    void stopTour() {
        m_touring = false;
    }

    [[nodiscard]] bool isTouring() const { return m_touring; }

private:
    Camera() = default;

    float m_x{};
    float m_y{};
    float m_speedX{0.1f};
    float m_speedY{0.1f};
    bool m_touring{false};
    size_t m_tourIndex{0};
    std::array<std::pair<float, float>, 8> m_tourPoints{};
};

inline Camera* cameraGetMain() {
    return &Camera::get();
}

inline void cameraGetPosition(Camera* cam, float* x, float* y) {
    cam->getPosition(x, y);
}

}
