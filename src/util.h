#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
    double x;
    double y;
} Point;

#define point(_x, _y) (Point){.x = (_x), .y = (_y)}

typedef enum {
    ORIENTATION_NS,
    ORIENTATION_EW
} LineSegmentOrientation;

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

typedef struct {
    double x;
    double y;
} Vector;

#define vector(_x, _y) ((Vector){.x = (_x), .y = (_y)})

#define vector_reverse(vec) ((Vector){.x = (-vec.x), .y = (-vec.y)}) 

#define vector_angle(vec) ((atan2(vec.y, vec.x) + M_PI) / M_PI * 180);

Vector vector_rotate(Vector vector, double rotation);

#endif
