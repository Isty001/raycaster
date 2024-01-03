#ifndef VECTOR
#define VECTOR

#include <math.h>

typedef struct {
    double x;
    double y;
} Vector;

#define vector(_x, _y) ((Vector){.x = (_x), .y = (_y)})

#define vector_reverse(vec) ((Vector){.x = (-vec.x), .y = (-vec.y)}) 

#define vector_angle(vec) ((atan2(vec.y, vec.x) + M_PI) / M_PI * 180);

Vector vector_rotate(Vector vector, double rotation);

#endif
