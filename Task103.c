#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 700

int R = 60;
SDL_Window *win = NULL;
SDL_Renderer *rend = NULL;
bool gameIsRunning = false;

// for first circle
int X = -60; // Initialized with R value
int Y = SCREEN_HEIGHT / 2;

// for second circle
int x = SCREEN_WIDTH / 2;
int y = SCREEN_HEIGHT - 60;

int r_color = 200;
int g_color = 50;
int f = 0;

bool starting() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Error: SDL failed to initialize: %s\n", SDL_GetError());
        return false;
    }

    // SDL3: SDL_CreateWindowAndRenderer is a convenient way to do both
    if (!SDL_CreateWindowAndRenderer("Collision between two circles (SDL3)", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &win, &rend)) {
        SDL_Log("Error: Failed to create window/renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool collision(int x1, int y1, int x2, int y2) {
    // Distance formula: distance^2 = (x2-x1)^2 + (y2-y1)^2
    float dx = (float)(x1 - x2);
    float dy = (float)(y1 - y2);
    float distance = SDL_sqrtf(dx * dx + dy * dy);
    
    // Collision if distance is less than sum of radii (R + R = 2R)
    return (distance <= 2.0f * R);
}

void update() {
    if (collision(x, y, X, Y)) {
        X = -R;
        if (f == 0) {
            r_color = 50; g_color = 200; f = 1;
        } else {
            r_color = 200; g_color = 50; f = 0;
        }
    } else {
        X += 4;
        if (X >= SCREEN_WIDTH + R) {
            X = -R;
        }
    }
}

void createCircle(int centerX, int centerY, int radius) {
    // Note: SDL3 uses float coordinates for many drawing functions (SDL_RenderPoint)
    for (int cx = -radius; cx <= radius; cx++) {
        for (int cy = -radius; cy <= radius; cy++) {
            if (cx * cx + cy * cy <= radius * radius) {
                SDL_RenderPoint(rend, (float)(centerX + cx), (float)(centerY + cy));
            }
        }
    }
}

void game_loop() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            gameIsRunning = false;
        } else if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.key) {
                case SDLK_UP:
                    y -= 10;
                    if (y <= R) { x = SCREEN_WIDTH / 2; y = SCREEN_HEIGHT - R; }
                    break;
                case SDLK_DOWN:
                    y += 10;
                    if (y + R >= SCREEN_HEIGHT) { x = SCREEN_WIDTH / 2; y = SCREEN_HEIGHT - R; }
                    break;
                case SDLK_LEFT:
                    x -= 10;
                    if (x <= R) { x = SCREEN_WIDTH / 2; y = SCREEN_HEIGHT - R; }
                    break;
                case SDLK_RIGHT:
                    x += 10;
                    if (x + R >= SCREEN_WIDTH) { x = SCREEN_WIDTH / 2; y = SCREEN_HEIGHT - R; }
                    break;
                case SDLK_ESCAPE:
                    gameIsRunning = false;
                    break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    gameIsRunning = starting();

    while (gameIsRunning) {
        game_loop();

        // SDL3: Colors are now represented by SDL_FColor (0.0f to 1.0f) 
        // or set via standard 0-255 using SDL_SetRenderDrawColor
        SDL_SetRenderDrawColor(rend, 88, 198, 200, 255);
        SDL_RenderClear(rend);

        update();

        // Draw First Circle
        SDL_SetRenderDrawColor(rend, (Uint8)r_color, (Uint8)g_color, 0, 255);
        createCircle(X, Y, R);

        // Draw Second Circle
        SDL_SetRenderDrawColor(rend, (Uint8)g_color, (Uint8)r_color, 0, 255);
        createCircle(x, y, R);

        SDL_RenderPresent(rend);
        
        // Add a small delay to prevent 100% CPU usage
        SDL_Delay(16); 
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}