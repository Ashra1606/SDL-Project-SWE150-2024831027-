#include <stdio.h>
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <time.h>

//configuration constant

#define width 900
#define height 600
#define cell_size 15
#define row (height/cell_size)
#define col (width/cell_size)
#define GRID_COLOR 0xFF444444
#define line_width 2
#define color_White 0xFFFFFFFF
#define food 0xFFFF0000

struct snakeElement {
    int x, y;
    struct snakeElement *next; //pointer addressing next segment
};

//using renderer

void draw_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x44, 0x44, 0x44, 0xFF); //gray grid lines
    for(int i = 0; i < row; i++) {
        SDL_FRect line_y = { 0.0f, (float)(i * cell_size), (float)width, (float)line_width };
        SDL_RenderFillRect(renderer, &line_y);
    }
    for(int j = 0; j < col; j++) {
        SDL_FRect line_x = { (float)(j * cell_size), 0.0f, (float)line_width, (float)height };
        SDL_RenderFillRect(renderer, &line_x); 
    }
}

//paint indiviual grid

void fill_cell(SDL_Renderer* renderer, int x, int y, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_FRect rect = { (float)(x * cell_size), (float)(y * cell_size), (float)cell_size, (float)cell_size };
    SDL_RenderFillRect(renderer, &rect);
}

// add renderer to the new segment of the snake

void draw_snake(SDL_Renderer* renderer, struct snakeElement* element) {
    while(element != NULL) {
        fill_cell(renderer, element->x, element->y, 255, 255, 255);
        element = element->next;
    }
}

//add new tail at the very end of the list 

void add_segment(struct snakeElement* head) {
    struct snakeElement* current = head;
    while(current->next != NULL) {
        current = current->next;
    }
    struct snakeElement* new_segment = malloc(sizeof(struct snakeElement));
    new_segment->x = current->x;
    new_segment->y = current->y;
    new_segment->next = NULL;
    current->next = new_segment;
}

//Shifts the position of every segment to follow the cell ahead of it.
//It passes old coordinates down the chain whenever the head takes a step forward.

void move_tail(struct snakeElement* head, int prev_x, int prev_y) {
    struct snakeElement *current = head->next;
    while(current != NULL) {
        int temp_x = current->x;
        int temp_y = current->y;
        current->x = prev_x;
        current->y = prev_y;
        prev_x = temp_x;
        prev_y = temp_y;
        current = current->next;
    }
}

// Added to handle the foundational lose condition where the snake bites its own tail.

int check_self_collision(struct snakeElement* head) {
    struct snakeElement* current = head->next;
    while(current != NULL) {
        if(head->x == current->x && head->y == current->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    printf("Hello snake!\n");

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow("Snake", width, height, 0);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

// initialize rendering

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Event event;
    struct snakeElement snake = {5, 5, NULL};
    int game = 1;
    
    int score = 0;

    srand(time(NULL));
    int food_x = rand() % col;
    int food_y = rand() % row;

    int bonus_x=-1;
    int bonus_y=-1;
    int bonus_active=0;
    Uint64 bonus_spawn_time=0;
    Uint64 bonus_duration=5000;
    
    int vx = 1;
    int vy = 0;

    //Using SDL_GetTicks() sets up an explicit timer restriction.
    //This controls the snake's update rate independently of screen refresh rates.
    Uint64 last_move_time = SDL_GetTicks();
    Uint64 move_interval = 150;// The game ticks every 150 milliseconds
    
    printf("Current Score: %d\n", score);

    while(game) {
        // Handle input events
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                game = 0;
            }
            //Checking 'vx == 0' or 'vy == 0' prevents self-destruction.
            //It stops the snake from turning instantly backward into its own neck.

            if(event.type == SDL_EVENT_KEY_DOWN) {
                if(event.key.key == SDLK_RIGHT && vx == 0) {
                    vx = 1;
                    vy = 0;
                }
                if(event.key.key == SDLK_LEFT && vx == 0) {
                    vx = -1;
                    vy = 0;
                }
                if(event.key.key == SDLK_UP && vy == 0) {
                    vx = 0;
                    vy = -1;
                }
                if(event.key.key == SDLK_DOWN && vy == 0) {
                    vx = 0;
                    vy = 1;
                }
            }
        }
        
        Uint64 current_time = SDL_GetTicks();
        if(current_time - last_move_time >= move_interval) {
            int old_x = snake.x;
            int old_y = snake.y;

            // Update head location

            snake.x += vx;
            snake.y += vy;

            // Sync tail segments to follow the updated head pathway

            move_tail(&snake, old_x, old_y);
            
           // Bounds validation must sit inside the timed movement conditional blocks
            //so we check for death exactly when the snake changes its grid coordinates. 

            if(snake.x < 0 || snake.x >= col || snake.y < 0 || snake.y >= row ) {
                printf("Game Over: Boundary Collision Detected!\n");
                printf("Final Score: %d\n", score);
                game = 0;
            }
            if(check_self_collision(&snake)) {
                printf("Game Over: Self Collision Detected!\n");
                printf("Final Score: %d\n", score);
                game = 0;
            }
            last_move_time = current_time;
        }
        
        // Target collision detection (eating food)

        if(snake.x == food_x && snake.y == food_y) {
            add_segment(&snake);
            score += 10;
            printf("Current Score: %d\n", score);
            food_x = rand() % col;
            food_y = rand() % row;

            if(!bonus_active && (rand() % 5 == 0)) {
                bonus_x = rand() % col;             // Assign random column position
                bonus_y = rand() % row;             // Assign random row position
                bonus_active = 1;                   // Turn on the visibility flag
                bonus_spawn_time = SDL_GetTicks();  // Mark the exact millisecond it appeared
            }
        }
        
        if(bonus_active) {
            // Check if snake head successfully occupies the bonus coordinate space
            if(snake.x == bonus_x && snake.y == bonus_y) {
                add_segment(&snake); // Grow segment 1
                add_segment(&snake); // Grow segment 2 (Double growth penalty/reward for bonus target!)
                score += 50;
                printf("BONUS! Current Score: %d\n", score);
                bonus_active = 0;    // Turn off visibility flag until next spawn trigger
            } 
            // Check if the current elapsed runtime minus the spawn moment exceeds 5000ms
            else if(SDL_GetTicks() - bonus_spawn_time >= bonus_duration) {
                bonus_active = 0;    // The timer ran out! Despawn the item silently
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);// Set background color brush to black
        SDL_RenderClear(renderer);
        
        draw_grid(renderer);
        fill_cell(renderer, food_x, food_y, 255, 0, 0);

        // Conditional drawing for bonus food
        if(bonus_active) {
            fill_cell(renderer, bonus_x, bonus_y, 0, 255, 0);
        }
        draw_snake(renderer, &snake);
        
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}