#include<stdio.h>
#include <SDL3/SDL.h>
#include<stdlib.h>
#include<time.h>
#define width 900
#define height 600
#define cell_size 15
#define row height/cell_size
#define col width/cell_size
#define GRID_COLOR 0xFF444444
#define line_width 2
#define color_White 0xFFFFFFFF
#define snake(x,y) 
#define food 0xFFFF0000


void draw_grid(SDL_Surface *surface)
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
    int snake_x=5;
    int snake_y=5;
    srand(time(NULL));
    int food_x=rand() % col;
    int food_y=rand() % row;
    while(game)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
        {
            game=0;
        }
        if(event.type== SDL_EVENT_KEY_DOWN)
        {
            if(event.key.key == SDLK_RIGHT)
            {
                snake_x++;
            }
             if(event.key.key == SDLK_LEFT)
            {
                snake_x--;
            }
            if(event.key.key == SDLK_UP)
            {
                snake_y--;
            }
             if(event.key.key == SDLK_DOWN)
            {
                snake_y++;
            }
        }
        }
        if(snake_x == food_x && snake_y==food_y)
        {
            food_x=rand()%col;
            food_y=rand()%row;
        }
    
    SDL_FillSurfaceRect(surface, NULL, 0xFF000000);
    draw_grid(surface);
    fill_cell(surface,food_x,food_y,food);
    fill_cell(surface,snake_x,snake_y,color_White);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(16);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}