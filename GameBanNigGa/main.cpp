#include "defs.h"

using namespace std;


struct Enemy {
    SDL_Texture* texture;
    SDL_Rect rect;
};
struct Bom {
    SDL_Texture* texture;
    SDL_Rect rect;
};

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "Lỗi load ảnh: " << IMG_GetError() << "\n";
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

bool isInside(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

bool runMenu(SDL_Renderer* renderer) {
    SDL_Event e;
    bool quitMenu = false;
    bool startGame = false;

    SDL_Texture* menuBackground = loadTexture("image\\Menu.png", renderer);
    SDL_Rect playButton = {SCREEN_WIDTH/2 - 300, 150, 500, 100};
    SDL_Rect exitButton = {SCREEN_WIDTH/2 - 300, 350, 500, 100};

    while (!quitMenu) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quitMenu = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                if (isInside(mouseX, mouseY, playButton)) {
                    startGame = true;
                    quitMenu = true;
                }
                if (isInside(mouseX, mouseY, exitButton)) {
                    startGame = false;
                    quitMenu = true;
                }
            }
        }

        if (menuBackground) {
            SDL_RenderCopy(renderer, menuBackground, nullptr, nullptr);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(menuBackground);
    return startGame;
}

void runGame(SDL_Renderer* renderer) {
    SDL_Texture* enemyTex = loadTexture("image\\ga.png", renderer);
    SDL_Texture* gameBackground = loadTexture("image\\background.png", renderer);
    SDL_Texture* bomTex = loadTexture("image\\bom.png", renderer);
    if (!enemyTex ) {
        return;
    }

    vector<Enemy> enemies;
    vector<Bom> bomss;


    for (int i = 0; i < numberEnemy; ++i) {
        int x = rand() % (SCREEN_WIDTH - 64);
        Enemy emy = {enemyTex, {x, -rand() % 300, 64, 64}};
        enemies.push_back(emy);
    }
    for (int i = 0; i < numberBom; ++i) {
        int x = rand() % (SCREEN_WIDTH - 64);
        Bom b = {bomTex, {x, -rand() % 300, 64, 64}};
        bomss.push_back(b);
    }
    Uint32 lastSpawnTime = 1000;
    bool quitGame = false;
    SDL_Event e;
    int score = 0;

    while (!quitGame) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quitGame = true;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                for (int i = enemies.size() - 1; i >= 0; --i) {
                    SDL_Rect r = enemies[i].rect;
                    if (isInside(mouseX, mouseY, r)) {
                        enemies.erase(enemies.begin() + i);
                        score++;
                        cout << "Score: " << score << "\n";
                        break;

                }
            }
               for (int i = bomss.size() - 1; i >= 0; --i) {
                    SDL_Rect k = bomss[i].rect;
                    if (isInside(mouseX, mouseY, k)) {

                        cout << "Score: " << score << "\n";
                        quitGame = true;
                        break;

                }
            }
        }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastSpawnTime >= spawnIntervalEnemy) {
            int x = rand() % (SCREEN_WIDTH - 64);
            Enemy emy = {enemyTex, {x, -64, 64, 64}};
            enemies.push_back(emy);
            lastSpawnTime = currentTime;
        }
        if (currentTime - lastSpawnTime >= spawnIntervalBom) {
            int x = rand() % (SCREEN_WIDTH - 64);
            Bom b = {bomTex, {x, -64, 64, 64}};
            bomss.push_back(b);
            lastSpawnTime = currentTime;
        }


        for (auto& enemy : enemies) {
            enemy.rect.y += enemySpeed;
            /*if (enemy.rect.y > SCREEN_HEIGHT) {
                enemy.rect.y = -64;
                enemy.rect.x = rand() % (SCREEN_WIDTH - 64);
            }
            */
            if (enemy.rect.y + enemy.rect.h >= SCREEN_HEIGHT) {
                quitGame = true;
                break;
            }
        }

        for (auto& boman : bomss) {
            boman.rect.y += bomSpeed;
           if (boman.rect.y > SCREEN_HEIGHT) {
                boman.rect.y = -64;
                boman.rect.x = rand() % (SCREEN_WIDTH - 64);
           }
        }

        // Vẽ game
        if (gameBackground) {
            SDL_RenderCopy(renderer, gameBackground, nullptr, nullptr);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        for (auto& enemy : enemies) {
            SDL_RenderCopy(renderer, enemy.texture, nullptr, &enemy.rect);
        }
        for (auto& boman : bomss) {
            SDL_RenderCopy(renderer, boman.texture, nullptr, &boman.rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    SDL_DestroyTexture(enemyTex);
    SDL_DestroyTexture(bomTex);

    SDL_DestroyTexture(gameBackground);
    cout << "Gà!\n";
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Lỗi SDL: " << SDL_GetError() << "\n";
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Lỗi SDL_image: " << IMG_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    srand(static_cast<unsigned int>(time(nullptr)));

    SDL_Window* window = SDL_CreateWindow("Game Ban NigGa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        cout << "Lỗi tạo cửa sổ hoặc renderer: " << SDL_GetError() << "\n";
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    bool play = runMenu(renderer);
    if (play) {
        runGame(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
