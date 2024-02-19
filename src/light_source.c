#include "light_source.h"
#include "geometry.h"
#include "util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    bool available;
    CellSide side;
} RayCastResult;

static RayCastResult ray_cast_to_point(const Map *map, const LightSource *source, Point point)
{
    RayCastResult result = {0};

    if (source->radius < hypot(source->x - point.x, source->y - point.y)) {
        return result;
    }

    Vector ray_dir = vector(source->x - point.x, source->y - point.y);

    // Make sure to always start from the outside of the current Cell
    /* if (ray_dir.y < 0) { */
    /*     point.y += 0.00001; */
    /* } else { */
    /*     point.y -= 0.00001; */
    /* } */

    /* if (ray_dir.x < 0) { */
    /*     point.x += 0.00001; */
    /* } else { */
    /*     point.x -= 0.00001; */
    /* } */

    ray_dir = vector(source->x - point.x, source->y - point.y);

    // Prevents the ray from keep going in a single direction, and miss the source cell
    ray_dir.x += 0.001;
    ray_dir.y += 0.001;

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
            result.side = SIDE_HORIZONTAL;
        } else {
            side_dist_y += ray_unit_step_size_y;
            point.y += step_y;
            result.side = SIDE_VERTICAL;
        }

        if ((int)point.x == (int)source->x && (int)point.y == (int)source->y) {
            result.available = true;

            return result;
        }

        if (false == map_get_cell(map, (int)point.x, (int)point.y)->wall.empty) {
            return result;
        }
    }

    return result;
}

typedef struct {
    RGBA *colors;
    uint8_t color_count;
} RGBAList;

typedef struct LightMapCache {
    LightMap *light_map;
    RGBAList *color_lists;
    double *brightness;
    struct LightMapCache *next;
    const Texture *texture;
} LightMapCache;

static LightMapCache *LIGHT_MAP_CACHE_LIST = NULL;

static LightMapCache *append_to_cache(LightMap *light_map, const Texture *texture, LightMapCache *prev_cache)
{
    LightMapCache *cache = calloc(1, sizeof(LightMapCache));
    cache->light_map     = light_map;
    cache->texture       = texture;
    cache->color_lists   = calloc(1, texture->width * texture->height * sizeof(RGBAList));
    cache->brightness    = calloc(1, texture->width * texture->height * sizeof(double));
    cache->next          = NULL;

    if (NULL == LIGHT_MAP_CACHE_LIST) {
        LIGHT_MAP_CACHE_LIST = cache;
    } else {
        prev_cache->next = cache;
    }

    return cache;
}

static LightMapCache *get_cache(LightMap *light_map, const Texture *texture)
{
    LightMapCache *cache      = LIGHT_MAP_CACHE_LIST;
    LightMapCache *prev_cache = NULL;

    while (cache) {
        if (cache->light_map == light_map) {
            return cache;
        }

        prev_cache = cache;
        cache      = cache->next;
    }

    return append_to_cache(light_map, texture, prev_cache);
}

static void apply_light_source_to_polygon(const Map *map, const LightSource *source, const Cell *cell, Point cell_point)
{
    const Texture *texture = cell->wall.texture;
    double step            = 1.0 / texture->width;

    for (unsigned int i = 0; i < cell->wall.polygon->segment_count; i++) {
        const PolygonSegment *poly_segment = &cell->wall.polygon->segments[i];
        const LineSegment *line            = &poly_segment->line;

        Vector line_direction = vector_multiply(vector(line->end.x - line->start.x, line->end.y - line->start.y), step);
        Point point           = line->start;

        for (unsigned int x = 0; x < texture->width; x++) {

            RayCastResult result = ray_cast_to_point(map, source, point);

            if (result.available) {
                Vector vector = vector(point.x - source->x, point.y - source->y);
                PolygonyHit hit = polygon_hit(point(source->x, source->y), cell->wall.polygon, &vector, cell_point, result.side);

                if (hit.exists && hit.intersected_segment == poly_segment) {
                    double dist       = hypot(source->x - point.x, source->y - point.y);
                    double brightness = fabs((1 - (dist / (source->radius))) * source->brightness);

                    for (unsigned int y = 0; y < texture->height; y++) {
                        LightMapCache *cache = get_cache(poly_segment->light_map, texture);

                        unsigned int position       = x * texture->height + y;
                        cache->brightness[position] = max(cache->brightness[position], brightness);

                        if (source->color.a != 0) {
                            RGBAList *color_list = &cache->color_lists[position];

                            color_list->color_count++;
                            color_list->colors                              = realloc(color_list->colors, color_list->color_count * sizeof(RGBA));
                            color_list->colors[color_list->color_count - 1] = source->color;
                        }
                    }
                }
            }

            point = point_move(point, line_direction);
        }
    }
}

