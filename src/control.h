#ifndef CONTROLS
#define CONTROLS

#include "player.h"
#include "map.h"

void control_key_down(unsigned char key, int x, int y);

void control_key_up(unsigned char key, int x, int y);

void control_apply(const Map *map, Player *player, int frame_duration);

void control_motion(int x, int y);

#endif
