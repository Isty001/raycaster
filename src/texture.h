#ifndef TEXTURE
#define TEXTURE

#include <stdint.h>

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

static const RGBA FALLBACK_COLOR = {
    .r = 0,
    .g = 250,
    .b = 0,
    .a = 255,
};

static const RGBA INVISIBLE_COLOR = {
    .r = 255,
    .g = 0,
    .b = 255,
    .a = 255,
};

#define color_is_transparent(c) (c.r == INVISIBLE_COLOR.r && c.g == INVISIBLE_COLOR.g && c.b == INVISIBLE_COLOR.b)

void texture_load(const char *path);

const Texture *texture_get(int id);

#define texture_get_color(texture, x, y) ((x > (int)texture->width -1 || y > (int)texture->height - 1) ? FALLBACK_COLOR : texture->pixels[y * texture->width + x])

void texture_cleanup(void);

#endif
