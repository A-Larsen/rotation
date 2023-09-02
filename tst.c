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
#define FRAME_COUNT 60
#define FPS 5
/* #define M_PI 3.14159265359 */
const int point_size = 10;
const int radius = 100;
bool quit = false;

void drawCircle(SDL_Renderer *ren, float radius, SDL_Point center, uint32_t color)
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
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderFillRect(ren, &point);
    }
}

void drawSpiningDot(SDL_Renderer *ren, float radius, SDL_Point center, uint32_t color)
{
    
    static float i = 0;
    if (i >= 2 * M_PI) i = 0;
    float radians = 2.0f * M_PI * i;
    // point1 -> show the default clockwise rotation
    //
    // point2 -> show the default clockwise rotation, then rotating it
    //           clockwise again at the same degrees in radians
    //
    // point3 -> show the default clockwise rotation, then rotating it
    //           anticlockwise again at the same degrees in radians
    float x1 = cosf(radians) * radius;
    float y1 = sinf(radians) * radius;

    float x2 = x1 * cosf(radians) + y1 * sinf(radians);
    float y2 = x1 * sinf(radians) - y1 * cosf(radians);

    float x3 = x1 * cosf(radians) - y1 * sinf(radians);
    float y3 = x1 * sinf(radians) + y1 * cosf(radians);
    SDL_Rect point1 = {
        x1 + center.x, y1 + center.y,
        point_size, 
        point_size
    };
    SDL_Rect point2 = {
        x2 + center.x, y2 + center.y,
        point_size, 
        point_size
    };
    SDL_Rect point3 = {
        x3 + center.x, y3 + center.y,
        point_size, 
        point_size
    };
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    SDL_RenderFillRect(ren, &point1);
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    SDL_RenderFillRect(ren, &point2);
    SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
    SDL_RenderFillRect(ren, &point3);
    i += (2 * (float)M_PI) / FRAME_COUNT;
}

void updateToFPS(SDL_Renderer *ren, uint8_t fps, void (*callback)
                                      (SDL_Renderer *ren))
{
    uint32_t goal_mspf = 1000 / fps;

    uint32_t start = SDL_GetTicks();
    callback(ren);
    uint32_t end = SDL_GetTicks();
    uint32_t elapsed_time = end - start;
    if (elapsed_time < goal_mspf) SDL_Delay(goal_mspf - elapsed_time);
}

void update(SDL_Renderer *ren)
{

    static int frame_number = 0;
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_Point center1 = {125, 125};
    drawCircle(ren, radius, center1, 0x00000000);

    SDL_Point center2 = {125 - (point_size / 2), 125 - (point_size / 2)};
    drawSpiningDot(ren, radius, center2, 0x00FF0000);
    SDL_RenderPresent(ren);
    frame_number = (frame_number + 1) % FRAME_COUNT;
    SDL_Event event;
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

}



int main(void)
{
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret != 0) {
        fprintf(stderr, "Could not initialize SDL\n");
        exit(1);
    }
    SDL_Window *win = SDL_CreateWindow("Measuring Rotational Speed", 100, 100,
                                       250, 250, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_Event event;
    float i = 0;
    while(!quit) {

        updateToFPS(ren,  FPS, update);
    }
    SDL_DestroyRenderer(ren);
    SDL_Quit();
}
