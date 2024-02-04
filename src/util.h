#ifndef UTIL_H
#define UTIL_H

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef enum {
    SIDE_HORIZONTAL = 0,
    SIDE_VERTICAL   = 1,
} CellSide;

#endif
