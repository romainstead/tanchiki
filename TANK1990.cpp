#include <SDL.h>
#include <vector>
#include <fstream>
#include <string>

int tankDirection = 0; // 0 - up, 1 - right, 2 - down, 3 - left
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float TANK_WIDTH = 50;
const float TANK_HEIGHT = 50;
const int PROJECTILE_WIDTH = 16;
const int PROJECTILE_HEIGHT = 64;
bool tankMoving = false;
float tankSpeed = 0.1f;

struct Brick {
    float x;
    float y;
    int width;
    int height;
};

struct Enemy {
    float x;
    float y;
    int direction; // 0 - up, 1 - right, 2 - down, 3 - left
    float speed;
    bool canFire; // Может ли враг выстрелить
};


struct Projectile {
    float x;
    float y;
    int direction; // 0 - up, 1 - right, 2 - down, 3 - left
    float speed;
};

struct SteelBlock {
    float x;
    float y;
    int width;
    int height;
};

std::vector<SteelBlock> steelBlocks;
std::vector<Enemy> enemies;
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
            if (projectiles.empty() && SDL_GetTicks() - lastShotTime > 2000) {
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
                    projectile.x = tankX - PROJECTILE_WIDTH;
                    projectile.y = tankY + TANK_HEIGHT / 2 - PROJECTILE_HEIGHT / 2;
                    break;
                }
                projectiles.push_back(projectile);
                lastShotTime = SDL_GetTicks(); // Обновляем время последнего выстрела
            }
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

void updateEnemies() {
    for (Enemy& enemy : enemies) {
        switch (enemy.direction) {
        case 0:
            enemy.y -= enemy.speed;
            break;
        case 1:
            enemy.x += enemy.speed;
            break;
        case 2:
            enemy.y += enemy.speed;
            break;
        case 3:
            enemy.x -= enemy.speed;
            break;
        }
    }
}

bool hasClearLineOfSight(float x1, float y1, float x2, float y2) {
    for (Brick& brick : bricks) {
        // Проверяем пересечение отрезка и прямоугольника
        float brickLeft = brick.x;
        float brickRight = brick.x + brick.width;
        float brickTop = brick.y;
        float brickBottom = brick.y + brick.height;

        // Находим точки пересечения отрезка и сторон прямоугольника
        float intersectionLeft = y1 + (y2 - y1) * (brickLeft - x1) / (x2 - x1);
        float intersectionRight = y1 + (y2 - y1) * (brickRight - x1) / (x2 - x1);
        float intersectionTop = x1 + (x2 - x1) * (brickTop - y1) / (y2 - y1);
        float intersectionBottom = x1 + (x2 - x1) * (brickBottom - y1) / (y2 - y1);

        // Проверяем, есть ли пересечение
        if ((intersectionLeft >= brickTop && intersectionLeft <= brickBottom && brickLeft >= std::min(x1, x2) && brickLeft <= std::max(x1, x2)) ||
            (intersectionRight >= brickTop && intersectionRight <= brickBottom && brickRight >= std::min(x1, x2) && brickRight <= std::max(x1, x2)) ||
            (intersectionTop >= brickLeft && intersectionTop <= brickRight && brickTop >= std::min(y1, y2) && brickTop <= std::max(y1, y2)) ||
            (intersectionBottom >= brickLeft && intersectionBottom <= brickRight && brickBottom >= std::min(y1, y2) && brickBottom <= std::max(y1, y2))) {
            return false; // Есть пересечение
        }
    }

    return true; // Нет пересечения
}

