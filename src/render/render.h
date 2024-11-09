#ifndef RENDER_H
#define RENDER_H

#include "../definitions.h"
#include <stdbool.h>


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