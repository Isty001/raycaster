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

static const RGBA FALLBACK_COLOR = {
    .r = 0,
    .g = 250,
    .b = 0,
    .a = 255,
};

#define color_is_transparent(c) (c.r == 255 && c.g == 0 && c.b == 255)

void texture_load(const char *path);

const Texture *texture_get(int id);

#define texture_get_color(texture, x, y) ((x > (int)texture->width || y > (int)texture->height) ? FALLBACK_COLOR : texture->pixels[y * texture->width + x])

void texture_cleanup(void);

#endif
