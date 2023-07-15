#include <SDL.h>
#include <vector>
#include <fstream>
#include <string>

int tankDirection = 0; // 0 - up, 1 - right, 2 - down, 3 - left
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const int TANK_WIDTH = 64;
const int TANK_HEIGHT = 64;
const int PROJECTILE_WIDTH = 64;
const int PROJECTILE_HEIGHT = 64;
bool tankMoving = false;
float tankSpeed = 0.2f;

struct Brick {
    float x;
    float y;
    int width;
    int height;
};


struct Projectile {
    float x;
    float y;
    int direction; // 0 - up, 1 - right, 2 - down, 3 - left
    float speed;
};

std::vector<Brick> bricks;
std::vector<Projectile> projectiles;

// Задаем начальные координаты танка
float tankX = 0;
float tankY = 0;

// Функция для обработки ввода с клавиатуры
void handleInput(SDL_Event& event, bool& running) {
    if (event.type == SDL_QUIT) {
        running = false;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            if (projectiles.empty()) {
                // Создаем новый снаряд
                Projectile projectile;
                projectile.direction = tankDirection;
                projectile.speed = 0.3f;
                switch (tankDirection) {
                case 0:
                    projectile.x = tankX + TANK_WIDTH / 2 - PROJECTILE_WIDTH / 2;
                    projectile.y = tankY - PROJECTILE_HEIGHT;
                    break;
                case 1:
                    projectile.x = tankX + TANK_WIDTH;
                    projectile.y = tankY + TANK_HEIGHT / 2 - PROJECTILE_HEIGHT / 2;
                    break;
                case 2:
                    projectile.x = tankX + TANK_WIDTH / 2 - PROJECTILE_WIDTH / 2;
                    projectile.y = tankY + TANK_HEIGHT;
                    break;
                case 3:
                    projectile.x = tankX;
                    projectile.y = tankY + TANK_HEIGHT / 2 - PROJECTILE_HEIGHT / 2;
                    break;
                }
                projectiles.push_back(projectile);
                break;
        case SDLK_UP:
            tankDirection = 0;
            tankMoving = true;
            break;
        case SDLK_DOWN:
            tankDirection = 2;
            tankMoving = true;
            break;
        case SDLK_LEFT:
            tankDirection = 3;
            tankMoving = true;
            break;
        case SDLK_RIGHT:
            tankDirection = 1;
            tankMoving = true;
            break;
            }
        }
    }
    else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
            if (tankDirection == 0) {
                tankMoving = false;
            }
            break;
        case SDLK_DOWN:
            if (tankDirection == 2) {
                tankMoving = false;
            }
            break;
        case SDLK_LEFT:
            if (tankDirection == 3) {
                tankMoving = false;
            }
            break;
        case SDLK_RIGHT:
            if (tankDirection == 1) {
                tankMoving = false;
            }
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
        SDL_WINDOW_FULLSCREEN_DESKTOP);


    // Создание рендерера
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Закгрузка изображения танка
    SDL_Surface* brickSurface = SDL_LoadBMP("brick.bmp");
    SDL_Texture* brickTexture = SDL_CreateTextureFromSurface(renderer, brickSurface);
    SDL_FreeSurface(brickSurface);


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
    SDL_Surface* projectileSurfaceUp = SDL_LoadBMP("projectile_up.bmp");
    SDL_Texture* projectileTextureUp = SDL_CreateTextureFromSurface(renderer, projectileSurfaceUp);
    SDL_FreeSurface(projectileSurfaceUp);

    SDL_Surface* projectileSurfaceDown = SDL_LoadBMP("projectile_down.bmp");
    SDL_Texture* projectileTextureDown = SDL_CreateTextureFromSurface(renderer, projectileSurfaceDown);
    SDL_FreeSurface(projectileSurfaceDown);

    SDL_Surface* projectileSurfaceLeft = SDL_LoadBMP("projectile_left.bmp");
    SDL_Texture* projectileTextureLeft = SDL_CreateTextureFromSurface(renderer, projectileSurfaceLeft);
    SDL_FreeSurface(projectileSurfaceLeft);

    SDL_Surface* projectileSurfaceRight = SDL_LoadBMP("projectile_right.bmp");
    SDL_Texture* projectileTextureRight = SDL_CreateTextureFromSurface(renderer, projectileSurfaceRight);
    SDL_FreeSurface(projectileSurfaceRight);

    // Загрузка информации об уровне из файла
    std::ifstream levelFile("level.txt");
    std::vector<std::string> levelData;
    std::string line;
    while (std::getline(levelFile, line)) {
        levelData.push_back(line);
    }
    levelFile.close();

    // Создание кирпичей
    for (int y = 0; y < levelData.size(); y++) {
        for (int x = 0; x < levelData[y].size(); x++) {
            if (levelData[y][x] == '1') {
                Brick brick;
                brick.x = x * 64;
                brick.y = y * 64;
                brick.width = 64;
                brick.height = 64;
                bricks.push_back(brick);
            }
        }
    }


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

        // Обновление координат танка
        if (tankMoving) {
            switch (tankDirection) {
            case 0:
                tankY -= tankSpeed;
                break;
            case 1:
                tankX += tankSpeed;
                break;
            case 2:
                tankY += tankSpeed;
                break;
            case 3:
                tankX -= tankSpeed;
                break;
            }
        }


        // Обработка столкновений танка с кирпичами
        for (Brick& brick : bricks) {
            if (tankX + TANK_WIDTH > brick.x && tankX < brick.x + brick.width &&
                tankY + TANK_HEIGHT > brick.y && tankY < brick.y + brick.height) {
                // Танк столкнулся с кирпичом
                switch (tankDirection) {
                case 0: // Танк направлен вверх
                    tankY = brick.y + brick.height;
                    break;
                case 1: // Танк направлен вправо
                    tankX = brick.x - TANK_WIDTH;
                    break;
                case 2: // Танк направлен вниз
                    tankY = brick.y - TANK_HEIGHT;
                    break;
                case 3: // Танк направлен влево
                    tankX = brick.x + brick.width;
                    break;
                }
            }
        }


        // Очистка экрана
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Отрисовка кирпичей
        for (Brick& brick : bricks) {
            SDL_Rect brickRect = { brick.x, brick.y, brick.width, brick.height };
            SDL_RenderCopy(renderer, brickTexture, nullptr, &brickRect);
        }

        // Отрисовка снарядов
        for (Projectile& projectile : projectiles) {
            SDL_Rect projectileRect = { projectile.x, projectile.y, PROJECTILE_WIDTH, PROJECTILE_HEIGHT };
            switch (projectile.direction) {
            case 0:
                SDL_RenderCopy(renderer, projectileTextureUp, nullptr, &projectileRect);
                break;
            case 1:
                SDL_RenderCopy(renderer, projectileTextureRight, nullptr, &projectileRect);
                break;
            case 2:
                SDL_RenderCopy(renderer, projectileTextureDown, nullptr, &projectileRect);
                break;
            case 3:
                SDL_RenderCopy(renderer, projectileTextureLeft, nullptr, &projectileRect);
                break;
            }
        }

        // Обработка столкновений снарядов с кирпичами
        for (auto it = projectiles.begin(); it != projectiles.end();) {
            Projectile& projectile = *it;
            bool hitBrick = false;
            for (auto it2 = bricks.begin(); it2 != bricks.end();) {
                Brick& brick = *it2;
                if (projectile.x + PROJECTILE_WIDTH > brick.x && projectile.x < brick.x + brick.width &&
                    projectile.y + PROJECTILE_HEIGHT > brick.y && projectile.y < brick.y + brick.height) {
                    // Снаряд попал в кирпич
                    hitBrick = true;
                    it2 = bricks.erase(it2); // Удаляем кирпич
                    break; // Прерываем цикл после первого попадания
                }
                else {
                    ++it2;
                }
            }
            if (hitBrick) {
                it = projectiles.erase(it); // Удаляем снаряд
            }
            else {
                ++it;
            }
        }

        // Удаление снарядов, которые вылетели за границы окна
        for (auto it = projectiles.begin(); it != projectiles.end();) {
            Projectile& projectile = *it;
            if (projectile.x < 0 || projectile.x > WINDOW_WIDTH ||
                projectile.y < 0 || projectile.y > WINDOW_HEIGHT) {
                it = projectiles.erase(it);
            }
            else {
                ++it;
            }
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

