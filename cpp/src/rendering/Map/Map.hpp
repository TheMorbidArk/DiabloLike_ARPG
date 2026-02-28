#pragma once

#include "core/Config.hpp"
#include "core/Types.hpp"
#include "core/Entity/EntityManager.hpp"
#include <array>
#include <cstdlib>
#include <cstring>

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

    void generate(uint32_t seed, bool generateWalls) {
        if (!generateWalls) {
            for (int y = 0; y < core::MAP_SIZE; ++y) {
                for (int x = 0; x < core::MAP_SIZE; ++x) {
                    setTile(x, y, core::ID_GRASS);
                }
            }
            return;
        }

        std::array<uint8_t, core::MAP_SIZE * core::MAP_SIZE> tempMap{};
        std::array<uint8_t, core::MAP_SIZE * core::MAP_SIZE> visited{};
        std::array<int, core::MAP_SIZE * core::MAP_SIZE> qx{};
        std::array<int, core::MAP_SIZE * core::MAP_SIZE> qy{};

        std::srand(seed);

        for (int y = 0; y < core::MAP_SIZE; ++y) {
            for (int x = 0; x < core::MAP_SIZE; ++x) {
                if (x == 0 || x == core::MAP_SIZE - 1 || y == 0 || y == core::MAP_SIZE - 1) {
                    setTile(x, y, core::ID_WALL);
                } else {
                    setTile(x, y, (std::rand() % 100 < 48) ? core::ID_WALL : core::ID_GRASS);
                }
            }
        }

        for (int iter = 0; iter < 5; ++iter) {
            for (int y = 1; y < core::MAP_SIZE - 1; ++y) {
                for (int x = 1; x < core::MAP_SIZE - 1; ++x) {
                    int walls = 0;
                    for (int yy = -1; yy <= 1; ++yy) {
                        for (int xx = -1; xx <= 1; ++xx) {
                            if (safeGet(x + xx, y + yy) == core::ID_WALL) {
                                walls++;
                            }
                        }
                    }
                    if (walls >= 5) {
                        tempMap[static_cast<size_t>(y) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(x)] = core::ID_WALL;
                    } else {
                        tempMap[static_cast<size_t>(y) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(x)] = core::ID_GRASS;
                    }
                }
            }
        for (int y = 1; y < core::MAP_SIZE - 1; ++y) {
            for (int x = 1; x < core::MAP_SIZE - 1; ++x) {
                setTile(x, y, tempMap[static_cast<size_t>(y) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(x)]);
            }
        }
        }

        visited.fill(0);

        size_t head = 0;
        size_t tail = 0;

        auto entities = core::EntityManager::get().getArray();
        const auto count = core::EntityManager::get().getCount();

        for (size_t e = 0; e < count; ++e) {
            int entityX = static_cast<int>(entities[e].entity.pos.x);
            int entityY = static_cast<int>(entities[e].entity.pos.y);

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int tx = entityX + dx;
                    int ty = entityY + dy;
                    if (tx >= 0 && tx < core::MAP_SIZE && ty >= 0 && ty < core::MAP_SIZE) {
                        setTile(tx, ty, core::ID_GRASS);

                        if (visited[static_cast<size_t>(ty) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(tx)] == 0) {
                            qx[tail] = tx;
                            qy[tail] = ty;
                            tail++;
                            visited[static_cast<size_t>(ty) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(tx)] = 1;
                        }
                    }
                }
            }
        }

        const int dxx[4] = {1, -1, 0, 0};
        const int dyy[4] = {0, 0, 1, -1};

        while (head < tail) {
            int cx = qx[head];
            int cy = qy[head];
            head++;

            for (int i = 0; i < 4; ++i) {
                int nx = cx + dxx[i];
                int ny = cy + dyy[i];
                if (nx > 0 && nx < core::MAP_SIZE - 1 && ny > 0 && ny < core::MAP_SIZE - 1) {
                    if (visited[static_cast<size_t>(ny) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(nx)] == 0 && 
                        getTile(nx, ny) == core::ID_GRASS) {
                        visited[static_cast<size_t>(ny) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(nx)] = 1;
                        qx[tail] = nx;
                        qy[tail] = ny;
                        tail++;
                    }
                }
            }
        }

        for (int y = 0; y < core::MAP_SIZE; ++y) {
            for (int x = 0; x < core::MAP_SIZE; ++x) {
                if (getTile(x, y) == core::ID_GRASS && 
                    visited[static_cast<size_t>(y) * static_cast<size_t>(core::MAP_SIZE) + static_cast<size_t>(x)] == 0) {
                    setTile(x, y, core::ID_WALL);
                }
            }
        }
    }

    uint8_t getTile(int x, int y) const {
        if (!isInside(x, y)) return core::ID_OUTSIDE;
        return m_tiles[static_cast<size_t>(y) * core::MAP_SIZE + static_cast<size_t>(x)];
    }

    void setTile(int x, int y, uint8_t id) {
        if (!isInside(x, y)) return;
        m_tiles[static_cast<size_t>(y) * core::MAP_SIZE + static_cast<size_t>(x)] = id;
    }

    bool isInside(int x, int y) const {
        return x >= 0 && x < core::MAP_SIZE && y >= 0 && y < core::MAP_SIZE;
    }

private:
    Map() = default;

    uint8_t safeGet(int x, int y) const {
        if (x < 0 || x >= core::MAP_SIZE || y < 0 || y >= core::MAP_SIZE) {
            return core::ID_WALL;
        }
        return getTile(x, y);
    }

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
