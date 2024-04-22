#ifndef PIXMAP_H
#define PIXMAP_H

#include "raylib.h"
#include "../render/render.h"

enum shape_types
{
    NOTHING,
    RECT,
    CIRCLE,
    ELLIPSE,
    EQUILATERAL, //triangle
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

/**
 * @brief generate a pixel map (Color array) that represents an image
 * @param shape_type refer to enum shape_types
 * @param dat dimensions of the shape and supplementary info specific to shape type. See code
 * @param colour colour of the shape
 * @param out location to output pixmap. If null space will be allocated on heap
*/
Color *pixmap_generate(int shape_type, union shape_info dat, Color colour, Color *out);

/**
 * @brief initiate a sprite that already exists in heap or stack
 * @param sp pointer to sprite
 * @param width width of the sprite in pixels
 * @param height height of the sprite in pixels
 * @param x x coordinate
 * @param y y coordinate
 * @param pixmap pointer to pixmap
*/
int init_sprite(sprite *sp, int width, int height, int x, int y, Color *pixmap);

/**
 * @brief delete a sprite
 * @param sp sprite pointer
 * @note if your sprite's pixmap is in your local stack, null the pointer first. This function assumes that the pixmap is on the heap.
*/
void delete_sprite(sprite *sp);

/**
 * @brief check for collision between sp and all sprites in layer. Set pointers in sp
 * @param sp sprite we are checking
 * @param lid layer whose sprites we are checking against
 * @return -1 invalid arguments
 * @return number of sprites sp collided with
*/
int update_collision(sprite *sp, int lid);

/**
 * @brief check if two sprites have collided with each other
 * @param obj1
 * @param obj2 sprites to check collisions with
 * @return true if these sprites have collided with each other.
 * @returns false if they haven't
*/
bool check_collision(sprite *obj1, sprite *obj2);
#endif