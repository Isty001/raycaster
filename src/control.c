#include "control.h"
#include "state.h"
#include "util.h"
#include <GL/freeglut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    KEY_DOWN,
    KEY_UP,
} KeyState;

static struct {
    KeyState w;
    KeyState a;
    KeyState s;
    KeyState d;
    KeyState c;
    KeyState space;
} Keys = {
    .w = KEY_UP,
    .a = KEY_UP,
    .s = KEY_UP,
    .d = KEY_UP,
    .c = KEY_UP,
    .space = KEY_UP
};

void control_key_down(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if ('w' == key) {
        Keys.w = KEY_DOWN;
    } else if ('a' == key) {
        Keys.a = KEY_DOWN;
    } else if ('s' == key) {
        Keys.s = KEY_DOWN;
    } else if ('d' == key) {
        Keys.d = KEY_DOWN;
    } else if ('c' == key) {
        Keys.c = KEY_DOWN;
    } else if (' ' == key) {
        Keys.space = KEY_DOWN;
    } else if (27 == key) { // ESC
        glutLeaveMainLoop();
    }
}

void control_key_up(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if ('w' == key) {
        Keys.w = KEY_UP;
    } else if ('a' == key) {
        Keys.a = KEY_UP;
    } else if ('s' == key) {
        Keys.s = KEY_UP;
    } else if ('d' == key) {
        Keys.d = KEY_UP;
    } else if ('c' == key) {
        Keys.c = KEY_UP;
    } else if (' ' == key) {
        Keys.space = KEY_UP;
    }
}

static void move_player(const Map *map, Player *player, Vector direction, double speed)
{
    int new_x = (int)(player->pos.x + direction.x * speed);

    if (map_get_cell(map, new_x, (int)(player->pos.y))->wall.properties & PROP_EMPTY) {
        player->pos.x += direction.x * speed;
    }

    int new_y = (int)(player->pos.y + direction.y * speed);

    if (map_get_cell(map, (int)(player->pos.x), new_y)->wall.properties & PROP_EMPTY) {
        player->pos.y += direction.y * speed;
    }
}

void control_apply(const Map *map, Player *player, int frame_duration)
{
    double speed = player->speed.movement * frame_duration;

    if ((Keys.w == KEY_DOWN || Keys.s == KEY_DOWN) && (Keys.a == KEY_DOWN || Keys.d == KEY_DOWN)) {
        speed *= 0.7;
    }

    if (Keys.w == KEY_DOWN) {
        move_player(map, player, player->direction.vector, speed);
    }

    if (Keys.s == KEY_DOWN) {
        move_player(map, player, vector_reverse(player->direction.vector), speed);
    }

    if (Keys.d == KEY_DOWN) {
        move_player(map, player, player->camera.plane, speed);
    }

    if (Keys.a == KEY_DOWN) {
        move_player(map, player, vector_reverse(player->camera.plane), speed);
    }

    if (Keys.space == KEY_DOWN) {
        player->camera.z = 200;
    } else if (Keys.c == KEY_DOWN){
        player->camera.z = -200;
    } else {
        player->camera.z = 0;
    }
}

static int PREV_MOUSE_X = -1;
static int PREV_MOUSE_Y = -1;

void control_motion(int x, int y)
{
    if (PREV_MOUSE_X == -1) {
        PREV_MOUSE_X = x;
        PREV_MOUSE_Y = y;

        return;
    }

    Player *player        = state_get()->player;
    /* float frame_duration  = 0; */ // TODO
    double rotation_speed = player->speed.rotation;

    if (Keys.a == KEY_DOWN || Keys.d == KEY_DOWN) {
        rotation_speed -= 0.005;
    }

    if (x < PREV_MOUSE_X) {
        player->direction.vector    = vector_rotate(player->direction.vector, rotation_speed);
        player->direction.angle    = vector_angle(player->direction.vector);
        player->camera.plane = vector_rotate(player->camera.plane, rotation_speed);
    } else if (x > PREV_MOUSE_X) {
        player->direction.vector    = vector_rotate(player->direction.vector, -rotation_speed);
        player->direction.angle    = vector_angle(player->direction.vector);
        player->camera.plane = vector_rotate(player->camera.plane, -rotation_speed);
    }

    if (y < PREV_MOUSE_Y) {
        player->camera.pitch += rotation_speed * 600;
        player->camera.pitch = min(player->camera.pitch, 300);
    } else if (y > PREV_MOUSE_Y) {
        player->camera.pitch -= rotation_speed * 600;
        player->camera.pitch = max(player->camera.pitch, -300);
    }

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // TODO
    if (x < 30 || x > width - 10 || y < 30 || y > height -10) {
        PREV_MOUSE_X = width / 2;
        PREV_MOUSE_Y = height / 2;

        glutWarpPointer(width / 2, height / 2);
    }

    PREV_MOUSE_X = x;
    PREV_MOUSE_Y = y;
}
