#ifndef RENDERER
#define RENDERER

#include "map.h"
#include "player.h"
#include "state.h"

void renderer_init(void);

void renderer_set_resolution(int width, int height);

void render(const Map *map, const Player *player, double frame_duration);

void renderer_cleanup(void);

#endif
