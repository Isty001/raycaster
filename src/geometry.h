#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "util.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define deg_to_rad(deg) (deg * (M_PI / 180))

typedef struct {
    double x;
    double y;
} Point;

#define point(_x, _y) ((Point){.x = (_x), .y = (_y)})

#define point_move(p, v) point(p.x + v.x, p.y + v.y)

typedef enum { ORIENTATION_NS, ORIENTATION_EW } LineSegmentOrientation;

typedef struct {
    Point start;
    Point end;
} LineSegment;

#define line_segment(_a, _b) ((LineSegment){.start = (_a), .end = (_b)})

typedef struct {
    bool exists;
    Point point;
} Intersection;

Intersection line_segment_intersection(const LineSegment *l1, const LineSegment *l2);

bool line_segment_has_point(const LineSegment *segment, Point point);

typedef struct {
    double x;
    double y;
} Vector;

#define vector(_x, _y) ((Vector){.x = (_x), .y = (_y)})

#define vector_multiply(v, m) vector(v.x *m, v.y *m)

#define vector_reverse(vec) ((Vector){.x = (-vec.x), .y = (-vec.y)})

#define vector_angle(vec) ((atan2(vec.y, vec.x) + M_PI) / M_PI * 180);

Vector vector_rotate(Vector vector, float rotation);

typedef struct {
    LineSegment line;
    struct LightMap *light_map;
} PolygonSegment;

typedef struct {
    const PolygonSegment *segments;
    unsigned int segment_count;
} Polygon;

typedef struct {
    bool exists;
    double rel_dist;
    double wall_x;
    const PolygonSegment *intersected_segment;
} PolygonyHit;

PolygonyHit polygon_hit(Point from, const Polygon *polygon, const Vector *ray_dir, Point cell_point, CellSide side);

#endif
