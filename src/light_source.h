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

typedef struct LightMap {
    RGBA *colors;
    float *brightness;
} LightMap;

void light_map_generate(Map *map);

void light_map_cleanup(LightMap *light_map);

#endif
