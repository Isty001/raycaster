#ifndef MAP_H
#define MAP_H

#include "texture.h"
#include "util.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    Point p1;
    Point p2;
} ThinWall;

typedef enum {
    PROP_EMPTY = 1,
    PROP_THIN  = 2,
} CellPartProperty;

typedef struct {
    const Texture *texture;
    uint32_t properties;
} CellPart;

typedef struct {
    CellPart ceiling;
    CellPart wall;
    CellPart floor;
} Cell;

typedef struct {
    uint32_t id;
    double x;
    double y;
    // TODO make this relvative to the resolution
    int32_t vertical_offset;
    double size;
    const Texture *texture;
} Sprite;

typedef struct {
    Cell **cells;
    Sprite *sprites;
    uint32_t sprite_count;
    const Texture *sky_texture;
} Map;

Map *map_load(void);

const Cell *map_get_cell(const Map *map, int x, int y);

void map_cleanup(Map *map);

#endif
