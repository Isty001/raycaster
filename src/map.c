#include "map.h"
#include "geometry.h"
#include "light_source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
static int worldMap[MAP_WIDTH][MAP_HEIGHT] =
{
  {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,2,0,0,1,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,2,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,1,0,2,0,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

static int CEILING_MAP[MAP_WIDTH][MAP_HEIGHT] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

/* static int CEILING_MAP[MAP_WIDTH][MAP_HEIGHT] = */
/* { */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, */
/*   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} */
/* }; */
// clang-format on

#define numSprites 20

static Sprite sprites[numSprites] = {
    {.x = 20.5, .y = 11.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400}, // green light in front of playerstart
    // green lights in every room
    {.x = 20, .y = 6.7, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},
    {.x = 21, .y = 3, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},

    {.x = 10.0, .y = 4.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},
    {.x = 10.0, .y = 12.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},
    {.x = 3.5, .y = 6.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},
    {.x = 3.5, .y = 20.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},
    {.x = 3.5, .y = 14.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},
    {.x = 14.5, .y = 20.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},

    // row of pillars in front of wall: fisheye test
    {.x = 17.5, .y = 10.5, .id = 5002, .texture = NULL, .size = 1, .vertical_offset = 0},
    {.x = 17.5, .y = 11.5, .id = 5002, .texture = NULL, .size = 1, .vertical_offset = 0},
    {.x = 17.5, .y = 12.5, .id = 5002, .texture = NULL, .size = 1, .vertical_offset = 0},

    // some barrels around the map
    {.x = 21.5, .y = 1.5, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 15.5, .y = 1.5, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 16.0, .y = 1.8, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 16.2, .y = 1.2, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 3.5, .y = 2.5, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 9.5, .y = 15.5, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 10.0, .y = 15.1, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
    {.x = 10.5, .y = 15.8, .id = 5000, .texture = NULL, .size = 0.5, .vertical_offset = 300},
};

/* static PolygonSegment POLYGON_SEGMENTS[] = {{.line = {.start = point(17, 4.0), .end = point(17, 4.99999999)}, .light_map = NULL}, */
/*                                             {.line = {.start = point(17, 5), .end = point(18, 4.99999999)}, .light_map = NULL}, */
/*                                             {.line = {.start = point(18, 5), .end = point(17.99999999, 4)}, .light_map = NULL}, */
/*                                             {.line = {.start = point(18, 4), .end = point(17.00000001, 4)}, .light_map = NULL}}; */

static PolygonSegment POLYGON_SEGMENTS[] = {{.line = {.start = point(18, 4.0), .end = point(18, 4.99999999)}, .light_map = NULL},
                                            };

static Polygon POLYGON = {
    .segment_count = 1,
    .segments      = POLYGON_SEGMENTS,
};

static void load_sprites(Map *map)
{
    map->sprite_count = numSprites;
    map->sprites      = malloc(map->sprite_count * sizeof(Sprite));

    for (unsigned int i = 0; i < map->sprite_count; i++) {
        map->sprites[i] = sprites[i];

        map->sprites[i].texture = texture_get(sprites[i].id);
    }
}

Map *map_load(void)
{
    Map *map                = malloc(sizeof(Map));
    map->cells              = malloc(MAP_HEIGHT * sizeof(Cell *));
    map->general_brightness = 0.20;

    load_sprites(map);

    for (int x = 0; x < MAP_WIDTH; x++) {
        map->cells[x] = malloc(MAP_HEIGHT * sizeof(Cell));

        for (int y = 0; y < MAP_HEIGHT; y++) {
            map->cells[x][y].wall.texture = texture_get(worldMap[x][y]);

            if (worldMap[x][y] == 0) {
                map->cells[x][y].wall.empty = true;
            } else {
                map->cells[x][y].wall.empty = false;
            }

            if (x == 17 && y == 4) {
                for (unsigned int pi = 0; pi < POLYGON.segment_count; pi++) {
                    ((PolygonSegment *)POLYGON.segments)[pi].light_map = calloc(1, sizeof(LightMap));
                }

                map->cells[x][y].wall.polygon = &POLYGON;
            } else {
                if (false == map->cells[x][y].wall.empty) {
                    /* if (17 == x && 4 == y) { */
                        /* printf("ASD"); */
                        /* exit(0); */
                    /* } */
                    Point bottom_left  = point(x, y);
                    Point bottom_right = point(x, y + 1);

                    Point top_right = point(x + 1, y + 1);
                    Point top_left  = point(x + 1, y);

                    Polygon *polygon       = malloc(sizeof(Polygon));
                    polygon->segment_count = 4;

                    PolygonSegment *segments = malloc(4 * sizeof(PolygonSegment));

                    segments[0] = (PolygonSegment){.line = line_segment(point(x, y), point(x + 1, y)), .light_map = calloc(1, sizeof(LightMap))};
                    segments[1] = (PolygonSegment){.line = line_segment(point(x, y + 1), point(x + 1, y + 1)), .light_map = calloc(1, sizeof(LightMap))};

                    segments[2] = (PolygonSegment){.line = line_segment(point(x, y + 0.00000001), point(x, y + 0.9999999)), .light_map = calloc(1, sizeof(LightMap))};
                    segments[3] = (PolygonSegment){.line = line_segment(point(x + 1, y + 0.00000001), point(x + 1, y + 0.9999999)), .light_map = calloc(1, sizeof(LightMap))};

                    polygon->segments = segments;

                    map->cells[x][y].wall.polygon = polygon;
                } else {
                    map->cells[x][y].wall.polygon = NULL;
                }
            }

            map->cells[x][y].floor.texture   = texture_get(7);
            map->cells[x][y].floor.empty     = false;
            map->cells[x][y].floor.light_map = calloc(1, sizeof(LightMap));

            map->cells[x][y].ceiling.texture   = texture_get(3);
            map->cells[x][y].ceiling.empty     = false;
            map->cells[x][y].ceiling.light_map = calloc(1, sizeof(LightMap));

            if (CEILING_MAP[x][y] == 2) {
                map->cells[x][y].ceiling.texture = NULL;
                map->cells[x][y].ceiling.empty   = true;
            }
        }
    }

    map->sky_texture = texture_get(9);

    light_map_generate(map);

    return map;
}

void map_cleanup(Map *map)
{
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            Cell *cell = &map->cells[x][y];

            if (cell->wall.polygon) {
                if (&POLYGON != cell->wall.polygon) {

                    for (int si = 0; si < cell->wall.polygon->segment_count; si++) {
                        light_map_cleanup(cell->wall.polygon->segments[si].light_map);
                    }

                    free((void *)cell->wall.polygon->segments);
                    free((void *)cell->wall.polygon);
                }
            }

            light_map_cleanup(cell->ceiling.light_map);
            light_map_cleanup(cell->floor.light_map);
        }

        free(map->cells[x]);
    }

    free(map->light_sources);
    free(map->sprites);
    free(map->cells);
    free(map);
}