static void apply_light_source_to_cell(const Map *map, const LightSource *source, const Cell *cell, Point point)
{
    if (cell->wall.polygon) {
        apply_light_source_to_polygon(map, source, cell, point);
    }
}

static void compile_light_maps(void)
{
    LightMapCache *cache = LIGHT_MAP_CACHE_LIST;
    LightMapCache *next  = NULL;

    while (cache) {
        const Texture *texture = cache->texture;
        LightMap *light_map    = cache->light_map;

        if (NULL == light_map->brightness) {
            light_map->brightness = calloc(1, texture->width * texture->height * sizeof(double));
        }

        if (NULL == light_map->colors) {
            light_map->colors = calloc(1, texture->width * texture->height * sizeof(RGBA));
        }

        for (unsigned int x = 0; x < texture->width; x++) {
            for (unsigned int y = 0; y < texture->height; y++) {
                unsigned int position = x * texture->height + y;

                RGBAList list     = cache->color_lists[position];
                double brightness = cache->brightness[position];
                double ratio      = 1.0 / list.color_count;

                unsigned int r = 0, g = 0, b = 0;

                for (unsigned int i = 0; i < list.color_count; i++) {
                    r += list.colors[i].r * ratio;
                    g += list.colors[i].g * ratio;
                    b += list.colors[i].b * ratio;
                }

                r = min(255, r);
                g = min(255, g);
                b = min(255, b);

                light_map->colors[position]     = (RGBA){.r = r, .g = g, .b = b, .a = 255};
                light_map->brightness[position] = brightness;

                free(cache->color_lists[position].colors);
            }
        }

        next = cache->next;

        free(cache->color_lists);
        free(cache->brightness);
        free(cache);
        cache = next;
    }
}

void light_map_generate(Map *map)
{
    map->light_source_count = 2;
    map->light_sources      = malloc(map->light_source_count * sizeof(LightSource));
    map->light_sources[0]   = (LightSource){.x = 20.0, .y = 6.7, .radius = 4, .brightness = 2, .color = (RGBA){.r = 255, .g = 0, .b = 0, .a = 255}};
    map->light_sources[1] = (LightSource){.x = 21.0, .y = 3.0, .radius = 3.0, .brightness = 2, .color = (RGBA){.r = 0, .g = 255, .b = 0, .a = 255}};

    for (int i = 0; i < map->light_source_count; i++) {
        const LightSource *source = &map->light_sources[i];

        Cell *cell = map_get_cell(map, (int)source->x, (int)source->y);

        Cell **checked_cells            = NULL;
        unsigned int checked_cell_count = 0;

        Vector ray_dir;
        double angle = 0;

        while (angle <= 360) {
            ray_dir = vector_rotate(vector(1, 1), deg_to_rad(angle));

            angle += 0.5;

            Point point = point(source->x, source->y);

            double side_dist_x;
            double side_dist_y;

            double ray_unit_step_size_x = (ray_dir.x == 0) ? 1e30 : fabs(1.0 / ray_dir.x); // 1e30 -> prevent divsion by zero
            double ray_unit_step_size_y = (ray_dir.y == 0) ? 1e30 : fabs(1.0 / ray_dir.y);

            double step_x;
            double step_y;

            CellSide side;

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

                double dist  = side == SIDE_HORIZONTAL ? side_dist_x : side_dist_y;
                double ray_x = (source->x + dist * ray_dir.x);
                double ray_y = (source->y + dist * ray_dir.y);

                if (source->radius + 2 < hypot(source->x - ray_x, source->y - ray_y)) {
                    break;
                }

                for (unsigned int cell_i = 0; cell_i < checked_cell_count; cell_i++) {
                    if (checked_cells[cell_i] == cell) {
                        goto skip_cell;
                    }
                }

                apply_light_source_to_cell(map, source, cell, point((int)point.x, (int)point.y));

                checked_cell_count++;
                checked_cells                         = realloc(checked_cells, checked_cell_count * sizeof(Cell *));
                checked_cells[checked_cell_count - 1] = cell;

            skip_cell:;
            }
        }

        checked_cell_count = 0;
        free(checked_cells);
    }

    compile_light_maps();

    /* exit(0); */
}

void light_map_cleanup(LightMap *light_map)
{
    free(light_map->brightness);
    free(light_map->colors);
    free(light_map);
}
