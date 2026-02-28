#pragma once

#include "core/Config.hpp"
#include "core/Types.hpp"
#include <array>

namespace rendering {

class Map {
public:
    static Map& get() {
        static Map instance;
        return instance;
    }

    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    void init() {
        m_tiles.fill(0);
    }

    void generate(uint32_t seed, bool walls) {
        if (walls) {
            for (int y = 0; y < core::MAP_SIZE; ++y) {
                for (int x = 0; x < core::MAP_SIZE; ++x) {
                    uint32_t hash = seed + static_cast<uint32_t>(x * 374761393 + y * 668265263);
                    hash = (hash ^ (hash >> 13)) * 1274126177;
                    const uint8_t val = static_cast<uint8_t>(hash % 100);
                    if (val < 10) {
                        setTile(x, y, core::ID_WALL);
                    } else {
                        setTile(x, y, core::ID_GRASS);
                    }
                }
            }
        } else {
            for (int y = 0; y < core::MAP_SIZE; ++y) {
                for (int x = 0; x < core::MAP_SIZE; ++x) {
                    setTile(x, y, core::ID_GRASS);
                }
            }
        }
    }

    uint8_t getTile(int x, int y) const {
        if (!isInside(x, y)) return core::ID_OUTSIDE;
        return m_tiles[static_cast<size_t>(y) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(x)];
    }

    void setTile(int x, int y, uint8_t id) {
        if (!isInside(x, y)) return;
        m_tiles[static_cast<size_t>(y) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(x)] = id;
    }

    bool isInside(int x, int y) const {
        return x >= 0 && x < core::MAP_SIZE && y >= 0 && y < core::MAP_SIZE;
    }

private:
    Map() = default;

    std::array<uint8_t, core::MAP_SIZE * core::MAP_SIZE> m_tiles{};
};

inline uint8_t mapGetTile(int x, int y) {
    return Map::get().getTile(x, y);
}

inline void mapSetTile(int x, int y, uint8_t id) {
    Map::get().setTile(x, y, id);
}

inline bool mapIsInside(int x, int y) {
    return Map::get().isInside(x, y);
}

inline void mapGenerate(uint32_t seed, bool walls) {
    Map::get().generate(seed, walls);
}

}
