#pragma once

#include "core/Types.hpp"
#include "core/Config.hpp"

namespace gameplay {

struct PlayerState {
    float posX{};
    float posY{};
    float z{};
    float velocityZ{};
    bool isJumping{false};

    void reset() {
        posX = 0;
        posY = 0;
        z = 0;
        velocityZ = 0;
        isJumping = false;
    }
};

class Player {
public:
    static Player& get() {
        static Player instance;
        return instance;
    }

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    void init() {
        m_state.reset();
    }

    void update() {
        if (m_state.isJumping) {
            m_state.velocityZ -= 0.5f;
            m_state.z += m_state.velocityZ;
            if (m_state.z <= 0.0f) {
                m_state.z = 0.0f;
                m_state.velocityZ = 0.0f;
                m_state.isJumping = false;
            }
        }
    }

    void jump() {
        if (!m_state.isJumping && m_state.z == 0.0f) {
            m_state.isJumping = true;
            m_state.velocityZ = 2.0f;
        }
    }

    void move(float dx, float dy) {
        const float speed = 0.15f;
        const int newX = static_cast<int>(m_state.posX + dx * speed);
        const int newY = static_cast<int>(m_state.posY + dy * speed);

        if (rendering::mapIsInside(newX, newY)) {
            const uint8_t tile = rendering::mapGetTile(newX, newY);
            if (tile != core::ID_WALL) {
                m_state.posX += dx * speed;
                m_state.posY += dy * speed;
            }
        }
    }

    void setPosition(float x, float y, float z) {
        m_state.posX = x;
        m_state.posY = y;
        m_state.z = z;
    }

    void getPosition(float* x, float* y, float* z) const {
        *x = m_state.posX;
        *y = m_state.posY;
        *z = m_state.z;
    }

    [[nodiscard]] auto getState() -> PlayerState& { return m_state; }
    [[nodiscard]] auto getState() const -> const PlayerState& { return m_state; }

private:
    Player() = default;
    PlayerState m_state{};
};

}
