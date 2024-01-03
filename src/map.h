#ifndef MAP_H
#define MAP_H

#include "texture.h"
#include <stdbool.h>

typedef enum {
    PROP_EMPTY       = 1,
    PROP_HAS_TRANSPARENCY = 2,
    PROP_THIN = 4,
} CellPartProperty;

typedef struct {
    const Texture *texture;
    unsigned int properties;
} CellPart;

typedef struct {
    CellPart ceiling;
    CellPart wall;
    CellPart floor;
} Cell;

typedef struct {
    unsigned int id;
    double x;
    double y;
    int vertical_offset;
    double size;
    const Texture *texture;
} Sprite;

typedef struct Map {
    Cell **cells;
    Sprite *sprites;
    unsigned int sprite_count;
    const Texture *sky_texture;
} Map;

Map *map_load(void);

const Cell *map_get_cell(const Map *map, int x, int y);

void map_cleanup(Map *map);

#endif
