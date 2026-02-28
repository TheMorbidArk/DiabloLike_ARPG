#pragma once

#include "core/Types.hpp"

namespace gameplay {

class BattleSystem {
public:
    static BattleSystem& get() {
        static BattleSystem instance;
        return instance;
    }

    BattleSystem(const BattleSystem&) = delete;
    BattleSystem& operator=(const BattleSystem&) = delete;

    void init() {
        m_active = false;
        m_enemyCount = 0;
    }

    void startBattle() {
        m_active = true;
    }

    void endBattle() {
        m_active = false;
    }

    [[nodiscard]] auto isActive() const -> bool { return m_active; }

    void update() {
    }

private:
    BattleSystem() = default;

    bool m_active{false};
    uint8_t m_enemyCount{0};
};

}
