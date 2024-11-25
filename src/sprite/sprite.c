#include "sprite.h"
#include "../render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "../math.h"

/**
 * check DGL documentation/pixmap.md
 */
Color *pixmap_generate(int shape_type, union shape_info dat, Color colour, Color *out)
{
    bool skip_blank = false;
    Color *res = 0;

    if (out)
    {
        res = out;
        skip_blank = true;
    }

    if (shape_type == RECT)
    {
        if (!res)
            res = (Color *)malloc(dat.v2.x * dat.v2.y * sizeof(Color));

        for (int i = 0; i < dat.v2.x * dat.v2.y; i++)
            *(res + i) = colour;
    }
    else if (shape_type == CIRCLE)
    {
        if (!res)
            res = (Color *)malloc(dat.v2.x * dat.v2.y * sizeof(Color));

        for (int y = 0; y < dat.v2.y; y++)
            for (int x = 0; x < dat.v2.x; x++)
                if (distance((dat.v2.x / 2), (dat.v2.x / 2), x, y) <= dat.v2.x / 2)
                    *(res + x + ((int)(y * dat.v2.x))) = colour;
                else if (!skip_blank)
                    *(res + x + ((int)(y * dat.v2.x))) = BLANK;
    }
    else if (shape_type == ELLIPSE)
    {
        /**
         * Calculations for ellipse is based on info found at Omni Calculator:
         *
         * Alambra, K. "Foci of an Ellipse Calculator".
         * Available at: https://www.omnicalculator.com/math/foci-of-ellipse.
         * Accessed: 7 September 2023.
         */
        if (!res)
            res = (Color *)malloc(dat.v2.x * dat.v2.y * sizeof(Color));

        int major = larger(dat.v2.x, dat.v2.y), minor = smaller(dat.v2.x, dat.v2.y);
        float focal_distance = sqrt(squared(major / 2) - squared(minor / 2));

        float focus1[2], focus2[2];
        float std_length;
        if (dat.v2.x >= dat.v2.y) // horizontal ellipse
        {
            focus1[0] = (dat.v2.x / 2) - focal_distance;
            focus1[1] = dat.v2.y / 2;

            focus2[0] = (dat.v2.x / 2) + focal_distance;
            focus2[1] = dat.v2.y / 2;

            std_length = sqrt(distance(focus1[0], focus1[1], dat.v2.x / 2, dat.v2.y) * 2);
        }
        else // vertical
        {
            focus1[0] = dat.v2.x / 2;
            focus1[1] = (dat.v2.y / 2) - focal_distance;

            focus2[0] = dat.v2.x / 2;
            focus2[1] = (dat.v2.y / 2) + focal_distance;

            std_length = sqrt(distance(focus1[0], focus1[1], dat.v2.x - 1, dat.v2.y / 2) * 2);
        }

        for (int y = 0; y < dat.v2.y; y++)
            for (int x = 0; x < dat.v2.x; x++)
                if (sqrt(distance(focus1[0], focus1[1], x, y) + distance(focus2[0], focus2[1], x, y)) <= std_length)
                    *(res + x + ((int)(y * dat.v2.x))) = colour;
                else if (!skip_blank)
                    *(res + x + ((int)(y * dat.v2.x))) = BLANK;
    }
    else if (shape_type == EQUILATERAL)
    {
        if (!res)
            res = (Color *)malloc(dat.v2.x * dat.v2.y * sizeof(Color));

        Vector2 vertices[3];

        float m; // slope

        if (dat.v2.y >= dat.v2.x)
        {
            // upright triangle
            vertices[0].y = 0;            /*top vertex  +          */
            vertices[0].x = dat.v2.x / 2; /*            / \         */

            vertices[1].y = dat.v2.y - 1; /*bottom left vertex  /   */
            vertices[1].x = 0;            /*                    +--  */

            vertices[2].y = dat.v2.y - 1; /*bottom right vertex   \   */
            vertices[2].x = dat.v2.x - 1; /*                      --+  */

            m = (vertices[1].y - vertices[0].y) / (vertices[1].x - vertices[0].x); // left arm slope

#define conditions                            \
    (x >= slope_intercept_x(y, m, 0)) &&      \
        (x <= slope_intercept_x(y, -m, 0)) && \
        (y <= dat.v2.y)

            for (int x = 0; x < dat.v2.x; x++)
                for (int y = 0; y < dat.v2.y; y++)
                    if (conditions)
                        *(res + x + ((int)(y * dat.v2.x))) = colour;
                    else if (!skip_blank)
                        *(res + x + ((int)(y * dat.v2.x))) = BLANK;
#undef conditions
        }
        else
        {

            // triangle will point to the right
            vertices[0].y = dat.v2.y / 2; /*mid-right vertex \ */
            vertices[0].x = dat.v2.x;     /*                 /     */

            vertices[1].y = 0; /*top left vertex   |\      */
            vertices[1].x = 0; /*                  | \ */

            vertices[2].y = dat.v2.y; /*bottom left vertex  | / */
            vertices[2].x = 0;        /*                   |/   */

            m = (vertices[1].y - vertices[0].y) / (vertices[1].x - vertices[0].x); // top am

#define conditions                               \
    (y <= slope_intercept_y(x, -m, dat.v2.y)) && \
        (y >= slope_intercept_y(x, m, 0)) &&     \
        (x <= dat.v2.x)

            for (int x = 0; x < dat.v2.x; x++)
                for (int y = 0; y < dat.v2.y; y++)
                    if (conditions)
                        *(res + x + ((int)(y * dat.v2.x))) = colour;
                    else if (!skip_blank)
                        *(res + x + ((int)(y * dat.v2.x))) = BLANK;
#undef conditions
        }
    }
    else if (shape_type == SC_RECT)
    {
        if (!res)
            res = (Color *)malloc(dat.v3.x * dat.v3.y * sizeof(Color));

        for (int x = 0; x < dat.v3.x; x++)
            for (int y = 0; y < dat.v3.y; y++)
                if (x < dat.v3.x - dat.v3.z)
                    *(res + x + (int)(dat.v3.x * y)) = colour;
                else if (!skip_blank)
                    *(res + x + (int)(dat.v3.x * y)) = BLANK;
    }
    else if (shape_type == SC_CIRCLE)
    {
        if (!res)
            res = (Color *)malloc(dat.v3.y * dat.v3.y * sizeof(Color));

        for (int y = 0; y < dat.v3.y; y++)
            for (int x = 0; x < dat.v3.x; x++)
                if (distance(((dat.v3.x - dat.v3.z) / 2), ((dat.v3.x - dat.v3.z) / 2), x, y) <= (dat.v3.x - dat.v3.z) / 2)
                    *(res + x + ((int)(y * dat.v3.x))) = colour;
                else if (!skip_blank)
                    *(res + x + ((int)(y * dat.v3.x))) = BLANK;
    }
    else if (shape_type == SCATTER)
    {
        if (!res)
            res = (Color *)malloc(dat.v3.x * dat.v3.y * sizeof(Color));

        unsigned points = (int)(dat.v3.x * dat.v3.y) / dat.v3.z; // z = density as in "1 in z pixels"
        while (points--)
        {
            *(res + GetRandomValue(0, dat.v3.x) + (int)(dat.v3.x * GetRandomValue(0, dat.v3.y))) = colour;
        }
    }
    else if (shape_type == SC_SCATTER)
    {
        // w = size of each point
        // x = width of area
        // y = height of area
        // z = density as in "1 in z pixels"
        if (!res)
            res = (Color *)malloc(dat.v4.x * dat.v4.y * sizeof(Color));

        unsigned points = (int)(dat.v4.x * dat.v4.y) / dat.v4.z;
        while (points--)
        {
            int x = GetRandomValue(0, dat.v4.x), y = GetRandomValue(0, dat.v4.y);
            for (int x_off = 0; x_off < dat.v4.w; x_off++)
                for (int y_off = 0; y_off < dat.v4.w; y_off++)
                    *(res + x + x_off + ((y + y_off) * (int)dat.v4.x)) = colour;
        }
    }

    return res;
}

