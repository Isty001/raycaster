#include "light_source.h"
#include "geometry.h"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// @TODO
typedef enum {
    SIDE_HORIZONTAL = 0,
    SIDE_VERTICAL   = 1,
} HitSide;

static bool is_light_source_visible(const Map *map, const LightSource *source, const Cell *expected_cell, Point point)
{
    if (source->radius < hypot(source->x - point.x, source->y - point.y)) {
        return false;
    }

    Vector ray_dir = vector((source->x + 0.001) - point.x, source->y - point.y);

    // Make sure to always start from the outside of the current Cell
    if (ray_dir.y < 0) {
        point.y += 0.0001;
    } else {
        point.y -= 0.0001;
    }

    if (ray_dir.x < 0) {
        point.x += 0.0001;
    } else {
        point.x -= 0.0001;
    }

    ray_dir = vector(source->x - point.x, source->y - point.y);
    ray_dir.x += 0.001;
    ray_dir.y += 0.001;

    /* printf("ray_dir.x: %f, ray_dir.y: %f\n", ray_dir.x, ray_dir.y); */

    double ray_unit_step_size_x = (point.x == 0) ? 1e30 : fabs(1.0 / ray_dir.x); // 1e30 -> prevent divsion by zero
    double ray_unit_step_size_y = (point.y == 0) ? 1e30 : fabs(1.0 / ray_dir.y);

    double side_dist_x;
    double side_dist_y;

    double step_x;
    double step_y;

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

    while (1) {
        if (side_dist_x < side_dist_y) {
            side_dist_x += ray_unit_step_size_x;
            point.x += step_x;
        } else {
            side_dist_y += ray_unit_step_size_y;
            point.y += step_y;
        }

        if ((int)point.x == (int)source->x && (int)point.y == (int)source->y) {
            return true;
        }

        if (false == map_get_cell(map, (int)point.x, (int)point.y)->wall.empty) {
            return false;
        }
    }

    return true;
}

Lighting light_source_get_color(const Map *map, const Cell *cell, Point point)
{
    Lighting lighting = {0};

    /* float brightness = 0; */
    /* bool has_color   = false; */

    /* for (unsigned int i = 0; i < cell->light_source_count; i++) { */
    /*     const LightSource *source = cell->light_sources[i]; */

    /*     if (false == is_light_source_visible(map, source, cell, point)) { */
    /*         continue; */
    /*     } */

    /*     float dist = hypot(source->x - point.x, source->y - point.y); */
    /*     float ratio = dist / source->radius; */

    /*     brightness += fabs((1 - (dist / (source->radius))) * source->brightness); */

    /*     if (0 != source->color.a) { */
    /*         lighting.color.r = (lighting.color.r + source->color.r); */
    /*         lighting.color.g = (lighting.color.g + source->color.g); */
    /*         lighting.color.b = (lighting.color.b + source->color.b); */
    /*         lighting.color.a = 255; */

    /*         has_color = true; */
    /*     } */
    /* } */

    /* lighting.brightness = max(map->general_brightness, brightness); */

    return lighting;
}

static void apply_light_source(const Map *map, const LightSource *source, const Cell *cell, Point point)
{
    float step_x = 1.0 / LIGHT_MAP_WIDTH;
    float step_y = 1.0 / LIGHT_MAP_HEIGHT;

    for (unsigned int x = 0; x <= LIGHT_MAP_WIDTH; x++) {
        for (unsigned int y = 0; y <= LIGHT_MAP_HEIGHT; y++) {
            float target_x = point.x + x * step_x;
            float target_y = point.y + x * step_y;


        }
    }
}

void light_map_generate(Map *map)
{
    map->light_source_count = 1;
    map->light_sources      = malloc(map->light_source_count * sizeof(LightSource));
    /* map->light_sources[0]   = (LightSource){.x = 20.0, .y = 6.7, .radius = 4, .brightness = 6, .color = (RGBA){.r = 255, .g = 0, .b = 0, .a = 255}}; */
    map->light_sources[0] = (LightSource){.x = 21.0, .y = 3.0, .radius = 3.0, .brightness = 2, .color = (RGBA){.r = 0, .g = 255, .b = 0, .a = 255}};

    for (int i = 0; i < map->light_source_count; i++) {
        const LightSource *source = &map->light_sources[i];

        Cell *cell = map_get_cell(map, (int)source->x, (int)source->y);

        if (cell->light_map) {
            continue;
        }

        cell->light_map = malloc(sizeof(LightMap));
        memset(&cell->light_map->pixels, 0, LIGHT_MAP_WIDTH * LIGHT_MAP_HEIGHT);

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

            while (1) {
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

                float dist  = side == SIDE_HORIZONTAL ? side_dist_x : side_dist_y;
                float ray_x = (source->x + dist * ray_dir.x);
                float ray_y = (source->y + dist * ray_dir.y);

                if (source->radius + 2 < hypot(source->x - ray_x, source->y - ray_y)) {
                    break;
                }

                apply_light_source(map, source, cell, point((int) point.x, (int) point.y));
            }
        }
    }
}