// Функция для обработки стрельбы врагов
void handleEnemyFire() {
    for (Enemy& enemy : enemies) {
        if (enemy.canFire) {
            // Проверяем, находится ли игрок на линии огня
            bool canFire = false;
            switch (enemy.direction) {
            case 0:
                if (tankX >= enemy.x && tankX <= enemy.x + TANK_WIDTH && tankY < enemy.y &&
                    hasClearLineOfSight(enemy.x + TANK_WIDTH / 2, enemy.y, tankX + TANK_WIDTH / 2, tankY + TANK_HEIGHT)) {
                    canFire = true;
                }
                break;
            case 1:
                if (tankY >= enemy.y && tankY <= enemy.y + TANK_HEIGHT && tankX > enemy.x &&
                    hasClearLineOfSight(enemy.x + TANK_WIDTH, enemy.y + TANK_HEIGHT / 2, tankX, tankY + TANK_HEIGHT / 2)) {
                    canFire = true;
                }
                break;
            case 2:
                if (tankX >= enemy.x && tankX <= enemy.x + TANK_WIDTH && tankY > enemy.y &&
                    hasClearLineOfSight(enemy.x + TANK_WIDTH / 2, enemy.y + TANK_HEIGHT, tankX + TANK_WIDTH / 2, tankY)) {
                    canFire = true;
                }
                break;
            case 3:
                if (tankY >= enemy.y && tankY <= enemy.y + TANK_HEIGHT && tankX < enemy.x &&
                    hasClearLineOfSight(enemy.x, enemy.y + TANK_HEIGHT / 2, tankX + TANK_WIDTH, tankY + TANK_HEIGHT / 2)) {
                    canFire = true;
                }
                break;
            }

            if (canFire) {
                // Создаем новый снаряд
                Projectile projectile;
                projectile.direction = enemy.direction;
                projectile.speed = 0.3f;
                switch (enemy.direction) {
                case 0:
                    projectile.x = enemy.x + TANK_WIDTH / 2 - PROJECTILE_WIDTH / 2;
                    projectile.y = enemy.y - PROJECTILE_HEIGHT;
                    break;
                case 1:
                    projectile.x = enemy.x + PROJECTILE_WIDTH;
                    projectile.y = enemy.y + TANK_WIDTH / 2 - PROJECTILE_WIDTH / 2;
                    break;
                case 2:
                    projectile.x = enemy.x + TANK_WIDTH / 2 - PROJECTILE_WIDTH / 2;
                    projectile.y = enemy.y + TANK_HEIGHT;
                    break;
                case 3:
                    projectile.x = enemy.x - TANK_HEIGHT;
                    projectile.y = enemy.y + TANK_WIDTH / 2 - PROJECTILE_WIDTH / 2;
                    break;
                }
                projectiles.push_back(projectile);

                // Запрещаем стрелять до следующего попадания или промаха
                enemy.canFire = false;
            }
        }
    }
}


