#include <SDL2/SDL.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int TANK_WIDTH = 32;
const int TANK_HEIGHT = 32;

int tankX = WINDOW_WIDTH / 2 - TANK_WIDTH / 2;
int tankY = WINDOW_HEIGHT / 2 - TANK_HEIGHT / 2;

void handleInput(SDL_Event& event, bool& running) {
    if (event.type == SDL_QUIT) {
        running = false;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
            tankY -= 5;
            break;
        case SDLK_DOWN:
            tankY += 5;
            break;
        case SDLK_LEFT:
            tankX -= 5;
            break;
        case SDLK_RIGHT:
            tankX += 5;
            break;
        case SDLK_ESCAPE:
            running = false;
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Tank Movement Example",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* tankSurface = SDL_LoadBMP("tank.bmp");
    SDL_Texture* tankTexture = SDL_CreateTextureFromSurface(renderer, tankSurface);
    SDL_FreeSurface(tankSurface);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handleInput(event, running);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect tankRect = { tankX, tankY, TANK_WIDTH, TANK_HEIGHT };
        SDL_RenderCopy(renderer, tankTexture, nullptr, &tankRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(tankTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

