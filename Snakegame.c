#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 380
#define TILE_SIZE 10
#define MAX_SNAKE_LENGTH 1000

// Struct to replace the Snake class
typedef struct {
    SDL_Rect body[MAX_SNAKE_LENGTH];
    int length;
    SDL_Rect food;
    SDL_Rect bonus;
    SDL_Rect obstacles[4];
    int direction;
    int score;
    Mix_Chunk *eatsound;
} Snake;

// Global variables
int bonusOn = 0;
int bonusCreateTime, p_time;
TTF_Font *font = NULL;

// Function Prototypes
void spawnFood(Snake *snake);
void handleBonus(Snake *snake);

void initSnake(Snake *snake) {
    snake->length = 1;
    snake->body[0] = (SDL_Rect){320, 240, TILE_SIZE, TILE_SIZE};
    snake->direction = 3;
    snake->score = 0;
    snake->bonus = (SDL_Rect){-1, -1, 0, 0};

    // Initialize Obstacles
    int obsW = 3 * TILE_SIZE;
    int obsH = 10 * TILE_SIZE + 20;
    snake->obstacles[0] = (SDL_Rect){100, 50, obsW, obsH};
    snake->obstacles[1] = (SDL_Rect){100, 200, obsW, obsH};
    snake->obstacles[2] = (SDL_Rect){400, 50, obsW, obsH};
    snake->obstacles[3] = (SDL_Rect){400, 200, obsW, obsH};

    spawnFood(snake);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024);
    snake->eatsound = Mix_LoadWAV("eat.wav");
}

int isCollidingWithObstacles(Snake *snake, int x, int y, int w, int h) {
    for (int i = 0; i < 4; i++) {
        if (x < snake->obstacles[i].x + snake->obstacles[i].w &&
            x + w > snake->obstacles[i].x &&
            y < snake->obstacles[i].y + snake->obstacles[i].h &&
            y + h > snake->obstacles[i].y) {
            return 1;
        }
    }
    return 0;
}

void spawnFood(Snake *snake) {
    do {
        snake->food.x = (rand() % (SCREEN_WIDTH / TILE_SIZE)) * TILE_SIZE;
        snake->food.y = (rand() % (SCREEN_HEIGHT / TILE_SIZE)) * TILE_SIZE;
    } while (isCollidingWithObstacles(snake, snake->food.x, snake->food.y, TILE_SIZE, TILE_SIZE));
    snake->food.w = snake->food.h = TILE_SIZE;
}

void spawnBonus(Snake *snake) {
    do {
        snake->bonus.x = (rand() % (SCREEN_WIDTH / TILE_SIZE)) * TILE_SIZE;
        snake->bonus.y = (rand() % (SCREEN_HEIGHT / TILE_SIZE)) * TILE_SIZE;
    } while (isCollidingWithObstacles(snake, snake->bonus.x, snake->bonus.y, TILE_SIZE * 2, TILE_SIZE * 2));
    snake->bonus.w = snake->bonus.h = TILE_SIZE * 2;
}

void render(SDL_Renderer *renderer, Snake *snake) {
    // Background
    SDL_Surface *surface = IMG_Load("./Bground.png");
    SDL_Texture *bgTex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, bgTex, NULL, NULL);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(bgTex);

    // Score Text
    char scoreStr[32];
    sprintf(scoreStr, "Score: %d", snake->score);
    SDL_Color blue = {25, 0, 255, 255};
    surface = TTF_RenderText_Solid(font, scoreStr, blue);
    SDL_Texture *scoreTex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, scoreTex, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(scoreTex);

    // Draw Snake
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < snake->length; i++) {
        SDL_RenderFillRect(renderer, &snake->body[i]);
    }

    // Draw Food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &snake->food);

    // Draw Bonus
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &snake->bonus);

    // Draw Obstacles
    SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
    for (int i = 0; i < 4; i++) {
        SDL_RenderFillRect(renderer, &snake->obstacles[i]);
    }
}

void moveSnake(Snake *snake, SDL_Renderer *renderer) {
    SDL_Rect nextHead = snake->body[0];

    if (snake->direction == 0) nextHead.y -= TILE_SIZE;      // Up
    else if (snake->direction == 1) nextHead.y += TILE_SIZE; // Down
    else if (snake->direction == 2) nextHead.x -= TILE_SIZE; // Left
    else if (snake->direction == 3) nextHead.x += TILE_SIZE; // Right

    // Screen Wrap
    if (nextHead.x < 0) nextHead.x = SCREEN_WIDTH - TILE_SIZE;
    else if (nextHead.x >= SCREEN_WIDTH) nextHead.x = 0;
    if (nextHead.y < 0) nextHead.y = SCREEN_HEIGHT - TILE_SIZE;
    else if (nextHead.y >= SCREEN_HEIGHT) nextHead.y = 0;

    // Bonus logic
    if (bonusOn != snake->score && snake->score > 0 && snake->score % 5 == 0) {
        spawnBonus(snake);
        bonusCreateTime = SDL_GetTicks();
        bonusOn = snake->score;
    }
    if (SDL_GetTicks() - bonusCreateTime >= 4000) handleBonus(snake);

    // Move body
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = nextHead;

    // Food Collision
    if (nextHead.x == snake->food.x && nextHead.y == snake->food.y) {
        Mix_PlayChannel(-1, snake->eatsound, 0);
        snake->score++;
        if (snake->length < MAX_SNAKE_LENGTH) snake->length++;
        spawnFood(snake);
    }

    // Self/Obstacle Collision Check
    if (isCollidingWithObstacles(snake, nextHead.x, nextHead.y, TILE_SIZE, TILE_SIZE)) {
        exit(0); // Simple game over
    }
}

void handleBonus(Snake *snake) {
    snake->bonus = (SDL_Rect){-1, -1, 0, 0};
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    
    font = TTF_OpenFont("KnightWarrior-w16n8.otf", 24);
    SDL_Window *win = SDL_CreateWindow("Snake C", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    Snake snake;
    initSnake(&snake);

    int quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP && snake.direction != 1) snake.direction = 0;
                if (e.key.keysym.sym == SDLK_DOWN && snake.direction != 0) snake.direction = 1;
                if (e.key.keysym.sym == SDLK_LEFT && snake.direction != 3) snake.direction = 2;
                if (e.key.keysym.sym == SDLK_RIGHT && snake.direction != 2) snake.direction = 3;
            }
        }

        moveSnake(&snake, ren);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        render(ren, &snake);
        SDL_RenderPresent(ren);
        SDL_Delay(100);
    }

    return 0;
}