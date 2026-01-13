#include "map.h"
#include "../tic80.h"
#include "../config.h"

void map_init() {
    for(int y=0; y<MAP_SIZE; y++) for(int x=0; x<MAP_SIZE; x++) {
        int t = ID_GRASS;
        if (x == 0 || x == MAP_SIZE-1 || y == 0 || y == MAP_SIZE-1) t = ID_WALL;
        else if ((x * y) % 11 == 0 && (x+y)%2 == 0) t = ID_WALL;
        mset(x, y, t);
    }
}

bool is_solid(float x, float y) {
    int ix = (int)x; int iy = (int)y;
    if (ix >= 0 && ix < MAP_SIZE && iy >= 0 && iy < MAP_SIZE) {
        if (mget(ix, iy) == ID_WALL) return true;
    }
    return false;
}