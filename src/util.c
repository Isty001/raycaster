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

Vector vector_rotate(Vector vector, double rotation)
{
    return (Vector){
        .x = vector.x * cos(rotation) - vector.y * sin(rotation),
        .y = vector.x * sin(rotation) + vector.y * cos(rotation),
    };
}

Vector vector_get_unit(Vector vector)
{
    double magnitude = hypot(vector.x, vector.y);

    return vector(
        vector.x / magnitude,
        vector.y / magnitude
    );
}
