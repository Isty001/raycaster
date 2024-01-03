#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
    double x;
    double y;
} Point;

bool line_intersects(Point p1, Point q1, Point p2, Point q2);

#endif
