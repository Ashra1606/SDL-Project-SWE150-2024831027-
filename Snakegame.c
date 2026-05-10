#include<stdio.h>
#include <SDL3/SDL.h>

#define width 900
#define height 600
int main()
{
    printf("Hello snake!\n");

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window=SDL_CreateWindow("Snake",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);

    SDL_Surface* surface=SDL_GetWindowSurface(window);
    SDL_Rect rect={200,200,200,200};
    SDL_FillRect(surface,&rect,0xFFFFFF);
    SDL_Delay(5000);
}