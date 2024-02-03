#include "renderer.h"
#include "light_source.h"
#include "map.h"
#include "geometry.h"
#include "util.h"
// clang-format off
#include <GL/glew.h>
#include <GL/glut.h>
// clang-format on
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    SIDE_HORIZONTAL = 0,
    SIDE_VERTICAL   = 1,
} HitSide;

#define numSprites 19

// 1D Zbuffer
static double ZBuffer[4000];

static GLuint GL_TEXTURE_OBJ = 0;
static RGBA *PIXEL_BUFFER    = NULL;

static int WIDTH;
static int HEIGHT;

#define add_pixel(x, y, color) PIXEL_BUFFER[WIDTH * (HEIGHT - 1 - (y)) + (x)] = color
#define get_pixel(x, y) PIXEL_BUFFER[WIDTH * (HEIGHT - 1 - (y)) + (x)]

void renderer_init(void)
{
    glGenTextures(1, &GL_TEXTURE_OBJ);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE_OBJ);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE_OBJ);

    renderer_set_resolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void renderer_set_resolution(int width, int height)
{
    PIXEL_BUFFER = realloc(PIXEL_BUFFER, (size_t)(height * width) * sizeof(RGBA));

    WIDTH  = width;
    HEIGHT = height;
}

void renderer_cleanup(void)
{
    glDeleteTextures(1, &GL_TEXTURE_OBJ);
    free(PIXEL_BUFFER);
}

static void display_fps(double frame_duration, int render_time, int display_time, const Player *player)
{
    glDisable(GL_TEXTURE_2D);

    glRasterPos2f(0, 0);

    char text[200] = {0};
    snprintf(text, 200, "FPS: %f MS: %f Render: %d Display: %d Player.angle: %f Player.x: %f Player.y: %f", 1.0 / (frame_duration / 1000), frame_duration, render_time, display_time,
             player->direction.angle, player->pos.x, player->pos.y);

    for (int i = 0; i < 200; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }

    glEnable(GL_TEXTURE_2D);
}

typedef struct {
    double distance;
    const Sprite *sprite;
} SpriteDistance;

static int sort_sprite_distances_desc(const void *a, const void *b)
{
    const SpriteDistance *distance_a = a;
    const SpriteDistance *distance_b = b;

    if (distance_a->distance == distance_b->distance) {
        return 0;
    }

    return distance_a->distance > distance_b->distance ? -1 : 1;
}

static void render_sprites(const Map *map, const Player *player)
{
    SpriteDistance sprite_order[map->sprite_count];

    for (unsigned int i = 0; i < map->sprite_count; i++) {
        sprite_order[i] = (SpriteDistance){
            .sprite = &map->sprites[i],
            .distance =
                ((player->pos.x - map->sprites[i].x) * (player->pos.x - map->sprites[i].x) + (player->pos.y - map->sprites[i].y) * (player->pos.y - map->sprites[i].y)) // sqrt not taken, unneeded
        };
    }

    qsort(sprite_order, map->sprite_count, sizeof(SpriteDistance), sort_sprite_distances_desc);

    // after sorting the sprites, do the projection and draw them
    for (unsigned int i = 0; i < map->sprite_count; i++) {
        const Sprite *sprite = sprite_order[i].sprite;

        // translate sprite position to relative to camera
        double spriteX = sprite->x - player->pos.x;
        double spriteY = sprite->y - player->pos.y;

        // transform sprite with the inverse camera matrix
        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        // [ planeY   dirY ]                                          [ -planeY  planeX ]
        double invDet = 1.0 / (player->camera.plane.x * player->direction.vector.y - player->direction.vector.x * player->camera.plane.y); // required for correct matrix multiplication

        double transformX = invDet * (player->direction.vector.y * spriteX - player->direction.vector.x * spriteY);
        double transformY = invDet * (-player->camera.plane.y * spriteX + player->camera.plane.x * spriteY); // this is actually the depth inside the screen, that what Z is in 3D

        int spriteScreenX   = (int)((WIDTH / 2.0) * (1 + transformX / transformY));
        int vertical_offset = (int)(sprite->vertical_offset / transformY) + player->camera.pitch + player->camera.z / transformY;

        const Texture *texture = sprite->texture;

        // calculate height of the sprite on screen
        int spriteHeight = abs((int)(HEIGHT / (transformY))) * sprite->size; // using 'transformY' instead of the real distance prevents fisheye

        // calculate lowest and highest pixel to fill in current stripe
        int drawStartY = -spriteHeight / 2 + HEIGHT / 2 + vertical_offset;

        if (drawStartY < 0) {
            drawStartY = 0;
        }

        int drawEndY = spriteHeight / 2 + HEIGHT / 2 + vertical_offset;

        if (drawEndY >= HEIGHT) {
            drawEndY = HEIGHT - 1;
        }

        // calculate width of the sprite
        int spriteWidth = abs((int)(HEIGHT / (transformY))) * sprite->size; // <<<<<<<<<

        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0)
            drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;

        if (drawEndX >= WIDTH)
            drawEndX = WIDTH - 1;

        // loop through every vertical stripe of the sprite on screen
        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texture->width / spriteWidth) / 256;

            /* if (texX > texture->width) { */
            /*     continue;; */
            /* } */
            // the conditions in the if are:
            // 1) it's in front of camera plane so you don't see things behind you
            // 2) it's on the screen (left)
            // 3) it's on the screen (right)
            // 4) ZBuffer, with perpendicular distance
            if (transformY > 0 && stripe > 0 && stripe < WIDTH && transformY < ZBuffer[stripe])
                for (int y = drawStartY; y < drawEndY; y++) {                                   // for every pixel of the current stripe
                    int d    = (y - vertical_offset) * 256 - HEIGHT * 128 + spriteHeight * 128; // 256 and 128 factors to avoid doubles
                    int texY = ((d * texture->height) / spriteHeight) / 256;

                    RGBA color = texture_get_color(texture, texX, texY);

                    if (color_is_transparent(color)) {
                        continue;
                    }

                    add_pixel(stripe, y, color);
                }
        }
    }
}