/**
 * @brief Find a sprite in an entity
 * @returns index into entity.objects, -1 if sprite does not occur in entity, -2 for invalid arg
 */
int find_sprite_in_entity(sprite *sp, entity *ent)
{
    if (!ent)
        return -2;
    int res = -1;
    for (int i = 0; i < MAX_SPRITES; i++)
        if (ent->objects[i].sp == sp)
        {
            res = i;
            break;
        }
    return res;
}

/**
 * @brief add a sprite to an entity
 * @returns index into entity.objects, or -1 if there was no room
 */
int add_sprite_to_entity(sprite *sp, entity *ent)
{
    if (!ent || !sp)
        return -2;
    int res;
    for (res = 0; res < MAX_SPRITES; res++)
    {
        if (!ent->objects[res].sp)
        {
            ent->objects[res].sp = sp;
            sp->ent = ent;
            break;
        }
    }
    if(res == MAX_SPRITES) res = -1;
    return res;
}

/**
 * @brief Load pixmap data from file
 * @param filename filename of the file that has the pixmap data
 * @returns NULL on failure, address of pixmap in heap on success
 * @note It is necessary to free the memory returned by load_pixmap_from_file. This is not done automatically
 */
Color *load_pixmap_from_file(char const *filename)
{
    static struct
    {
        int width;
        int height;
    } header;

    if (!filename)
        return NULL;

    Color *res;
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return NULL;

    fread(&header, sizeof(header), 1, fp);
    res = malloc(header.height * header.width * sizeof(Color));
    if (!res)
        return NULL;

    fread(res, header.height * header.width * sizeof(Color), 1, fp);
    fclose(fp);
    return res;
}

