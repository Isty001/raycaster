#include "vector.h"
#include <math.h>

Vector vector_rotate(Vector vector, double rotation)
{
    return (Vector) {
        .x = vector.x * cos(rotation) - vector.y * sin(rotation),
        .y = vector.x * sin(rotation) + vector.y * cos(rotation)
    };
}