typedef struct {
    const Map *map;
    const Player *player;
    unsigned int walls_from_width;
    unsigned int walls_to_width;
    unsigned int ceiling_from_height;
    unsigned int ceiling_to_height;
} RenderContext;

static void render_floor_and_ceiling(const RenderContext *ctx)
{
    const Player *player = ctx->player;
    const Map *map = ctx->map;

    // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
    float ray_dir_x_0 = player->direction.vector.x - player->camera.plane.x;
    float ray_dir_y0  = player->direction.vector.y - player->camera.plane.y;
    float ray_dir_x1  = player->direction.vector.x + player->camera.plane.x;
    float ray_dir_y1  = player->direction.vector.y + player->camera.plane.y;

    float half_height = HEIGHT * 0.5;

    float cam_z_floor   = half_height + player->camera.z;
    float cam_z_ceiling = half_height - player->camera.z;

    const Texture *sky_texture = map->sky_texture;
    float sky_angle_ratio      = player->direction.angle * (sky_texture->width / 360.0);

    // FLOOR CASTING
    for (unsigned int y = ctx->ceiling_from_height; y < ctx->ceiling_to_height; ++y) {
        /* if (y < bottom && y > top) { */
        /*     continue; */
        /* } */

        bool is_floor = y > half_height + player->camera.pitch;

        // Current y position compared to the center of the screen (the horizon)
        int p = is_floor ? (y - half_height - player->camera.pitch) : (half_height - y + player->camera.pitch);

        // Vertical position of the camera.
        // NOTE: with 0.5, it's exactly in the center between floor and ceiling,
        // matching also how the walls are being raycasted. For different values
        // than 0.5, a separate loop must be done for ceiling and floor since
        // they're no longer symmetrical.
        float camera_z = is_floor ? cam_z_floor : cam_z_ceiling;

        // Horizontal distance from the camera to the floor for the current row.
        // 0.5 is the z position exactly in the middle between floor and ceiling.
        // NOTE: this is affine texture mapping, which is not perspective correct
        // except for perfectly horizontal and vertical surfaces like the floor.
        // NOTE: this formula is explained as follows: The camera ray goes through
        // the following two points: the camera itself, which is at a certain
        // height (posZ), and a point in front of the camera (through an imagined
        // vertical plane containing the screen pixels) with horizontal distance
        // 1 from the camera, and vertical position p lower than posZ (posZ - p). When going
        // through that point, the line has vertically traveled by p units and
        // horizontally by 1 unit. To hit the floor, it instead needs to travel by
        // posZ units. It will travel the same ratio horizontally. The ratio was
        // 1 / p for going through the camera plane, so to go posZ times farther
        // to reach the floor, we get that the total horizontal distance is posZ / p.
        float row_distance = camera_z / (float)p;

        // calculate the real world step vector we have to add for each x (parallel to camera plane)
        // adding step by step avoids multiplications with a weight in the inner loop
        float step_x = row_distance * (ray_dir_x1 - ray_dir_x_0) / WIDTH;
        float step_y = row_distance * (ray_dir_y1 - ray_dir_y0) / WIDTH;

        // real world coordinates of the leftmost column. This will be updated as we step to the right.
        float floor_x = player->pos.x + row_distance * ray_dir_x_0;
        float floor_y = player->pos.y + row_distance * ray_dir_y0;

        float prev_x = 0;
        float prev_y = 0;

        /* Lighting lighting = {0}; */

        for (int x = 0; x < WIDTH; ++x) {
            int cell_x = (int)(floor_x);
            int cell_y = (int)(floor_y);

            floor_x += step_x;
            floor_y += step_y;

            if (0 != get_pixel(x, y).a) {
                continue;
            }

            const Cell *cell             = map_get_cell(map, cell_x, cell_y);
            const Texture *floor_texture = cell->floor.texture;

            // get the texture coordinate from the fractional part
            int tx = (int)(floor_texture->width * (floor_x - cell_x)) & (floor_texture->width - 1);
            int ty = (int)(floor_texture->height * (floor_y - cell_y)) & (floor_texture->height - 1);


            if ((int)(floor_y * 10) != (int)(prev_y * 10)) {
                /* lighting = light_source_get_color(map, cell, point(floor_x, floor_y)); */
            }

            prev_x = floor_x;
            prev_y = floor_y;

            if (is_floor) {
                RGBA color = texture_get_color(cell->floor.texture, ty, tx);

                /* if (lighting.color.a > 0) { */
                /*     int r = (color.r * 0.90 + lighting.color.r) * lighting.brightness; */
                /*     int g = (color.g * 0.90 + lighting.color.g) * lighting.brightness; */
                /*     int b = (color.b * 0.90 + lighting.color.b) * lighting.brightness; */

                /*     color.r = min(255, r); */
                /*     color.g = min(255, g); */
                /*     color.b = min(255, b); */
                /* } */

                add_pixel(x, y, color);
            } else {
                if (cell->ceiling.texture) {
                    const Texture *ceiling_texture = cell->ceiling.texture;

                    if (ceiling_texture->width != floor_texture->width) {
                        tx = (int)(floor_texture->width * (floor_x - cell_x)) & (floor_texture->width - 1);
                    }

                    if (ceiling_texture->height != floor_texture->height) {
                        ty = (int)(floor_texture->height * (floor_y - cell_y)) & (floor_texture->height - 1);
                    }

                    add_pixel(x, y, texture_get_color(ceiling_texture, ty, tx));
                } else {
                    int texture_x = sky_angle_ratio - x * (sky_texture->width / (double)WIDTH);
                    int texture_y = y * (sky_texture->height / (double)HEIGHT);

                    if (texture_x <= 0) {
                        texture_x += sky_texture->width;
                    }

                    texture_x = texture_x % sky_texture->width;

                    add_pixel(x, y, texture_get_color(sky_texture, texture_x, texture_y));
                }
            }
        }
    }
}

