#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DGL_version "2024.11.1 (Linux dev3)"

#include "raylib.h"

#define colour_compare(x, y) (((x).r == (y).r) && ((x).g == (y).g) && ((x).b == (y).b) && ((x).a == (y).a))

#define absolute(x) (x < 0 ? -x : x)

#define match_sign(x, y) (x < 0 ? -y : y)

extern int windowWidth, windowHeight;

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

struct _entity;
#define IMAGE 0
#define TEXT 1
struct _sprite
{
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

    struct 
    {
        char skip : 1;
        char image_text : 1;
        char negate_x : 1;
        char negate_y : 1;
    };

    int current_level;
    int current_layer;
    struct _entity *ent;
};

struct _entity
{
    int x;
    int y;
    bool entity_collision;
    struct
    {
        sprite *sp;
        char collision:1;
    }objects[MAX_SPRITES];
};
#define entity struct _entity

typedef struct
{
    sprite *objects[MAX_SPRITES];
    bool changed;
    bool in_use;
} layer;

struct screen
{
    struct
    {
        char pause_render : 1;
        char rw : 1;
        char new_img : 1; 
    };
    Color *cache;
    Color *running_image;
};

enum shape_types
{
    NOTHING,
    RECT,
    CIRCLE,
    ELLIPSE,
    EQUILATERAL, //triangle
    CURVE,
    SC_RECT, //special case rect
    SC_CIRCLE, //special case circle
    SCATTER, //scattered pixels
    SC_SCATTER
};
union shape_info
{
    Vector2 v2;
    Vector3 v3;
    Vector4 v4;
};

#endif