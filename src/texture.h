#ifndef TEXTURE
#define TEXTURE

#include <stdint.h>

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA;

typedef struct {
    int id;
    int width;
    int height;
    RGBA *pixels;
} Texture;

#define color_is_transparent(c) (c.r == 255 && c.g == 0 && c.b == 255)

void texture_load(const char *path);

const Texture *texture_get(unsigned int id);

RGBA texture_get_color(const Texture *texture, int x, int y);

void texture_cleanup(void);

#endif
