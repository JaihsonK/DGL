#define RENDER_C

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <stdint.h>
#include <pthread.h>
#include <math.h>
#include "render.h"
#define MAX_TEXT MAX_SPRITES

text *text_render_log[MAX_TEXT];
size_t text_render_log_index = 0;

layer layers[MAX_LAYERS];
struct screen screen_dat;

static void swap_image_buffers()
{
    while(screen_dat.rw);
    screen_dat.rw = true;

    void *tmp = screen_dat.running_image;
    screen_dat.running_image = screen_dat.cache;
    screen_dat.cache = tmp;

    screen_dat.rw = false;
}

/**
 * @brief initiate render system
 */
void init_render_system()
{
    screen_dat.cache = malloc(windowHeight * windowWidth * sizeof(Color));
    screen_dat.running_image = malloc(windowHeight * windowWidth * sizeof(Color));
    screen_dat.new_img = false;
}

/**
 * @brief deallocates resources used by the render system
 */
void clean_render_system()
{
    free(screen_dat.cache);
    free(screen_dat.running_image);
}

/**
 * @brief clear layer's object pointer array
 * @note will not free sprite data
 * @param id layer ID
 */
void reset_layer(int id)
{
    if (id < 0 || id >= MAX_LAYERS)
        return;

    for (int i = 0; i < MAX_SPRITES; i++)
        if (layers[id].objects[i])
            layers[id].objects[i] = NULL;
    layers[id].in_use = false;
    layers[id].changed = false;
}

/**
 * @brief clears layer object pointer array for all layers marked "in use"
 * @return how many layers have been cleared
 * @note it's easy to reset all layers except ones you want to keep! Simply turn off the "in use" flag before calling this function
 */
int reset_all_layers()
{
    int count = 0;
    for (int i = 0; i < MAX_LAYERS; i++)
        if (layers[i].in_use)
        {
            reset_layer(i);
            count++;
        }
    return count;
}

/**
 * @brief render a layer's sprites to screen cache
 * @note changes will not be visable on screen until all layers have been compiled into the final image and that has been drawn to screen
 * @param id layer's ID
 */
void render_layer(int id)
{
    if (id < 0 || id >= MAX_LAYERS)
        return;

    layer *l = &layers[id];
    sprite *sp;
    for (size_t i = 0; i < MAX_SPRITES; i++)
    {
        sp = l->objects[i];
        if (!(sp) || (sp)->skip)
            continue;

        if (sp->image_text == TEXT)
        {
            text_render_log[text_render_log_index++] = sp->text;
            continue;
        }

        // NOTE contrary to original dodge designs, image sprites do not require a backup because each sprite is rerendered whenever a layer is rendered.

        // draw to screen cache

        
        for (int x = 0; x < sp->width; x++)
            for (int y = 0; y < sp->height; y++)
            {
                register unsigned x_pos = (sp->x + ((x * cos(sp->rotation DEG2RAD)) - (y * sin(sp->rotation DEG2RAD)))), y_pos = (sp->y + ((y * cos(sp->rotation DEG2RAD)) + (x * sin(sp->rotation DEG2RAD))));
                //register unsigned x_pos = x + sp->x, y_pos = y + sp->y;
                if ((y_pos < 0) || (x_pos < 0) || (y_pos >= windowHeight) || (x_pos >= windowWidth))
                    continue;
                else if (colour_compare(BLANK, sp->pixmap[(x + (y * sp->width)) % (sp->height * sp->width)]))
                    continue;
                else
                    screen_dat.cache[x_pos + (y_pos * windowWidth)] = sp->pixmap[x + (y * sp->width)];
            }
    }
}

/**
 * @brief render all layers to screen cache
 * @param changedonly if true, function only renders layers with the "changed" flag turned on
 * @returns number of layers rendered
 */
int render_all_layers(bool changedonly)
{
    int count = 0;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        if (changedonly && !layers[i].changed)
            continue;
        render_layer(i);
        count++;
    }
    return count;
}

/**
 * @brief compiles all sprites to the final image cache
 * @note This function DOES render layers
 */
void compile()
{
    render_all_layers(false);
    swap_image_buffers();
}

/**
 * @brief add a sprite to the layer at the first available level
 * @param ns pointer to the new sprite
 * @param id target layer's ID
 * @return level num on success (positive int < MAX_SPRITES).
 * @return -1 invalid argument.
 * @return -2 no more space on layer
 * @note automatically sets "in use" and "changed" flags on
 */
