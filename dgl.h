#ifndef DGL_H
#define DGL_H

#include <raylib.h>


/********
 * MATH *
 ********/

#define squared(x) ((x) * (x))
#define distance(x1, y1, x2, y2) sqrt(squared((x2) - (x1)) + squared((y2) - (y1)))

#define larger(x, y) (((x) >= (y)) ? (x) : (y))
#define smaller(x, y) (((x) >= (y)) ? (y) : (x))

#define slope_intercept_x(y, m, b) (((y - b) / m) + (dat.v2.x / 2))
#define slope_intercept_y(x, m, b) (m * (x) + b)


/***************
 * DEFINITIONS *
 ***************/

#define DGL_version "2024.12.1"

//compatibility list
#define DGL_linux
#define DGL_24_12_01

#define MAX_LAYERS 10
#define MAX_SPRITES 50

#define running !kill_program

#define colour_compare(x, y) ((x.r == y.r) && (x.g == y.g) && (x.b == y.b) && (x.a == y.a))

#define shinf(_x, _y) (union shape_info){.v2 = {.x = _x, .y = _y}}
#define shinf3(_x, _y, _z) (union shape_info){.v3 = {.x = _x, .y = _y, .z = _z}}
#define shinf4(_w, _x, _y, _z) (union shape_info){.v4 = {.w = _w, .x = _x, .y = _y, .z = _z}}

#define toggle int

//re: toggle in struct sprite
    #define IMAGE 0
    #define TEXT 1

enum shape_types
{
    NOTHING,
    RECT,
    CIRCLE,
    ELLIPSE,
    EQUILATERAL, //triangle
    SC_RECT, //special case rect
    SC_CIRCLE, //special case circle
    SCATTER, //scattered points
    SC_SCATTER, //special case scattered points
};
union shape_info
{
    Vector2 v2;
    Vector3 v3;
    Vector4 v4;
};

typedef struct text
{
    char *text;
    int fontsize;
    int x;
    int y;
    Color colour;
} text;

struct _entity;

typedef struct
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
}sprite;

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
typedef struct _entity entity;

typedef struct
{
    sprite *objects[MAX_SPRITES];
    Color *cache;
    bool changed;
    bool in_use;
}layer;

struct screen
{
    bool pause_render;
    bool rw;
    Color *cache;
};


/*************
 * FUNCTIONS *
 *************/

/**
 * @brief start the Dodge Graphics Library
 * @param session_dat path to a file that contains sessions information. See documentation
*/
extern int start(char *session_dat);


extern void terminate();

/**
 * @brief generate a pixel map (Color array) that represents an image
 * @param shape_type refer to enum shape_types
 * @param dat dimensions of the shape and supplementary info specific to shape. See code
 * @param colour colour of the shape
 * @param out location to output pixmap. If null space will be allocated on heap
*/
extern Color *pixmap_generate(int shape_type, union shape_info dat, Color colour, Color *out);

/**
 * @brief Load pixmap data from file
 * @param filename filename of the file that has the pixmap data
 * @returns NULL on failure, address of pixmap in heap on success
 * @note It is necessary to free the memory returned by load_pixmap_from_file. This is not done automatically
*/
extern Color *load_pixmap_from_file(char const *filename);

/**
 * @brief check if two sprites have collided with each other
 * @param sp1
 * @param sp2 sprites to check collisions with
 * @return true if these sprites have collided with each other.
 * @returns false if they haven't
*/
extern bool check_collision(sprite *sp1, sprite *sp2);

/**
 * @brief Check if the sprite has collided with anything in the layer
 * @param sp pointer to sprite
 * @param lid layer id to check sp against
 * @note to find out which sprites have collided with sp, call check_collision
*/
extern bool has_collided(sprite *sp, int lid);

/**
 * @brief generate an array of sprite pointers to sprites that have collided with sp
 * @param sp pointer to sprite to check against
 * @param lid layer to check sp against
 * @returns NULL for invalid arguments; Array of pointers to sprites that are in collision with sp
 * @note bounds: MAX_SPRITES pointers
*/
extern sprite **all_collided(sprite *sp, int lid);

