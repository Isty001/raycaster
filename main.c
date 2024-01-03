#include "src/control.h"
#include "src/renderer.h"
#include "src/state.h"
// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
// clang-format on
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void display(void)
{
    state_set_time(glutGet(GLUT_ELAPSED_TIME));

    const State *state = state_get();

    control_apply(state->map, state->player, state->frame_duration);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render(state->map, state->player, state->frame_duration);

    glutPostRedisplay();
    glutSwapBuffers();
}

void set_resolution(int width, int height)
{
    glViewport(0, 0, width, height);

    renderer_set_resolution(width, height);
}

// https://lodev.org/cgtutor/raycasting.html
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("asd");
    glutFullScreen();

    glewInit();

    texture_load("./assets/textures/walls");
    texture_load("./assets/textures/sprites");

    state_init(player_load(), map_load());

    glutSetCursor(GLUT_CURSOR_NONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glutDisplayFunc(display);
    glutReshapeFunc(set_resolution);
    glutKeyboardFunc(control_key_down);
    glutPassiveMotionFunc(control_motion);
    glutKeyboardUpFunc(control_key_up);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutMainLoop();

    state_cleanup();

    return 0;
}
