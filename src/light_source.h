#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "map.h"
#include "texture.h"

typedef struct LightSource {
    double x;
    double y;
    float radius;
    float brightness;
    RGBA color;
} LightSource;

#define LIGHT_MAP_WIDTH 32
#define LIGHT_MAP_HEIGHT 32

typedef struct LightMap {
    RGBA pixels[LIGHT_MAP_WIDTH][LIGHT_MAP_HEIGHT];
} LightMap;

typedef struct {
    RGBA color;
    float brightness;
} Lighting;

Lighting light_source_get_color(const Map *map, const Cell *cell, Point point);

void light_map_generate(Map *map);

#endif
