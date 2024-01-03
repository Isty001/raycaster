#ifndef PLAYER
#define PLAYER

#include "vector.h"

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
    struct {
        double x;
        double y;
    } pos;
    struct {
        double movement;
        double rotation;
    } speed;
} Player;

Player *player_load(void);

void player_cleanup(Player *player);

#endif
