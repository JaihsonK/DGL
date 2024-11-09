#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "definitions.h"
#include "pthread.h"
#include "render/render.h"
#include "sprite/sprite.h"

int windowWidth, windowHeight;
char title[51] = "Dodge Gaming Library";

extern char *get_item(FILE *fp, char *item);

void terminate()
{
    screen_dat.pause_render = true;
    kill_render = true;
    CloseWindow();
    clean_render_system();
    printf("\nDGL: terminating\n");
    
    while(IsWindowReady());

}

int start(char *session_dat)
{
    atexit(terminate);
    printf("Dodge Graphics Library version %s (c) 2024 Jaihson Kresak\na 2D graphics library based on Raylib by Ramon Santamaria.", DGL_version);

    FILE *session_fp = fopen(session_dat, "r");
    strncpy(title, get_item(session_fp, "title"), 50);
    printf("\n%s\n", title);

    windowWidth = (windowWidth = atoi(get_item(session_fp, "windowWidth"))) < 2000 && windowWidth > 10 ? windowWidth : 800;
    windowHeight = (windowHeight = atoi(get_item(session_fp, "windowHeight"))) < 1500 && windowHeight > 10 ? windowHeight : 500;

    init_render_system();

    screen_dat.pause_render = true;
    pthread_t gfx_processor;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&gfx_processor, &attr, render, title);

    return 0;
}