typedef struct {
    bool exists;
    double distance_x;
    double distance_y;
    double distance;
    double wall_x;
} PolygonyHit;

static PolygonyHit hit_polygon(const Player *player, const Polygon *polygon, const Vector *ray_dir, Point cell_point, HitSide side)
{
    // Extend the ray to make sure it hits the polygon
    double ray_x = player->pos.x + ray_dir->x * 1000.0;
    double ray_y = player->pos.y + ray_dir->y * 1000.0;

    const LineSegment ray_segment = line_segment(point(player->pos.x, player->pos.y), point(ray_x, ray_y));

    PolygonyHit hit = {.exists = false, .distance_x = 9999999999, .distance_y = 9999999999, .wall_x = 9999999999, .distance = 999999999};

    for (unsigned int seg_i = 0; seg_i < polygon->count; seg_i++) {
        // @TODO return the actual line segment hit
        const LineSegment *poly_segment = &polygon->segments[seg_i];
        const Intersection intersection = line_segment_intersection(&ray_segment, poly_segment);

        if (intersection.exists) {
            hit.exists = true;

            hit.distance = min(hit.distance, hypot(player->pos.x - intersection.point.x, player->pos.y - intersection.point.y));

            float dist;

            if (side == SIDE_HORIZONTAL) {
                dist = fabs(player->pos.x > cell_point.x ? ceil(cell_point.x + 0.001) - intersection.point.x : floor(cell_point.x) - intersection.point.x);
            } else {
                dist = fabs(player->pos.y > cell_point.y ? ceil(cell_point.y + 0.0001) - intersection.point.y : floor(cell_point.y) - intersection.point.y);
            }

            if (dist < hit.distance) {
                hit.distance = dist;
                hit.wall_x = hypot(poly_segment->start.x - intersection.point.x, poly_segment->start.y - intersection.point.y);
            }
        }
    }

    return hit;
}

