#include "util.h"
#include <stdlib.h>

/**
 * https://stackoverflow.com/a/1968345
 */
Intersection line_intersection(Point p0, Point p1, Point p2, Point p3)
{
    Point s1;
    Point s2;

    s1.x = p1.x - p0.x;
    s1.y = p1.y - p0.y;
    s2.x = p3.x - p2.x;
    s2.y = p3.y - p2.y;

    double s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
    double t = (s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

    float i_x;
    float i_y;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        i_x = p0.x + (t * s1.x);
        i_y = p0.y + (t * s1.y);

        return (Intersection){.exists = true, .point = point(i_x, i_y)};
    }

    return (Intersection){.exists = false, .point = point(0, 0)};
}
