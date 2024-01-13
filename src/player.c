#include "player.h"
#include <stdlib.h>

Player *player_load(void)
{
    Player *player = malloc(sizeof(Player));
    player->pos.x = 21;
    player->pos.y = 4;
    player->direction.vector = vector(-1, 0);
    player->direction.angle = vector_angle(player->direction.vector);
    player->camera.plane = vector(0, 0.66);
    player->camera.pitch = 50;
    player->camera.z = 0;
    player->speed.movement = 0.003;
    player->speed.rotation = 0.003;

    return player;
}

void player_cleanup(Player *player)
{
    free(player);
}
