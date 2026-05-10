#include<stdio.h>
#include <SDL3/SDL.h>

#define width 900
#define height 600
int main()
{
    printf("Hello snake!\n");

    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window=SDL_CreateWindow("Snake",width,height,0);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Surface* surface=SDL_GetWindowSurface(window);
    SDL_Rect rect={200,200,200,200};
    SDL_FillSurfaceRect(surface, &rect, 0xFFFFFFFF);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(5000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}