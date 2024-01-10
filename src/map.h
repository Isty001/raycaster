#ifndef MAP_H
#define MAP_H

#include "texture.h"
#include "util.h"
#include <stdbool.h>
#include <stdint.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

typedef struct {
    const LineSegment *segments;
    unsigned int count;
} Polygon;

#define CELL_PART_FIELDS                                                                                                                                                                               \
    const Texture *texture;                                                                                                                                                                            \

typedef struct {
    CELL_PART_FIELDS
} CellPart;

typedef struct {
    CELL_PART_FIELDS
    const Polygon *polygon;
    bool empty;
} CellWall;

typedef struct {
    double x;
    double y;
    double radius;
    RGBA color;
} LightSource;

typedef struct {
    CellPart ceiling;
    CellWall wall;
    CellPart floor;
    const LightSource **light_sources;
    unsigned int light_source_count;
} Cell;

typedef struct {
    int id;
    double x;
    double y;
    // TODO make this relative to the resolution
    int vertical_offset;
    double size;
    const Texture *texture;
} Sprite;

typedef struct {
    Cell **cells;
    Sprite *sprites;
    unsigned int sprite_count;
    const Texture *sky_texture;
    LightSource *light_sources;
    unsigned int light_source_count;
} Map;

Map *map_load(void);

#define map_get_cell(map, x, y) \
    (&map->cells[(x > MAP_WIDTH - 1) ? MAP_WIDTH - 1 : (x < 0 ? 0 : x)][(y > MAP_HEIGHT - 1) ? MAP_WIDTH - 1 : (y < 0 ? 0 : y)])

void map_cleanup(Map *map);

#endif
