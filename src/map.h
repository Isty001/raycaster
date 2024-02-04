#ifndef MAP_H
#define MAP_H

#include "texture.h"
#include "geometry.h"
#include <stdbool.h>
#include <stdint.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24


typedef struct {
    const Texture *texture;
    struct LightMap *light_map;
    bool empty;
} VerticalCellPart;

typedef struct {
    const Texture *texture;
    const Polygon *polygon;
    bool empty;
} CellWall;

typedef struct {
    VerticalCellPart ceiling;
    CellWall wall;
    VerticalCellPart floor;
} Cell;

typedef struct {
    int id;
    float x;
    float y;
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
    struct LightSource *light_sources;
    unsigned int light_source_count;
    float general_brightness;
} Map;

Map *map_load(void);

#define map_get_cell(map, x, y) (&map->cells[(x > MAP_WIDTH - 1) ? MAP_WIDTH - 1 : (x < 0 ? 0 : x)][(y > MAP_HEIGHT - 1) ? MAP_WIDTH - 1 : (y < 0 ? 0 : y)])

void map_cleanup(Map *map);

#endif
