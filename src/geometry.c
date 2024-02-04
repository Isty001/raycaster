#include "geometry.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * https://stackoverflow.com/a/1968345
 */
Intersection line_segment_intersection(const LineSegment *l1, const LineSegment *l2)
{
    if (NULL == l1 || NULL == l2) {
        return (Intersection){.exists = false, .point = point(0, 0)};
    }

    Point s1;
    Point s2;

    s1.x = l1->end.x - l1->start.x;
    s1.y = l1->end.y - l1->start.y;

    s2.x = l2->end.x - l2->start.x;
    s2.y = l2->end.y - l2->start.y;

    double s = (-s1.y * (l1->start.x - l2->start.x) + s1.x * (l1->start.y - l2->start.y)) / (-s2.x * s1.y + s1.x * s2.y);
    double t = (s2.x * (l1->start.y - l2->start.y) - s2.y * (l1->start.x - l2->start.x)) / (-s2.x * s1.y + s1.x * s2.y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        double x = l1->start.x + (t * s1.x);
        double y = l1->start.y + (t * s1.y);

        return (Intersection){.exists = true, .point = point(x, y)};
    }

    return (Intersection){.exists = false, .point = point(0, 0)};
}

// @ TODO remove
bool line_segment_has_point(const LineSegment *segment, Point point)
{
    return 0.001 > (hypot(segment->start.x - point.x, segment->start.y - point.y) + hypot(segment->end.x - point.x, segment->end.y - point.y)) -
                       hypot(segment->start.x - segment->end.x, segment->start.y - segment->end.y);
}

Vector vector_rotate(Vector vector, float rotation)
{
    return (Vector){
        .x = vector.x * cos(rotation) - vector.y * sin(rotation),
        .y = vector.x * sin(rotation) + vector.y * cos(rotation),
    };
}

PolygonyHit polygon_hit(Point from, const Polygon *polygon, const Vector *ray_dir, Point cell_point, CellSide side)
{
    // Extend the ray to make sure it hits the polygon
    double ray_x = from.x + ray_dir->x * 1000.0;
    double ray_y = from.y + ray_dir->y * 1000.0;

    const LineSegment ray_segment = line_segment(point(from.x, from.y), point(ray_x, ray_y));

    PolygonyHit hit = {.exists = false, .wall_x = 9999999999, .rel_distance = 999999999};

    for (unsigned int i = 0; i < polygon->segment_count; i++) {
        // @TODO return the actual line segment hit
        const LineSegment *poly_line_segment = &polygon->segments[i].line;
        const Intersection intersection      = line_segment_intersection(&ray_segment, poly_line_segment);

        if (intersection.exists) {
            hit.exists = true;

            float dist;

            if (side == SIDE_HORIZONTAL) {
                dist = fabs(from.x > cell_point.x ? ceil(cell_point.x + 0.0001) - intersection.point.x : floor(cell_point.x) - intersection.point.x);
            } else {
                dist = fabs(from.y > cell_point.y ? ceil(cell_point.y + 0.0001) - intersection.point.y : floor(cell_point.y) - intersection.point.y);
            }

            if (dist < hit.rel_distance) {
                hit.rel_distance        = dist;
                hit.wall_x              = hypot(poly_line_segment->start.x - intersection.point.x, poly_line_segment->start.y - intersection.point.y);
                hit.intersected_segment = &polygon->segments[i];
            }
        }
    }

    return hit;
}
