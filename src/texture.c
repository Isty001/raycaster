#include "texture.h"
#include "../vendor/tinydir/tinydir.h"
#include "light_source.h"
#include <stdint.h>
#include <stdio.h>

#define TEXTURE_MAX_NAME_SIZE 255

#if (defined(_WIN32) || defined(_MSC_VER)) && !defined(__MINGW32__)

#define strtok_r strtok_s

#endif

static int to_int(char *str)
{
    return NULL == str ? 0 : (int)atoi(str);
}

typedef struct TextureNode TextureNode;

struct TextureNode {
    const Texture *texture;
    TextureNode *next;
};

static TextureNode *TEXTURE_LIST = NULL;

static Texture FALLBACK_TEXTURE;

static void add_texture(const Texture *texture)
{
    TextureNode *node = malloc(sizeof(TextureNode));

    if (!node) {
        // TODO log
        return;
    }

    node->texture = texture;
    node->next    = NULL;

    if (TEXTURE_LIST) {
        TextureNode *prev_head = TEXTURE_LIST;

        TEXTURE_LIST       = node;
        TEXTURE_LIST->next = prev_head;
    } else {
        TEXTURE_LIST = node;
    }
}

static int parse_id(char *name)
{
    char name_copy[TEXTURE_MAX_NAME_SIZE] = {0};
    strncat(name_copy, name, TEXTURE_MAX_NAME_SIZE);

    char *ctx;

    return to_int(strtok_r((char *)name, "_", &ctx));
}

static void parse_size(Texture *texture, char *line)
{
    char *ctx;

    texture->width  = to_int(strtok_r((char *)line, " ", &ctx));
    texture->height = to_int(strtok_r(NULL, " ", &ctx));
}

static void load_file(char *file_path, char *name)
{

    FILE *file = fopen(file_path, "r");

    if (!file) {
        // TODO log
        return;
    }

    int id = parse_id(name);

    if (0 == id || &FALLBACK_TEXTURE != texture_get(id)) {
        // TODO log
        goto cleanup;
    }

    Texture *texture = malloc(sizeof(Texture));
    texture->id      = id;

    if (!texture) {
        // TODO log
        return;
    }

    char line[500];

    // TODO proper header parsing
    // Skip Magic PPM number, and comment line
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);

    fgets(line, sizeof(line), file);
    parse_size(texture, line);

    if (0 == texture->width || 0 == texture->height) {
        free(texture);

        goto cleanup;
    }

    // Skip max color number line
    fgets(line, sizeof(line), file);

    texture->pixels = malloc((size_t)(texture->width * texture->height) * sizeof(RGBA));

    for (int y = 0; y < texture->height; y++) {
        for (int x = 0; x < texture->width; x++) {
            uint8_t r, g, b;

            r = (uint8_t)to_int(fgets(line, sizeof(line), file));
            g = (uint8_t)to_int(fgets(line, sizeof(line), file));
            b = (uint8_t)to_int(fgets(line, sizeof(line), file));

            texture->pixels[y * texture->width + x].r = r;
            texture->pixels[y * texture->width + x].g = g;
            texture->pixels[y * texture->width + x].b = b;
            texture->pixels[y * texture->width + x].a = 255;
        }
    }

    add_texture(texture);

cleanup:
    fclose(file);
}

static void initialize_fallback_texture(void)
{
    FALLBACK_TEXTURE.id     = 0;
    FALLBACK_TEXTURE.width  = -1;
    FALLBACK_TEXTURE.height = -1;

    FALLBACK_TEXTURE.pixels = NULL;
}

void texture_load(const char *dir_path)
{
    initialize_fallback_texture();

    tinydir_dir dir;

    if (-1 == tinydir_open(&dir, dir_path)) {
        return;
    }

    while (dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        if (!file.is_dir) {
            load_file(file.path, file.name);
        }

        if (-1 == tinydir_next(&dir)) {
            break;
        }
    }

    tinydir_close(&dir);
}

const Texture *texture_get(int id)
{
    TextureNode *node = TEXTURE_LIST;

    while (node) {
        if (id == node->texture->id) {
            return node->texture;
        }

        node = node->next;
    }

    return &FALLBACK_TEXTURE;
}

void texture_cleanup(void)
{
    TextureNode *node = TEXTURE_LIST;
    TextureNode *next = NULL;

    while (node) {
        free(node->texture->pixels);
        free((void *)node->texture);
        next = node->next;
        free(node);

        node = next;
    }
}