// Функция для обработки столкновений врагов с кирпичами
void handleEnemyCollisions() {
    for (Enemy& enemy : enemies) {
        for (Brick& brick : bricks) {
            SDL_Rect enemyRect = { static_cast<int>(enemy.x), static_cast<int>(enemy.y), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
            SDL_Rect brickRect = { brick.x, brick.y, brick.width, brick.height };
            if (SDL_HasIntersection(&enemyRect, &brickRect)) {
                // Враг столкнулся с кирпичом
                switch (enemy.direction) {
                case 0: // Враг направлен вверх
                    enemy.y = brick.y + brick.height;
                    enemy.direction = rand() % 4; // Изменяем направление на случайное
                    break;
                case 1: // Враг направлен вправо
                    enemy.x = brick.x - TANK_WIDTH;
                    enemy.direction = rand() % 4; // Изменяем направление на случайное
                    break;
                case 2: // Враг направлен вниз
                    enemy.y = brick.y - TANK_HEIGHT;
                    enemy.direction = rand() % 4; // Изменяем направление на случайное
                    break;
                case 3: // Враг направлен влево
                    enemy.x = brick.x + brick.width;
                    enemy.direction = rand() % 4; // Изменяем направление на случайное
                    break;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Инициализация SDL
    SDL_Init(SDL_INIT_VIDEO);
    const float timeStep = 0.01f; // Фиксированный временной шаг в секундах
    float accumulatedTime = 0; // Накопленное время
    Uint32 previousTime = SDL_GetTicks(); // Время предыдущего кадра


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
            else if (levelData[y][x] == '2') {
                Enemy enemy;
                enemy.x = x * 64;
                enemy.y = y * 64;
                enemy.direction = 0; // Направление вверх
                enemy.speed = 0.1f;
                enemy.canFire = true; // Можно сразу выстрелить
                enemies.push_back(enemy);
            }
            else if (levelData[y][x] == 'P') {
                tankX = x * 64;
                tankY = y * 64;
            }
            else if (levelData[y][x] == '3') {
                            // Создание неразрушаемого стального блока
                            SteelBlock steelBlock;
                            steelBlock.x = x * 64;
                            steelBlock.y = y * 64;
                            steelBlock.width = 64;
                            steelBlock.height = 64;
                            steelBlocks.push_back(steelBlock);
                        }

        }
    }


    // Основной цикл игры
    bool running = true;
    while (running) {
        updateEnemies();
        handleEnemyFire();
        handleEnemyCollisions();
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


        for (Brick& brick : bricks) {
            SDL_Rect tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
            SDL_Rect brickRect = { brick.x, brick.y, brick.width, brick.height };
            if (SDL_HasIntersection(&tankRect, &brickRect)) {
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

        for (SteelBlock& steelBlock : steelBlocks) {
                        SDL_Rect tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
                        SDL_Rect steelBlockRect = { static_cast<int>(steelBlock.x), static_cast<int>(steelBlock.y), steelBlock.width, steelBlock.height };
                        if (SDL_HasIntersection(&tankRect, &steelBlockRect)) {
                            // Танк столкнулся с неразрушаемым стальным блоком
                            switch (tankDirection) {
                            case 0: // Танк направлен вверх
                                tankY = steelBlock.y + steelBlock.height;
                                break;
                            case 1: // Танк направлен вправо
                                tankX = steelBlock.x - TANK_WIDTH;
                                break;
                            case 2: // Танк направлен вниз
                                tankY = steelBlock.y - TANK_HEIGHT;
                                break;
                            case 3: // Танк направлен влево
                                tankX = steelBlock.x + steelBlock.width;
                                break;
                            }
                        }
                    }

        for (Enemy& enemy : enemies) {
                        SDL_Rect tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
                        SDL_Rect enemyRect = { static_cast<int>(enemy.x), static_cast<int>(enemy.y), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
                        if (SDL_HasIntersection(&tankRect, &enemyRect)) {
                            // Танк игрока столкнулся с вражеским танком
                            if (tankX < enemy.x) {
                                tankX = enemy.x - TANK_WIDTH;
                            }
                            else if (tankX > enemy.x) {
                                tankX = enemy.x + TANK_WIDTH;
                            }
                            if (tankY < enemy.y) {
                                tankY = enemy.y - TANK_HEIGHT;
                            }
                            else if (tankY > enemy.y) {
                                tankY = enemy.y + TANK_HEIGHT;
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

        // Отрисовка неразрушаемых стальных блоков
        for (SteelBlock& steelBlock : steelBlocks) {
            SDL_Rect steelBlockRect = { static_cast<int>(steelBlock.x), static_cast<int>(steelBlock.y), steelBlock.width, steelBlock.height };
            SDL_RenderCopy(renderer, steelBlockTexture, nullptr, &steelBlockRect);
        }

        // Отрисовка снарядов
        for (Projectile& projectile : projectiles) {
            SDL_Rect projectileRect;
            switch (projectile.direction) {
            case 0:
                projectileRect = { static_cast<int>(projectile.x), static_cast<int>(projectile.y), 16, 64 };
                SDL_RenderCopy(renderer, projectileTextureUp, nullptr, &projectileRect);
                break;
            case 1:
                projectileRect = { static_cast<int>(projectile.x), static_cast<int>(projectile.y), 64, 16 };
                SDL_RenderCopy(renderer, projectileTextureRight, nullptr, &projectileRect);
                break;
            case 2:
                projectileRect = { static_cast<int>(projectile.x), static_cast<int>(projectile.y), 16, 64 };
                SDL_RenderCopy(renderer, projectileTextureDown, nullptr, &projectileRect);
                break;
            case 3:
                projectileRect = { static_cast<int>(projectile.x), static_cast<int>(projectile.y), 64, 16 };
                SDL_RenderCopy(renderer, projectileTextureLeft, nullptr, &projectileRect);
                break;
            }
        }

        // Обработка столкновений снарядов с неразрушаемыми стальными блоками
                    for (auto it = projectiles.begin(); it != projectiles.end();) {
                        Projectile& projectile = *it;
                        bool hitSteelBlock = false;
                        for (auto it2 = steelBlocks.begin(); it2 != steelBlocks.end(); ++it2) {
                            SteelBlock& steelBlock = *it2;

                            int projectileHidth = PROJECTILE_WIDTH;
                            int projectileWidth = PROJECTILE_HEIGHT;

                            if (projectile.direction == 1 || projectile.direction == 3) {
                                std::swap(projectileHidth, projectileWidth);
                            }

                            if (projectile.x + projectileHidth > steelBlock.x && projectile.x < steelBlock.x + steelBlock.width &&
                                (projectile.y) + projectileWidth > steelBlock.y && (projectile.y) < steelBlock.y + steelBlock.height) {
                                // Снаряд попал в неразрушаемый стальной блок
                                hitSteelBlock = true;
                                break; // Прерываем цикл после первого попадания
                            }
                        }

                        if (hitSteelBlock) {
                            it = projectiles.erase(it); // Удаляем снаряд
                        }
                        else {
                            ++it;
                        }
                    }

        // Обработка столкновений снарядов с кирпичами
        for (auto it = projectiles.begin(); it != projectiles.end();) {
            Projectile& projectile = *it;
            bool hitBrick = false;
            for (auto it2 = bricks.begin(); it2 != bricks.end();) {
                Brick& brick = *it2;
                int projectileWidth = PROJECTILE_WIDTH;
                int projectileHeight = PROJECTILE_HEIGHT;
                if (projectile.direction == 1 || projectile.direction == 3) {
                    std::swap(projectileWidth, projectileHeight);
                }
                if (projectile.x + projectileWidth > brick.x && projectile.x < brick.x + brick.width &&
                    projectile.y + projectileHeight > brick.y && projectile.y < brick.y + brick.height) {
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
        // Удаление снарядов, которые вылетели за границы окна или попали в кирпич
        for (auto it = projectiles.begin(); it != projectiles.end();) {
            Projectile& projectile = *it;

            bool hitBrickOrWall = false;

            // Проверяем столкновение с кирпичами
            for (auto it2 = bricks.begin(); it2 != bricks.end();) {
                Brick& brick = *it2;

                int projectileWidth = PROJECTILE_WIDTH;
                int projectileHeight = PROJECTILE_HEIGHT;

                if (projectile.direction == 1 || projectile.direction == 3) {
                    std::swap(projectileWidth, projectileHeight);
                }

                if (projectile.x + projectileWidth > brick.x && projectile.x < brick.x + brick.width &&
                    projectile.y + projectileHeight > brick.y && projectile.y < brick.y + brick.height) {
                    // Снаряд попал в кирпич
                    hitBrickOrWall = true;
                    it2 = bricks.erase(it2); // Удаляем кирпич
                    break; // Прерываем цикл после первого попадания
                }
                else {
                    ++it2;
                }
            }

            // Проверяем вылет за границы окна
            if (projectile.x < 0 || projectile.x > WINDOW_WIDTH ||
                projectile.y < 0 || projectile.y > WINDOW_HEIGHT) {
                hitBrickOrWall = true;
            }

            if (hitBrickOrWall) {
                it = projectiles.erase(it); // Удаляем снаряд

                // Разрешаем стрелять всем врагам
                for (Enemy& enemy : enemies) {
                    enemy.canFire = true;
                }
            }
            else {
                ++it;
            }
        }


        // Отрисовка танка
        SDL_Rect tankRect;
        switch (tankDirection) {
        case 0:
            tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
            SDL_RenderCopy(renderer, tankTextureUp, nullptr, &tankRect);
            break;
        case 1:
            tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_HEIGHT), static_cast<int>(TANK_WIDTH) };
            SDL_RenderCopy(renderer, tankTextureRight, nullptr, &tankRect);
            break;
        case 2:
            tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
            SDL_RenderCopy(renderer, tankTextureDown, nullptr, &tankRect);
            break;
        case 3:
            tankRect = { static_cast<int>(tankX), static_cast<int>(tankY), static_cast<int>(TANK_HEIGHT), static_cast<int>(TANK_WIDTH) };
            SDL_RenderCopy(renderer, tankTextureLeft, nullptr, &tankRect);
            break;
        }

        // Отрисовка врагов
        for (Enemy& enemy : enemies) {
            SDL_Rect enemyRect;
            switch (enemy.direction) {
            case 0:
                enemyRect = { static_cast<int>(enemy.x), static_cast<int>(enemy.y), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
                SDL_RenderCopy(renderer, tankTextureUp, nullptr, &enemyRect);
                break;
            case 1:
                enemyRect = { static_cast<int>(enemy.x), static_cast<int>(enemy.y), static_cast<int>(TANK_HEIGHT), static_cast<int>(TANK_WIDTH) };
                SDL_RenderCopy(renderer, tankTextureRight, nullptr, &enemyRect);
                break;
            case 2:
                enemyRect = { static_cast<int>(enemy.x), static_cast<int>(enemy.y), static_cast<int>(TANK_WIDTH), static_cast<int>(TANK_HEIGHT) };
                SDL_RenderCopy(renderer, tankTextureDown, nullptr, &enemyRect);
                break;
            case 3:
                enemyRect = { static_cast<int>(enemy.x), static_cast<int>(enemy.y), static_cast<int>(TANK_HEIGHT), static_cast<int>(TANK_WIDTH) };
                SDL_RenderCopy(renderer, tankTextureLeft, nullptr, &enemyRect);
                break;
            }
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