static void render_walls(const RenderContext *ctx)
{
    const Map *map = ctx->map;
    const Player *player = ctx->player;

    for (unsigned int x = ctx->walls_from_width; x < ctx->walls_to_width; x++) {
        // Calculate ray position and direction
        // The right side of the screen will get coordinate 1, the center of the screen gets coordinate 0, and the left side of the screen gets coordinate
        // The direction the the ray is the sum of the direction vector and a part of the plane vector.
        //
        // cameraX is the x-coordinate on the camera plane that the current x-coordinate of the screen represents
        double camera_x = 2 * x / (double)WIDTH - 1; // x-coordinate in camera space

        const Vector ray_dir = vector(player->direction.vector.x + player->camera.plane.x * camera_x, player->direction.vector.y + player->camera.plane.y * camera_x);

        // which cell of the map we're in
        double mapX = (double)player->pos.x;
        double mapY = (double)player->pos.y;

        // length of ray from current position to next x or y-side
        double side_dist_x;
        double side_dist_y;

        // length of ray from one x or y-side to next x or y-side
        double ray_unit_step_size_x = (ray_dir.x == 0) ? 1e30 : fabs(1.0 / ray_dir.x); // 1e30 -> prevent divsion by zero
        double ray_unit_step_size_y = (ray_dir.y == 0) ? 1e30 : fabs(1.0 / ray_dir.y);

        // what direction to step in x or y-direction (either +1 or -1)
        double step_x;
        double step_y;

        HitSide side; // was a NS or a EW wall hit?

        // calculate step and initial sideDist
        if (ray_dir.x < 0) {
            step_x      = -1;
            side_dist_x = (player->pos.x - (int)mapX) * ray_unit_step_size_x;
        } else {
            step_x      = 1;
            side_dist_x = ((int)mapX + 1.0 - player->pos.x) * ray_unit_step_size_x;
        }
        if (ray_dir.y < 0) {
            step_y      = -1;
            side_dist_y = (player->pos.y - (int)mapY) * ray_unit_step_size_y;
        } else {
            step_y      = 1;
            side_dist_y = ((int)mapY + 1.0 - player->pos.y) * ray_unit_step_size_y;
        }

        const Cell *cell;
        PolygonyHit hit = {0};

        // Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
        // hit to the camera plane. Euclidean to center camera point would give fisheye effect!
        // This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
        // for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
        // because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
        // steps, but we subtract deltaDist once because one step more into the wall was taken above.
        double perpWallDist;

        // perform DDA
        while (1) {
            // jump to next map square, either in x-direction, or in y-direction
            // depending on which one is the shorter
            if (side_dist_x < side_dist_y) {
                side_dist_x += ray_unit_step_size_x;
                mapX += step_x;
                side = SIDE_HORIZONTAL;
            } else {
                side_dist_y += ray_unit_step_size_y;
                mapY += step_y;
                side = SIDE_VERTICAL;
            }
            cell = map_get_cell(map, (int)(mapX), (int)(mapY));

            // Check if ray has hit a wall
            if (false == cell->wall.empty) {
                if (cell->wall.polygon) {
                    hit = hit_polygon(player, cell->wall.polygon, &ray_dir, point(mapX, mapY), side);

                    if (!hit.exists) {
                        continue;
                    }

                    if (side == SIDE_HORIZONTAL) {
                        perpWallDist = ((side_dist_x + ray_unit_step_size_x * hit.distance) - ray_unit_step_size_x);
                    } else {
                        perpWallDist = ((side_dist_y + ray_unit_step_size_y * hit.distance) - ray_unit_step_size_y);

                    }
                    side_dist_x += ray_unit_step_size_x * hit.distance;
                    side_dist_y += ray_unit_step_size_y * hit.distance;
                }

                break;
            }
        }

        ZBuffer[x] = perpWallDist; // perpendicular distance is used

        // Calculate height of line to draw on screen
        int lineHeight = (int)(HEIGHT / perpWallDist);

        /* smallest_wall = min(smallest_wall, lineHeight); */

        // calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2.0 + HEIGHT / 2.0 + player->camera.pitch + (player->camera.z / perpWallDist);

        if (drawStart < 0) {
            drawStart = 0;
        }

        int drawEnd = lineHeight / 2.0 + HEIGHT / 2.0 + player->camera.pitch + (player->camera.z / perpWallDist);

        if (drawEnd >= HEIGHT) {
            drawEnd = HEIGHT - 1;
        }

        // texturing calculations
        const Texture *texture = cell->wall.texture; // 1 subtracted from it so that texture 0 can be used!

        // x coordinate on the texture
        int texX = (int)(hit.wall_x * (double)texture->width);

        // How much to increase the texture coordinate per screen pixel
        float step = (float)texture->height / lineHeight;

        // Starting texture coordinate
        float texPos = (drawStart - player->camera.pitch - (player->camera.z / perpWallDist) - HEIGHT / 2.0 + lineHeight / 2.0) * step;

        // TODO: calculate only once, see: hit_polygon()
        /* float ray_x = player->pos.x + ray_dir.x * perpWallDist; */
        /* float ray_y = player->pos.y + ray_dir.y * perpWallDist; */

        /* Lighting lighting = light_source_get_color(map, cell, point(ray_x, ray_y)); */

        for (int y = drawStart; y < drawEnd; y++) {
            int texY = (int)texPos;

            texPos += step;

            RGBA color = texture_get_color(texture, texX, texY);

            /* int r = (color.r * 0.90 + lighting.color.r) * lighting.brightness; */
            /* int g = (color.g * 0.90 + lighting.color.g) * lighting.brightness; */
            /* int b = (color.b * 0.90 + lighting.color.b) * lighting.brightness; */

            /* color.r = min(255, r); */
            /* color.g = min(255, g); */
            /* color.b = min(255, b); */

            add_pixel(x, y, color);
        }
    }
}

