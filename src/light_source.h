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

typedef struct {
    RGBA color;
    float brightness;
} Lighting;

Lighting light_source_get_color(const Map *map, const Cell *cell, Point point);

#endif
