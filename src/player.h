#ifndef PLAYER
#define PLAYER

#include "geometry.h"

typedef struct {
    struct {
        Vector vector;
        double angle;
    } direction;
    struct {
        Vector plane;
        double pitch;
        double z;
    } camera;
    Point pos;
    struct {
        double movement;
        double rotation;
    } speed;
} Player;

Player *player_load(void);

void player_cleanup(Player *player);

#endif