int add_sprite(sprite *ns, int id)
{
    if (!ns || id < 0 || id >= MAX_LAYERS)
        return -1;

    int i;
    layer *l = &layers[id];
    for (i = 0; i < MAX_SPRITES; i++)
        if (l->objects[i] == NULL)
        {
            l->objects[i] = ns;
            break;
        }
    if (i == MAX_SPRITES)
        i = -2;
    else
    {
        l->in_use = true;
        l->changed = true;
        ns->current_level = i;
        ns->current_layer = id;
    }

    return i;
}

/**
 * @brief attempt to insert a sprite at a specific level on a layer
 * @param ns pointer to new sprite
 * @param id target layer's ID
 * @param level target level within the layer
 * @note if a sprite already inhabits the target level, it and all higher sprites will be shifted up.
 * @returns 0 on success.
 * @returns -1 invalid argument.
 * @returns -2 failure.
 */
int insert_sprite(sprite *ns, int id, int level)
{
    if (!ns || id < 0 || id >= MAX_LAYERS || level < 0 || level >= MAX_SPRITES)
        return -1;

    layer *l = &layers[id];

    if (l->objects[level] && insert_sprite(l->objects[level], id, level + 1) < 0)
        return -2;

    ns->current_level = level;
    ns->current_layer = id;
    l->objects[level] = ns;

    return 0;
}

/**
 * @brief remove a sprite from layer
 * @note this will NOT free sprite data
 * @param level specify which level within the layer to remove
 * @param layer which layer contains the target sprite
 * @returns `0` on success.
 * @returns `-1` invalid arguments.
 */
int remove_sprite(int level, int layer)
{
    if (level < 0 || layer < 0 || level >= MAX_SPRITES || layer >= MAX_LAYERS)
        return -1;

    layers[layer].objects[level]->current_layer = 0;
    layers[layer].objects[level]->current_level = 0;
    layers[layer].objects[level] = 0;
    return 0;
}

/**
 * @brief find a sprite within a layer
 * @param sp sprite pointer that we're looking for
 * @param layer layer in which the sprite exists
 * @returns level the sprite is found at.
 * @returns `-1` if not found.
 */
int find_sprite(sprite *sp, int layer)
{
    int res = -1;
    for (int i = 0; i < MAX_SPRITES; i++)
        if (layers[layer].objects[i] == sp)
            res = i;
    return res;
}

/**
 * @brief attempt to remove a sprite from one layer and insert it into another
 * @note if insert fails, sprite will be placed in the highest available level
 * @param f_level level from which to take the sprite
 * @param f_layer layer from which to take the sprite
 * @param t_level target level to which function will attempt to insert sprite. If `-1` function will add sprite to first available level
 * @param t_layer target layer to which function will attempt to insert sprite
 * @returns level number function inserted sprite to.
 * @returns `-1` invalid argument.
 * @returns `-2` failure.
 */
int move_sprite(int f_level, int f_layer, int t_level, int t_layer)
{
#define conditions(x, y) (x < 0 || x >= y)

    if (conditions(f_layer, MAX_LAYERS) || conditions(f_level, MAX_SPRITES) || conditions(t_layer, MAX_LAYERS))
        return -1;

#undef conditions

    sprite *tmp = layers[f_layer].objects[f_level];
    layers[f_layer].objects[f_level] = 0;

    int res = t_level;

    if (insert_sprite(tmp, t_layer, t_level) < 0)
        res = add_sprite(tmp, t_layer);

    return res;
}

uint64_t _frames = 0;
bool kill_render = false, kill_program = false;
void *render(void *title_n)
{
    InitWindow(windowWidth, windowHeight, (char *)title_n);
    SetTargetFPS(60);
    while (!IsWindowReady())
        ;
    while (!WindowShouldClose() && !kill_render)
    {
        while (screen_dat.rw || screen_dat.pause_render)
            ;
        screen_dat.rw = true;
        BeginDrawing();
        for (int index = 0; index < windowHeight * windowWidth; index++)
            DrawPixel(index % windowWidth, index / windowWidth, screen_dat.running_image[index]);

        for (text **t = text_render_log; t < text_render_log + text_render_log_index; t++)
            if (*(t))
                DrawText((*t)->text, (*t)->x, (*t)->y, (*t)->fontsize, (*t)->colour);

        EndDrawing();
        screen_dat.rw = false;

        _frames++;
    }
    kill_program = true;
    pthread_exit(0);
}