static void *render_world(void *arg)
{
    render_walls(arg);
    render_floor_and_ceiling(arg);

    return NULL;
}

#include <pthread.h>

#define MULTITHREAD 0

void render(const Map *map, const Player *player, double frame_duration)
{
    int start = glutGet(GLUT_ELAPSED_TIME);

    memset(PIXEL_BUFFER, 0, WIDTH * HEIGHT * sizeof(RGBA));

#if MULTITHREAD
    pthread_t thread1;
    pthread_t thread2;

    RenderContext left_ctx = {
        .map = map,
        .player = player,
        .walls_from_width = 0,
        .walls_to_width = WIDTH * 0.5,
        .ceiling_from_height = 0,
        .ceiling_to_height = HEIGHT * 0.5
    };

    RenderContext right_ctx = {
        .map = map,
        .player = player,
        .walls_from_width = WIDTH * 0.5,
        .walls_to_width = WIDTH,
        .ceiling_from_height = HEIGHT * 0.5,
        .ceiling_to_height = HEIGHT
    };

    pthread_create(&thread1, NULL, render_world, (void *) &left_ctx);
    pthread_create(&thread2, NULL, render_world, (void *) &right_ctx);

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
#else
    RenderContext all_ctx = {
        .map = map,
        .player = player,
        .walls_from_width = 0,
        .walls_to_width = WIDTH,
        .ceiling_from_height = 0,
        .ceiling_to_height = HEIGHT
    };

    render_world(&all_ctx);
#endif

    render_sprites(map, player);

    int end         = glutGet(GLUT_ELAPSED_TIME);
    int render_time = end - start;

    start = glutGet(GLUT_ELAPSED_TIME);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, PIXEL_BUFFER);

    // Set up projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, (GLfloat)WIDTH, 0, (GLfloat)HEIGHT, 0, 1);

    // Render Quad
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);

    glVertex2f(0, 0);

    glTexCoord2f(0, 1);
    glVertex2f(0, (GLfloat)HEIGHT);

    glTexCoord2f(1, 1);
    glVertex2f((GLfloat)WIDTH, (GLfloat)HEIGHT);

    glTexCoord2f(1, 0);
    glVertex2f((GLfloat)WIDTH, 0);

    glEnd();

    /* memset(PIXEL_BUFFER, 0, WIDTH * HEIGHT * sizeof(RGBA)); */

    end = glutGet(GLUT_ELAPSED_TIME);

    display_fps(frame_duration, render_time, end - start, player);

    glPopMatrix();
}