/**
 * @brief clear layer's object pointer array
 * @note will not free sprite data
 * @param id layer ID
 */
extern void reset_layer(int id);

/**
 * @brief clears layer object pointer array for all layers marked "in use"
 * @return how many layers have been cleared
 * @note it's easy to reset all layers except ones you want to keep! Simply turn off the "in use" flag before calling this function
 */
extern int reset_all_layers();

/**
 * @brief render a layer's sprites to screen cache
 * @note changes will not be visable on screen until all layers have been compiled into the final image and that has been drawn to screen
 * @param id layer's ID
 */
extern void render_layer(int id);

/**
 * @brief render all layers to screen cache
 * @param changedonly if true, function only renders layers with the "changed" flag turned on
 * @returns number of layers rendered
 */
extern int render_all_layers(bool changedonly);

/**
 * @brief compiles all sprites to the final image cache
 * @note This function DOES render layers
 */
extern void compile();

/**
 * @brief add a sprite to the layer at the first available level
 * @param ns pointer to the new sprite
 * @param id target layer's ID
 * @return level num on success (positive int < MAX_SPRITES).
 * @return -1 invalid argument.
 * @return -2 no more space on layer
 * @note automatically sets "in use" and "changed" flags on
*/
extern int add_sprite(sprite *ns, int id);

/**
 * @brief attempt to insert a sprite at a specific level
 * @param ns pointer to new sprite
 * @param id target layer's ID
 * @param level target level within the layer
 * @note if a sprite already inhabits the target level, it and all higher sprites will be shifted up.
 * @returns 0 on success.
 * @returns -1 invalid argument.
 * @returns -2 failure.
*/
extern int insert_sprite(sprite *ns, int id, int level);

/**
 * @brief remove a sprite from layer
 * @note this will NOT free sprite data
 * @param level specify which level within the layer to remove
 * @param layer which layer contains the target sprite
 * @returns 0 on success.
 * @returns -1 invalid arguments.
*/
extern int remove_sprite(int level, int layer);

/**
 * @brief initiate a sprite that already exists in heap or stack
 * @param sp pointer to sprite
 * @param width width of the sprite in pixels
 * @param height height of the sprite in pixels
 * @param x x coordinate
 * @param y y coordinate
 * @param pixmap pointer to pixmap
*/
extern int init_sprite(sprite *sp, int width, int height, int x, int y, Color *pixmap);

/**
 * @brief delete a sprite
 * @param sp sprite pointer
 * @note if your sprite's pixmap is in your local stack, null the pointer first. This function assumes that the pixmap is on the heap.
*/
extern void delete_sprite(sprite *sp);

/**
 * @brief find a sprite within a layer
 * @param sp sprite pointer that we're looking for
 * @param layer layer in which the sprite exists
 * @returns level the sprite is found at (positive int < MAX_SPRITES).
 * @returns -1 if not found.
*/
extern int find_sprite(sprite *sp, int layer);

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
extern int move_sprite(int f_level, int f_layer, int t_level, int t_layer);

/**
 * @brief add a sprites to an entity
 * @returns 0 for success
 * @note Append a NULL pointer at the end of your list of sprites
 */
extern int add_sprites_to_entity(entity *ent, ...);

/**
 * @brief update the collision flags for an entity
 * @param ent entity to update collisions for
 * @param lid layer id to test collision against
 */
extern void ent_update_collision(entity *ent, int lid);

/**
 * @brief Open DIL file, include used files, and return the address of the entire program.
 * @param filename path to the DIL file
 * @note this is under development and not fully supported yet
*/
extern char *open_dil(char const *filename);

/**
 * @brief Execute a DIL file
 * @note Must be preceeded with a successful open_dil call.
 * @note This is under development and not fully supported yet.
*/
extern int run();


/*************
 * VARIABLES *
 *************/

extern struct screen screen_dat;

extern unsigned long long _frames; 

extern layer layers[MAX_LAYERS];

extern bool kill_program;

extern const int windowWidth, windowHeight;

#endif