int init_sprite(sprite *sp, int width, int height, int x, int y, Color *pixmap)
{
    if (!sp || width <= 0 || width > windowWidth || height <= 0 || height > windowHeight || !pixmap)
        return -1;
    sp->width = width;
    sp->height = height;
    sp->x = x;
    sp->y = y;
    sp->pixmap = pixmap;
    sp->rotation = 0;
    return 0;
}

void delete_sprite(sprite *sp)
{
    if (sp->pixmap)
        free(sp->pixmap);
    memset(sp, 0, sizeof(sprite));
}

/**
 * @brief check if two sprites have collided with each other
 * @param sp1
 * @param sp2 sprites to check collisions with
 * @return true if these sprites have collided with each other.
 * @returns false if they haven't
 */
bool check_collision(sprite *sp1, sprite *sp2)
{
#define conditions                              \
    ((obj1.origin.x <= obj2.extremity.x) &&     \
     (obj1.extremity.x >= obj2.origin.x)) &&    \
        ((obj1.origin.y <= obj2.extremity.y) && \
         (obj1.extremity.y >= obj2.origin.y))

    typedef struct
    {
        int x;
        int y;
    } vect2;
    struct
    {
        vect2 origin;
        vect2 extremity;
    } obj1, obj2;

    obj1.origin = (vect2){origin_x(sp1), origin_y(sp1)};
    obj1.extremity = (vect2){origin_x(sp1) + sp1->width, origin_y(sp1) + sp1->height};

    obj2.origin = (vect2){origin_x(sp2), origin_y(sp2)};
    obj2.extremity = (vect2){origin_x(sp2) + sp2->width, origin_y(sp2) + sp2->height};

    if (conditions)
    {
        if (sp1->ent)
        {
            int obj_index = find_sprite_in_entity(sp1, sp1->ent);
            if (!(obj_index < 0 || obj_index >= MAX_SPRITES))
            {
                if (find_sprite_in_entity(sp2, sp1->ent) < 0) sp1->ent->entity_collision = true;
                sp1->ent->objects[obj_index].collision = true;
            }
        }
        if (sp2->ent)
        {
            int obj_index = find_sprite_in_entity(sp2, sp2->ent);
            if (!(obj_index < 0 || obj_index >= MAX_SPRITES))
            {
                if (find_sprite_in_entity(sp1, sp2->ent) < 0) sp2->ent->entity_collision = true;
                sp2->ent->objects[obj_index].collision = true;
            }
        }
        return true;
    }
    return false;
#undef conditions
}

/**
 * @brief Check if the sprite has collided with anything in the layer
 * @param sp pointer to sprite
 * @param lid layer id to check sp against
 * @note to find out which sprites have collided with sp, call check_collision
 */
bool has_collided(sprite *sp, int lid)
{
    for (size_t i = 0; i < MAX_SPRITES; i++)
        if (layers[lid].objects[i] && layers[lid].objects[i] != sp && check_collision(layers[lid].objects[i], sp))
            return true;
    return false;
}

/**
 * @brief generate an array of sprite pointers to sprites that have collided with sp
 * @param sp pointer to sprite to check against
 * @param lid layer to check sp against
 * @returns NULL for invalid arguments; array of pointers to sprites that are in collision with sp
 * @note bounds: MAX_SPRITES pointers
 */
sprite **all_collided(sprite *sp, int lid)
{
    static sprite *collided[MAX_SPRITES];

    if (!sp || lid < 0 || lid > MAX_LAYERS)
        return NULL;

    memset(collided, 0, sizeof(collided));

    size_t c = 0;
    for (size_t i = 0; i < MAX_SPRITES; i++)
        if (layers[lid].objects[i] && layers[lid].objects[i] != sp && check_collision(layers[lid].objects[i], sp))
            collided[c++] = layers[lid].objects[i];
    return collided;
}

/**
 * @brief update the collision flags for an entity
 * @param ent entity to update collisions for
 * @param lid layer id to test collision against
 */
void ent_update_collision(entity *ent, int lid)
{
    ent->entity_collision = false;
    for(int i = 0; i < MAX_SPRITES; i++)
    {
        if(!ent->objects[i].sp) continue;
        if(!*all_collided(ent->objects[i].sp, lid))
            ent->objects[i].collision = false;
    }
}