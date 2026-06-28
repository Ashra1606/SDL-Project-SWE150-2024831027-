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

// Game States
#define STATE_PLAYING 1
#define STATE_GAME_OVER 0

struct snakeElement {
    int x, y;//stores position 
    struct snakeElement *next; //pointer addressing next segment
};

// --- VISUAL 5x5 RETRO BITMAP FONT SYSTEM ---
// Maps alphanumeric characters onto a predictable 5x5 pixel array matrix for the game over screen.
void draw_char(SDL_Renderer* renderer, char c, float start_x, float start_y, float pixel_size) {
    unsigned char rows[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

    switch (c) {
        case '0': rows[0]=0x0E; rows[1]=0x0A; rows[2]=0x0A; rows[3]=0x0A; rows[4]=0x0E; break;
        case '1': rows[0]=0x04; rows[1]=0x0C; rows[2]=0x04; rows[3]=0x04; rows[4]=0x0E; break;
        case '2': rows[0]=0x0E; rows[1]=0x02; rows[2]=0x0E; rows[3]=0x08; rows[4]=0x0E; break;
        case '3': rows[0]=0x0E; rows[1]=0x02; rows[2]=0x0E; rows[3]=0x02; rows[4]=0x0E; break;
        case '4': rows[0]=0x0A; rows[1]=0x0A; rows[2]=0x0E; rows[3]=0x02; rows[4]=0x02; break;
        case '5': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x0E; rows[3]=0x02; rows[4]=0x0E; break;
        case '6': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x0E; rows[3]=0x0A; rows[4]=0x0E; break;
        case '7': rows[0]=0x0E; rows[1]=0x02; rows[2]=0x02; rows[3]=0x02; rows[4]=0x02; break;
        case '8': rows[0]=0x0E; rows[1]=0x0A; rows[2]=0x0E; rows[3]=0x0A; rows[4]=0x0E; break;
        case '9': rows[0]=0x0E; rows[1]=0x0A; rows[2]=0x0E; rows[3]=0x02; rows[4]=0x02; break;
        case 'A': rows[0]=0x0E; rows[1]=0x0A; rows[2]=0x0E; rows[3]=0x0A; rows[4]=0x0A; break;
        case 'B': rows[0]=0x0C; rows[1]=0x0A; rows[2]=0x0C; rows[3]=0x0A; rows[4]=0x0C; break;
        case 'C': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x08; rows[3]=0x08; rows[4]=0x0E; break;
        case 'D': rows[0]=0x0C; rows[1]=0x0A; rows[2]=0x0A; rows[3]=0x0A; rows[4]=0x0C; break;
        case 'E': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x0E; rows[3]=0x08; rows[4]=0x0E; break;
        case 'F': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x0E; rows[3]=0x08; rows[4]=0x08; break;
        case 'G': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x0B; rows[3]=0x0A; rows[4]=0x0E; break;
        case 'H': rows[0]=0x0A; rows[1]=0x0A; rows[2]=0x0E; rows[3]=0x0A; rows[4]=0x0A; break;
        case 'I': rows[0]=0x0E; rows[1]=0x04; rows[2]=0x04; rows[3]=0x04; rows[4]=0x0E; break;
        case 'L': rows[0]=0x08; rows[1]=0x08; rows[2]=0x08; rows[3]=0x08; rows[4]=0x0E; break;
        case 'M': rows[0]=0x11; rows[1]=0x1B; rows[2]=0x15; rows[3]=0x11; rows[4]=0x11; break;
        case 'N': rows[0]=0x11; rows[1]=0x13; rows[2]=0x15; rows[3]=0x19; rows[4]=0x11; break;
        case 'O': rows[0]=0x0E; rows[1]=0x0A; rows[2]=0x0A; rows[3]=0x0A; rows[4]=0x0E; break;
        case 'R': rows[0]=0x0C; rows[1]=0x0A; rows[2]=0x0C; rows[3]=0x0A; rows[4]=0x0A; break;
        case 'S': rows[0]=0x0E; rows[1]=0x08; rows[2]=0x0E; rows[3]=0x02; rows[4]=0x0E; break;
        case 'T': rows[0]=0x0E; rows[1]=0x04; rows[2]=0x04; rows[3]=0x04; rows[4]=0x04; break;
        case 'U': rows[0]=0x0A; rows[1]=0x0A; rows[2]=0x0A; rows[3]=0x0A; rows[4]=0x0E; break;
        case 'V': rows[0]=0x0A; rows[1]=0x0A; rows[2]=0x0A; rows[3]=0x0A; rows[4]=0x04; break;
        case 'W': rows[0]=0x11; rows[1]=0x11; rows[2]=0x15; rows[3]=0x1B; rows[4]=0x11; break;
        case ':': rows[0]=0x00; rows[1]=0x04; rows[2]=0x00; rows[3]=0x04; rows[4]=0x00; break;
        case ' ': default: break;
    }

    for (int r = 0; r < 5; r++) {
        for (int c_idx = 0; c_idx < 5; c_idx++) {
            if ((rows[r] >> (4 - c_idx)) & 1) {
                SDL_FRect pixel = { 
                    start_x + (c_idx * pixel_size), 
                    start_y + (r * pixel_size), 
                    pixel_size, 
                    pixel_size 
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
}

// Iterates through string arrays to pass characters into the text renderer pipeline.
void draw_text(SDL_Renderer* renderer, const char* text, float x, float y, float pixel_size) {
    int i = 0;
    while (text[i] != '\0') {
        draw_char(renderer, text[i], x + (i * 6 * pixel_size), y, pixel_size);
        i++;
    }
}

//using renderer

void draw_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x44, 0x44, 0x44, 0xFF); //gray grid lines(renderer, redvalue,greenvalue,bluevalue,alpha or transparency)
    //draw horizontal line
    for(int i = 0; i < row; i++) {
        SDL_FRect line_y = { 0.0f, (float)(i * cell_size), (float)width, (float)line_width };//rectangle floating point(x,y,w,h)
        SDL_RenderFillRect(renderer, &line_y);
    }
    //draw vertical line
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

void draw_snake(SDL_Renderer* renderer, struct snakeElement* element)//pointer to the head
 {
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

// Walks through the linked list to free heap nodes on closing down.
void free_snake(struct snakeElement* head) {
    struct snakeElement* current = head->next;
    while(current != NULL) {
        struct snakeElement* next = current->next;
        free(current);
        current = next;
    }
    head->next = NULL;
}

int main(int argc, char* argv[]) {
    printf("Hello snake!\n");

    if (SDL_Init(SDL_INIT_VIDEO) == -1)// initialize sdl3 video system 
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    
    // Customization: Injected dynamic window framework string placeholder logic
    SDL_Window *window = SDL_CreateWindow("Snake - Score: 0", width, height, 0);
    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

// initialize rendering

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) 
    {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Event event;
    struct snakeElement snake = {5, 5, NULL};// initialize head at (5,5)
    
    // Modified game tracking control loops to run on explicit state evaluations
    int game_state = STATE_PLAYING;
    int running = 1; 
    
    int score = 0;

    srand(time(NULL));//ensures everytime the food spawns in unique spots
    int food_x = rand() % col;
    int food_y = rand() % row;

    int bonus_x=-1;
    int bonus_y=-1;
    int bonus_active=0;
    Uint64 bonus_spawn_time=0;
    Uint64 bonus_duration=5000;//bonus lifetime duration
    
    int vx = 1;//continuos movement on the x axis at the beginning
    int vy = 0;

    //Using SDL_GetTicks() sets up an explicit timer restriction.
    //This controls the snake's update rate independently of screen refresh rates.
    Uint64 last_move_time = SDL_GetTicks();
    Uint64 move_interval = 150;// The game ticks every 150 milliseconds
    
    printf("Current Score: %d\n", score);

    while(running) {
        // Handle input events
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) 
            {
                running = 0;
            }
            //Checking 'vx == 0' or 'vy == 0' prevents self-destruction.
            //It stops the snake from turning instantly backward into its own neck.

            if(game_state == STATE_PLAYING && event.type == SDL_EVENT_KEY_DOWN) 
            {
                if(event.key.key == SDLK_RIGHT && vx == 0)//checks if the snake is already moving horizontal or not 
                {
                    vx = 1;
                    vy = 0;
                }
                if(event.key.key == SDLK_LEFT && vx == 0) 
                {
                    vx = -1;
                    vy = 0;
                }
                if(event.key.key == SDLK_UP && vy == 0) 
                {
                    vx = 0;
                    vy = -1;
                }
                if(event.key.key == SDLK_DOWN && vy == 0) 
                {
                    vx = 0;
                    vy = 1;
                }
            }
        }
        
        // Wrap playing updates inside state router condition to block movement on loss
        if (game_state == STATE_PLAYING) {
            Uint64 current_time = SDL_GetTicks();
            if(current_time - last_move_time >= move_interval)// Checks if 150 milliseconds have elapsed since the last time the snake advanced forward
            {
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
                    game_state = STATE_GAME_OVER;
                }
                if(game_state == STATE_PLAYING && check_self_collision(&snake)) {
                    printf("Game Over: Self Collision Detected!\n");
                    printf("Final Score: %d\n", score);
                    game_state = STATE_GAME_OVER;
                }
                last_move_time = current_time;
            }
            
            // Target collision detection (eating food)

            if(snake.x == food_x && snake.y == food_y) 
            {
                add_segment(&snake);
                score += 10;
                printf("Current Score: %d\n", score);
                
                // Write active real-time updates onto the native OS application frame tab bar
                char title_str[64];
                snprintf(title_str, sizeof(title_str), "Snake - Score: %d", score);
                SDL_SetWindowTitle(window, title_str);

                //generate a new food at random position
                food_x = rand() % col;
                food_y = rand() % row;

                if(!bonus_active && (rand() % 5 == 0)) //Triggers a mathematical 20% random spawn chance whenever normal food is eaten
                {
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
                    
                    // Keep native OS tab frame synchronized during high-value targets
                    char title_str[64];
                    snprintf(title_str, sizeof(title_str), "Snake - Score: %d", score);
                    SDL_SetWindowTitle(window, title_str);

                    bonus_active = 0;    // Turn off visibility flag until next spawn trigger
                } 
                // Check if the current elapsed runtime minus the spawn moment exceeds 5000ms
                else if(SDL_GetTicks() - bonus_spawn_time >= bonus_duration) {
                    bonus_active = 0;    // The timer ran out! Despawn the item silently
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);// Set background color brush to black
        SDL_RenderClear(renderer);
        
        if (game_state == STATE_PLAYING) {
            draw_grid(renderer);
            fill_cell(renderer, food_x, food_y, 255, 0, 0);

            // Conditional drawing for bonus food
            if(bonus_active) {
                fill_cell(renderer, bonus_x, bonus_y, 0, 255, 0);
            }
            draw_snake(renderer, &snake);
            
        } else if (game_state == STATE_GAME_OVER) {
            // Signal a terminal loss state up to the OS window wrapper frame
            SDL_SetWindowTitle(window, "Snake - Game Over!");

            // Print beautifully structured and clean endgame typography cards centered on screen
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
            draw_text(renderer, "GAME OVER", width / 2.0f - 160.0f, height / 2.0f - 50.0f, 6.0f);
            
            char final_score_str[32];
            snprintf(final_score_str, sizeof(final_score_str), "FINAL SCORE: %d", score);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
            draw_text(renderer, final_score_str, width / 2.0f - 140.0f, height / 2.0f + 20.0f, 3.0f);
        }
        
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);
    }
    
    free_snake(&snake);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}