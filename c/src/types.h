#ifndef TYPES_H
#define TYPES_H
typedef struct { float x; float y; } Vec2;
typedef struct { Vec2 pos; float z; int dir; } Entity;
#endif