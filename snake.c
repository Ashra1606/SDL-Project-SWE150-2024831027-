#include<stdio.h>
#include <SDL3/SDL.h>

#define width 900
#define height 600
#define cell_size 15
#define row height/cell_size
#define col width/cell_size
#define GRID_COLOR 0x444444FF
#define line_width 2
#define color_White 0xfffffff

int draw_grid(SDL_Surface *surface)
{
    for(int i=0;i<row;i++)
    {
        SDL_Rect line_y={0,i*cell_size,width,line_width};
        SDL_FillSurfaceRect(surface,&line_y,GRID_COLOR);
    }
     for(int j=0;j<col;j++)
    {
        SDL_Rect line_x={j*cell_size,0,line_width,height};
        SDL_FillSurfaceRect(surface,&line_x,GRID_COLOR); 
    }
    
}
void fill_cell(SDL_Surface* surface, int x, int y,Uint32 color)
{
    SDL_Rect rect={ x*cell_size,y*cell_size,cell_size,cell_size};
    SDL_FillSurfaceRect(surface, &rect,color);
}
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
    SDL_Event event;
    int game=1;
    while(game)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
        {
            game=0;
        }
        }
    
    SDL_FillSurfaceRect(surface, NULL, 0x000000FF);
    draw_grid(surface);
    fill_cell(surface,9,6,color_White);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(16);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}