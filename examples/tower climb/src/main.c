#include <dgl.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_PLATFORMS 9

#define MAX_OFFSET 200

int y_offset = -MAX_OFFSET;

int x_mod, y_mod;
size_t counter = 0;

sprite **collision;

union
{
    struct
    {
        uint8_t kp1 : 1;
        uint8_t kp4 : 1;
        uint8_t kp7 : 1;
        uint8_t kp3 : 1;
        uint8_t kp6 : 1;
        uint8_t kp9 : 1;
    };
    uint8_t all;
} keys_down;
bool platform_overflow[MAX_PLATFORMS];
sprite background, platform[MAX_PLATFORMS], player;
Color *platform_pixmap;

char score_msg[] = "score: 00000000";

int main()
{
    start("DGLsession.dat");

    platform_pixmap = pixmap_generate(RECT, shinf(100, 15), DARKGRAY, 0);
    init_sprite(&background, windowWidth, windowHeight, 0, 0, pixmap_generate(RECT, shinf(windowWidth, windowHeight), RAYWHITE, 0));
    add_sprite(&background, 0);
    for (size_t i = 0; i < MAX_PLATFORMS; i++)
    {
        init_sprite(&platform[i], 100, 15, GetRandomValue(0, windowWidth - 100), GetRandomValue(0, y_offset + (windowHeight - 15)) - MAX_OFFSET, platform_pixmap);
        add_sprite(&platform[i], 1);
    }

    init_sprite(&player, 15, 15, windowWidth / 2, 0, pixmap_generate(RECT, shinf(15, 15), RED, NULL));
    insert_sprite(&player, 1, MAX_SPRITES - 1);

    screen_dat.pause_render = false;
    while (!kill_program)
    {
        compile();

        size_t i;
        for (i = 0; i < MAX_PLATFORMS; i++)
        {
            if (platform_overflow[i])
            {
                platform[i].x = GetRandomValue(0, windowWidth - 100);
                platform[i].y = GetRandomValue(0, MAX_OFFSET) - MAX_OFFSET;
                platform_overflow[i] = false;
            }
            platform[i].y += 2;
            if (platform[i].y > windowHeight)
                platform_overflow[i] = true;
        }
        // handle player

        if (!counter)
        {
            keys_down.all = 0;
            x_mod = 0;
        }
        else
        {
            counter--;
            if (keys_down.kp1)
            {
                y_mod = -4;
                x_mod = -2;
            }
            else if(keys_down.kp3)
            {
                y_mod = -4;
                x_mod = 2;
            }
            else if(keys_down.kp4)
            {
                y_mod = -7;
                x_mod = -4;
            }
            else if(keys_down.kp6)
            {
                y_mod = -7;
                x_mod = 4;
            }
            else if(keys_down.kp7)
            {
                y_mod = -10;
                x_mod = -7;
            }
            else if(keys_down.kp9)
            {
                y_mod = -10;
                x_mod = 7;
            }
            
        }
        player.y += y_mod;
        player.x += x_mod;
        if(player.y > windowHeight)
        {
            player.y = 0;
            player.x = 100;
        }
        if ((collision = all_collided(&player, 1))[0] && player.y <= collision[0]->y - 15)
        {
            y_mod = 2;
            if (IsKeyPressed(KEY_KP_1))
            {
                counter = 25;
                keys_down.kp1 = 1;
            }
            else if (IsKeyPressed(KEY_KP_3))
            {
                counter = 25;
                keys_down.kp3 = 1;
            }
            else if (IsKeyPressed(KEY_KP_4))
            {
                counter = 25;
                keys_down.kp4 = 1;
            }
            else if (IsKeyPressed(KEY_KP_6))
            {
                counter = 25;
                keys_down.kp6 = 1;
            }
            else if (IsKeyPressed(KEY_KP_7))
            {
                counter = 25;
                keys_down.kp7 = 1;
            }
            else if (IsKeyPressed(KEY_KP_9))
            {
                counter = 25;
                keys_down.kp9 = 1;
            }
            continue;
        }
        else
            y_mod = 4;
    }

    delete_sprite(&background);
    delete_sprite(&player);
    delete_sprite(&platform[0]);
    exit(0);
}