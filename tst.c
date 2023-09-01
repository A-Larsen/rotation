#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SDL_main main
#define FRAME_COUNT 16
#define FPS 16
/* #define M_PI 3.14159265359 */
const int point_size = 10;
const int radius = 100;

void drawCircle(SDL_Surface *surf, float radius, SDL_Point center, uint32_t color)
{
    
    float x = 0;
    float y = 0;
    for(float i = 0; i < 2 * M_PI; i += 0.001f)
    {
        x = cos(2.0f * M_PI * i);
        y = sin(2.0f * M_PI * i);
        SDL_Rect point = {
            (x * radius) + center.x, 
            (y * radius) + center.y,
            1, 
            1
        };
        SDL_FillRect(surf, &point, color);
    }
    /* sin(M_PI * 2) */
}

void drawSpiningDot(SDL_Surface *surf, float radius, SDL_Point center, uint32_t color)
{
    
    float x = 0;
    float y = 0;
    static float i = 0;
    if (i >= 2 * M_PI) i = 0;
    x = cos(2.0f * M_PI * i);
    y = sin(2.0f * M_PI * i);
    SDL_Rect point = {
        (x * radius) + center.x, 
        (y * radius) + center.y,
        point_size, 
        point_size
    };
    SDL_FillRect(surf, &point, color);
    i += (2 * (float)M_PI) / FRAME_COUNT;
}

void updateToFPS(SDL_Window *win, SDL_Surface *surf, uint8_t fps, void (*callback)
                                      (SDL_Window *win, SDL_Surface *surf))
{
    uint32_t goal_mspf = 1000 / fps;

    uint32_t start = SDL_GetTicks();
    callback(win, surf);
    uint32_t end = SDL_GetTicks();
    uint32_t elapsed_time = end - start;
    if (elapsed_time < goal_mspf) SDL_Delay(goal_mspf - elapsed_time);
}

void update(SDL_Window *win, SDL_Surface *surf)
{

    /* SDL_Rect rect = { 0, 0, 200, 200 }; */


    SDL_FillRect(surf, NULL, 0xFFFFFFFF);
    SDL_Point center1 = {125, 125};
    drawCircle(surf, radius, center1, 0x00000000);

    SDL_Point center2 = {125 - (point_size / 2), 125 - (point_size / 2)};
    drawSpiningDot(surf, radius, center2, 0x00FF0000);
    SDL_UpdateWindowSurface(win);

}



int main(void)
{
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret != 0) {
        fprintf(stderr, "Could not initialize SDL\n");
        exit(1);
    }
    SDL_Window *win = SDL_CreateWindow("Measuring Rotational Speed", 100, 100,
                                       800, 800, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);

    /* SDL_Surface *surf = SDL_GetWindowSurface(win); */
    SDL_Surface *backsurf = SDL_CreateRGBSurface(0, 250, 250, 32, 0, 0, 0, 0);

    SDL_Event event;
    bool quit = false;
    float i = 0;
    while(!quit) {

        updateToFPS(win, backsurf, FPS, update);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, backsurf);
        SDL_Rect r1 = {
            .x = 0, .y = 0, .w = 250, .h = 250
        };
        SDL_Rect r2 = {
            .x = 275, .y = 275, .w = 250, .h = 250
        };
        SDL_Point center = {125, 125};

        /* float radians = (float)(2 * M_PI) / 8.0f; */

        float degrees = i * (180.0f / (float)M_PI);
        SDL_RenderCopyEx(ren, tex, &r1, &r2, degrees, &center, SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL);
        SDL_RenderPresent(ren);
        /* SDL_Delay(1000); */
        if (!tex) {
            fprintf(stderr, "could not create texture");
            exit(1);
        }
        /* SDL_BlitSurface(backsurf,  NULL, surf, &r2); */
        /* SDL_UpdateWindowSurface(win); */
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: quit = true; break;
                case SDL_KEYDOWN: 
                {
                        
                    switch(event.key.keysym.sym) {
                        case SDLK_q: 
                        {
                            quit = true;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        SDL_DestroyTexture(tex);
        i -= (2 * (float)M_PI) / FRAME_COUNT;
    }
    SDL_FreeSurface(backsurf);
    SDL_Quit();
}
