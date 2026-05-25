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

struct snakeElement{
    int x,y;
    struct snakeElement *next;
};
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
void draw_snake(SDL_Surface* surface, struct snakeElement* element)
{
    while(element != NULL)
    {
        fill_cell(surface, element->x, element->y, color_White);
        element = element->next;
    }
}
void add_segment(struct  snakeElement* head)
{
    struct snakeElement* current=head;
    while(current->next!=NULL)
    {
        current=current->next;
    }
    struct snakeElement* new_segment=malloc(sizeof(struct snakeElement));
    new_segment->x=current->x;
    new_segment->y=current->y;
    new_segment->next=NULL;
    current->next=new_segment;
}
void move_tail(struct snakeElement* head,int prev_x,int prev_y)
{
    struct  snakeElement *current=head->next;
    while(current!=NULL)
    {
        int temp_x=current->x;
        int temp_y=current->y;
        current->x=prev_x;
        current->y=prev_y;
        prev_x=temp_x;
        prev_y=temp_y;
        current=current->next;
    }
            
}
int check_self_collision(struct snakeElement* head)
{
    struct snakeElement* current=head->next;
    while(current!=NULL)
    {
        if(head->x == current->x && head->y == current->y)
        {
            return 1;
        }
        current= current->next;
    }
    return 0;
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
    struct snakeElement snake={5,5,NULL};
    int game=1;
    srand(time(NULL));
    int food_x=rand() % col;
    int food_y=rand() % row;
    
    int vx=1;
    int vy=0;

    Uint64 last_move_time = SDL_GetTicks();
    Uint64 move_interval =150;
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
            if(event.key.key == SDLK_RIGHT && vx==0)
            {
                vx=1;
                vy=0;
            }
             if(event.key.key == SDLK_LEFT && vx==0)
            {
                vx=-1;
                vy=0;
            }
            if(event.key.key == SDLK_UP && vy==0)
            {
                vx=0;
                vy=-1;
            }
             if(event.key.key == SDLK_DOWN && vy==0)
            {
                vx=0;
                vy=1;
            }
        }
        }
        Uint64 current_time = SDL_GetTicks();
        if(current_time-last_move_time >= move_interval)
        {
        int old_x=snake.x;
        int old_y=snake.y;

        snake.x +=vx;
        snake.y +=vy;
        move_tail(&snake, old_x,old_y);
        if(snake.x < 0 || snake.x >= col || snake.y < 0 || snake.y >= row )
        {
            printf("Game Over: Boundary Collision Detected!\n");
            game=0;
        }
        if(check_self_collision(&snake))
        {
            printf("Game Over: Self Collision Detected!\n");
            game=0;
        }
        last_move_time=current_time;
        }
        if(snake.x == food_x && snake.y==food_y)
        {
            add_segment(&snake);
            food_x=rand()%col;
            food_y=rand()%row;
        }
    SDL_FillSurfaceRect(surface, NULL, 0xFF000000);
    draw_grid(surface);
    fill_cell(surface,food_x,food_y,food);
    fill_cell(surface,snake.x,snake.y,color_White);
    draw_snake(surface,&snake);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(16);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}