#include "light_source.h"
#include "util.h"
#include <math.h>

#include <stdio.h>

static bool is_light_source_visible(const Map *map, const LightSource *source, const Cell *expected_cell, Point point)
{
    Vector ray_dir = vector((source->x + 0.001) - point.x, source->y - point.y);

    // Make sure to always start from the outside of the current Cell
    if (ray_dir.y < 0) {
        point.y += 0.0002;
    } else {
        point.y -= 0.0002;
    }

    if (ray_dir.x < 0) {
        point.x += 0.0002;
    } else {
        point.x -= 0.0002;
    }

    ray_dir = vector(source->x - point.x, source->y - point.y);

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
            /* printf("source.x: %f source.x int: %d source.y: %f source.y int: %d\n", source->x, (int)source->x, source->y, (int) source->y); */
            return true;
        }

        if (false == map_get_cell(map, (int)point.x, (int)point.y)->wall.empty) {
            return false;
        }
    }

    return true;
}

#include <stdio.h>

Lighting light_source_get_color(const Map *map, const Cell *cell, Point point)
{
    Lighting lighting = {0};

    bool first_color             = true;
    unsigned int collected_count = 0;

    for (unsigned int i = 0; i < cell->light_source_count; i++) {
        const LightSource *source = cell->light_sources[i];

        if (false == is_light_source_visible(map, source, cell, point)) {
            continue;
        }

        collected_count++;

        float dist       = hypot(source->x - point.x, source->y - point.y);
        float brightness = fabs((1 - (dist / (source->radius))) * source->brightness);

        lighting.brightness += brightness;

        if (first_color) {
            lighting.color = source->color;
            first_color    = false;

            continue;
        }

        lighting.color.r = (lighting.color.r + source->color.r) / 2;
        lighting.color.g = (lighting.color.g + source->color.g) / 2;
        lighting.color.b = (lighting.color.b + source->color.b) / 2;
    }

    lighting.color.r *= 0.10;
    lighting.color.g *= 0.10;
    lighting.color.b *= 0.10;

    return lighting;
}
