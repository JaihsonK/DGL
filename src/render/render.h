#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "../comp.h"
#include <stdbool.h>

// maximum 10 layers of cache
#define MAX_LAYERS 10
#define MAX_SPRITES 50

#define sprite struct _sprite

typedef struct text
{
    char *text;
    int fontsize;
    int x;
    int y;
    Color colour;
} text;

#define IMAGE 0
#define TEXT 1
struct _sprite
{
    toggle image_text;

    union
    {
        Color *pixmap;
        text *text;
    };

    int width;
    int height;
    int x;
    int y;

    int rotation; // in degrees

    bool skip;

    int current_level;
    int current_layer;
};

typedef struct
{
    sprite *objects[MAX_SPRITES];
    Color *cache;
    bool changed;
    bool in_use;
} layer;

struct screen
{
    bool pause_render;
    bool rw;
    Color *cache;
};

#ifndef RENDER_C

extern struct screen screen_dat;

extern unsigned long long _frames;

extern layer layers[MAX_LAYERS];

extern bool kill_render;

#endif
void init_render_system();
void clean_render_system();
void reset_layer(int id);
int reset_all_layers();
void render_layer(int id);
int render_all_layers(bool changedonly);
void compile();
int generate_final_image(bool changedonly);
int add_sprite(sprite *ns, int id);
int insert_sprite(sprite *ns, int id, int level);
int remove_sprite(int level, int layer);
int find_sprite(sprite *sp, int layer);
int move_sprite(int f_level, int f_layer, int t_level, int t_layer);

void *render(void *null);

#endif