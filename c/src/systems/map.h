#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
void map_init();
bool is_solid(float x, float y);
void map_generate(unsigned int seed, bool generate_walls);
void map_sync_player_to_entities();
bool map_is_inside(int x, int y);
#endif