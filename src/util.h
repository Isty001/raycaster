#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
    double x;
    double y;
} Point;

#define point(_x, _y) ((Point){.x = (_x), .y = (_y)})


typedef struct {
    bool exists;
    Point point;
} Intersection;

Intersection line_intersection(Point p1, Point q1, Point p2, Point q2);

#endif
