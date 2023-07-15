#include <SDL.h>
#include <vector>

// Задаем размеры окна и танка
int tankDirection = 0; // 0 - up, 1 - right, 2 - down, 3 - left
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int TANK_WIDTH = 32;
const int TANK_HEIGHT = 32;
const int PROJECTILE_WIDTH = 64;
const int PROJECTILE_HEIGHT = 64;

struct Projectile {
    int x;
    int y;
    int direction; // 0 - up, 1 - right, 2 - down, 3 - left
    int speed;
};
std::vector<Projectile> projectiles;

// Задаем начальные координаты танка
int tankX = WINDOW_WIDTH / 2 - TANK_WIDTH / 2;
int tankY = WINDOW_HEIGHT / 2 - TANK_HEIGHT / 2;

// Функция для обработки ввода с клавиатуры
void handleInput(SDL_Event& event, bool& running) {
    if (event.type == SDL_QUIT) {
        running = false;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            // Создаем новый снаряд
            Projectile projectile;
            projectile.direction = tankDirection;
            projectile.speed = 1;
            switch (tankDirection) {
            case 0:
                projectile.x = tankX + TANK_WIDTH / 2;
                projectile.y = tankY;
                break;
            case 1:
                projectile.x = tankX + TANK_WIDTH;
                projectile.y = tankY + TANK_HEIGHT / 2;
                break;
            case 2:
                projectile.x = tankX + TANK_WIDTH / 2;
                projectile.y = tankY + TANK_HEIGHT;
                break;
            case 3:
                projectile.x = tankX;
                projectile.y = tankY + TANK_HEIGHT / 2;
                break;
            }
            projectiles.push_back(projectile);
            break;
        case SDLK_UP:
            tankY -= 5;
            tankDirection = 0;
            break;
        case SDLK_DOWN:
            tankY += 5;
            tankDirection = 2;
            break;
        case SDLK_LEFT:
            tankX -= 5;
            tankDirection = 3;
            break;
        case SDLK_RIGHT:
            tankX += 5;
            tankDirection = 1;
            break;
        case SDLK_ESCAPE:
            running = false;
            break;
        }
    }
}


int main(int argc, char* argv[]) {
    // Инициализация SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Создание окна
    SDL_Window* window = SDL_CreateWindow("Tank 1990",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    // Создание рендерера
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Загрузка изображений танка
    SDL_Surface* tankSurfaceLeft = SDL_LoadBMP("tank_left.bmp");
    SDL_Texture* tankTextureLeft = SDL_CreateTextureFromSurface(renderer, tankSurfaceLeft);
    SDL_FreeSurface(tankSurfaceLeft);

    SDL_Surface* tankSurfaceRight = SDL_LoadBMP("tank_right.bmp");
    SDL_Texture* tankTextureRight = SDL_CreateTextureFromSurface(renderer, tankSurfaceRight);
    SDL_FreeSurface(tankSurfaceRight);

    SDL_Surface* tankSurfaceUp = SDL_LoadBMP("tank_up.bmp");
    SDL_Texture* tankTextureUp = SDL_CreateTextureFromSurface(renderer, tankSurfaceUp);
    SDL_FreeSurface(tankSurfaceUp);

    SDL_Surface* tankSurfaceDown = SDL_LoadBMP("tank_down.bmp");
    SDL_Texture* tankTextureDown = SDL_CreateTextureFromSurface(renderer, tankSurfaceDown);
    SDL_FreeSurface(tankSurfaceDown);

    // Загрузка изображения снаряда
    SDL_Surface* projectileSurface = SDL_LoadBMP("projectile.bmp");
    SDL_Texture* projectileTexture = SDL_CreateTextureFromSurface(renderer, projectileSurface);
    SDL_FreeSurface(projectileSurface);


    // Основной цикл игры
    bool running = true;
    while (running) {
        // Обработка ввода с клавиатуры
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handleInput(event, running);
        }

        for (Projectile& projectile : projectiles) {
            switch (projectile.direction) {
            case 0:
                projectile.y -= projectile.speed;
                break;
            case 1:
                projectile.x += projectile.speed;
                break;
            case 2:
                projectile.y += projectile.speed;
                break;
            case 3:
                projectile.x -= projectile.speed;
                break;
            }
        }

        // Очистка экрана
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Отрисовка снарядов
        for (Projectile& projectile : projectiles) {
            SDL_Rect projectileRect = { projectile.x, projectile.y, PROJECTILE_WIDTH, PROJECTILE_HEIGHT };
            SDL_RenderCopy(renderer, projectileTexture, nullptr, &projectileRect);
        }


        // Отрисовка танка
        SDL_Rect tankRect = { tankX, tankY, TANK_WIDTH, TANK_HEIGHT };
        switch (tankDirection) {
        case 0:
            SDL_RenderCopy(renderer, tankTextureUp, nullptr, &tankRect);
            break;
        case 1:
            SDL_RenderCopy(renderer, tankTextureRight, nullptr, &tankRect);
            break;
        case 2:
            SDL_RenderCopy(renderer, tankTextureDown, nullptr, &tankRect);
            break;
        case 3:
            SDL_RenderCopy(renderer, tankTextureLeft, nullptr, &tankRect);
            break;
        }


        // Обновление экрана
        SDL_RenderPresent(renderer);
    }

    // Освобождение ресурсов и выход
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
