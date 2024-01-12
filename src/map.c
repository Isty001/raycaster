#include "map.h"
#include "util.h"
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
  {2,2,0,0,2,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,2,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,1,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
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

#define numSprites 19

static Sprite sprites[numSprites] = {
    {.x = 20.5, .y = 11.5, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400}, // green light in front of playerstart
    // green lights in every room
    {.x = 20.5, .y = 6.7, .id = 5001, .texture = NULL, .size = 0.2, .vertical_offset = -400},


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

static LineSegment SEGMENTS[] = {
    {.start = point(17.5, 5.0), .end = point(17.5, 4.0)},
    {.start = point(17, 4.5), .end = point(18, 4.5)},
};

static Polygon POLYGON = {
    .count    = 2,
    .segments = SEGMENTS,
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

static double SOURCE_DIST = 5;

// TODO move this to one place
typedef enum {
    SIDE_HORIZONTAL = 0,
    SIDE_VERTICAL   = 1,
} HitSide;

static bool cell_has_light_source(const Cell *cell, const LightSource *light_source)
{
    for (unsigned int i = 0; i < cell->light_source_count; i++) {
        if (cell->light_sources[i] == light_source) {
            return true;
        }
    }

    return false;
}

static void map_generate_lightmap(Map *map)
{
    map->light_source_count = 1;
    map->light_sources      = malloc(map->light_source_count * sizeof(LightSource));
    map->light_sources[0]   = (LightSource){.x = 20.5, .y = 6.7, .radius = 4, .color = (RGBA){.r = 255, .g = 0, .b = 0, .a = 255}};

    for (int i = 0; i < map->light_source_count; i++) {
        const LightSource *source = &map->light_sources[i];

        Vector ray_dir = vector(1, 1);

        for (int x = 0; x < 360; x++) {
            ray_dir = vector_rotate(ray_dir, x);

            Point point = point(source->x, source->y);

            double side_dist_x;
            double side_dist_y;

            double ray_unit_step_size_x = (ray_dir.x == 0) ? 1e30 : fabs(1.0 / ray_dir.x); // 1e30 -> prevent divsion by zero
            double ray_unit_step_size_y = (ray_dir.y == 0) ? 1e30 : fabs(1.0 / ray_dir.y);

            double step_x;
            double step_y;

            HitSide side;

            if (ray_dir.x < 0) {
                step_x      = -1;
                side_dist_x = (point.x - (int)point.x) * ray_unit_step_size_x;
            } else {
                step_x      = 1;
                side_dist_x = ((int)point.x + 1.0 - point.x) * ray_unit_step_size_x;
            }
            if (ray_dir.y < 0) {
                step_y      = -1;
                side_dist_y = (point.y - (int)point.y) * ray_unit_step_size_y;
            } else {
                step_y      = 1;
                side_dist_y = ((int)point.y + 1.0 - point.y) * ray_unit_step_size_y;
            }

            Cell *cell;

            for (unsigned int distance = 0; distance < source->radius; distance++) {
                if (side_dist_x < side_dist_y) {
                    side_dist_x += ray_unit_step_size_x;
                    point.x += step_x;
                    side = SIDE_HORIZONTAL;
                } else {
                    side_dist_y += ray_unit_step_size_y;
                    point.y += step_y;
                    side = SIDE_VERTICAL;
                }

                cell = map_get_cell(map, (int)point.x, (int)point.y);

                if (!cell_has_light_source(cell, source)) {
                    cell->light_source_count++;
                    cell->light_sources                               = realloc(cell->light_sources, cell->light_source_count * sizeof(LightSource *));
                    cell->light_sources[cell->light_source_count - 1] = source;
                }

                /* printf("x: %f y: %f\n", point.x, point.y); */
            }
        }
    }
}

Map *map_load(void)
{
    Map *map   = malloc(sizeof(Map));
    map->cells = malloc(MAP_HEIGHT * sizeof(Cell *));

    load_sprites(map);

    for (int x = 0; x < MAP_WIDTH; x++) {
        map->cells[x] = malloc(MAP_HEIGHT * sizeof(Cell));

        for (int y = 0; y < MAP_HEIGHT; y++) {
            map->cells[x][y].wall.texture       = texture_get(worldMap[x][y]);
            map->cells[x][y].light_source_count = 0;
            map->cells[x][y].light_sources      = NULL;

            if (worldMap[x][y] == 0) {
                map->cells[x][y].wall.empty = true;
            } else {
                map->cells[x][y].wall.empty = false;
            }

            if (x == 17 && y == 4) {
                map->cells[x][y].wall.polygon = &POLYGON;
            } else {
                map->cells[x][y].wall.polygon = NULL;
            }

            map->cells[x][y].floor.texture   = texture_get(7);
            map->cells[x][y].ceiling.texture = texture_get(3);

            if (CEILING_MAP[x][y] == 2) {
                map->cells[x][y].ceiling.texture = NULL;
            }
        }
    }

    map->sky_texture = texture_get(9);

    map_generate_lightmap(map);

    return map;
}

void map_cleanup(Map *map)
{
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            free(map->cells[x][y].light_sources);
        }

        free(map->cells[x]);
    }

    free(map->light_sources);
    free(map->sprites);
    free(map->cells);
    free(map);
}
