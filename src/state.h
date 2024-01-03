#ifndef STATE_H
#define STATE_H

#include "player.h"
#include "map.h"

typedef struct {
    Player *player;
    Map *map;
    int frame_duration;
} State;

void state_init(Player *player, Map *map);

const State *state_get(void);

void state_set_time(int time);

void state_cleanup(void);

#endif
