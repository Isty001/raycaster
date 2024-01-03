#include "state.h"
#include "renderer.h"
#include <GL/freeglut_std.h>
#include <stdio.h>
#include <stdlib.h>

static int TIME_PREV = 0;
static int TIME_NOW  = 0;

static State *STATE = NULL;

void state_init(Player *player, Map *map)
{
    STATE                 = malloc(sizeof(State));
    STATE->frame_duration = 0;
    STATE->player         = player;
    STATE->map            = map;

    renderer_init();
}

const State *state_get(void)
{
    return STATE;
}

void state_set_time(int time)
{
    TIME_PREV = TIME_NOW;
    TIME_NOW  = time;

    STATE->frame_duration = TIME_NOW - TIME_PREV;

    if (STATE->frame_duration < 0) {
        STATE->frame_duration = 0;
    }
}

void state_cleanup(void)
{
    player_cleanup(STATE->player);
    map_cleanup(STATE->map);
    texture_cleanup();
    renderer_cleanup();

    free(STATE);
}
