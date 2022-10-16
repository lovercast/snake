#include "emsnake.h"

#include <assert.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <sys/time.h>

struct game game;

void loopHandler(void);

int initSDL(void);
int initTextures(void);
int initMedia(void);
int initGame(void);

void renderTexture(struct texture *t, int x, int y);
void displayTitle(void);
void displayClear(void);
void process_input(void);
void nextFrame(void);
void displayGame(void);

void gameState(void);
void pauseState(void);
void scoreState(void);
void titleState(void);
void deadState(void);

extern int display_score(int);

void
loopHandler()
{
    switch(game.state) {
        case ST_TITLE:
            titleState();
            break;
        case ST_PAUSE:
            pauseState();
            break;
        case ST_GAME:
            gameState();
            break;
        case ST_DEAD:
            deadState();
            break;
        case ST_SCORE:
            scoreState();
            break;
    }
}

void
deadState()
{
    static int count = 0;
    SDL_FlushEvent(SDL_KEYDOWN);
    display_score(game.score);
    if (count++ == DEAD_LOOPS) {
        game.state = ST_SCORE;
        count = 0;
    }
}

int
main()
{
    if (initSDL() < 0) {
        exit(EXIT_FAILURE);
    }
    if (initMedia() < 0) {
        exit(EXIT_FAILURE);
    }
    if (initTextures() < 0) {
        exit(EXIT_FAILURE);
    }
    if (initGame() < 0) {
        exit(EXIT_FAILURE);
    }
    emscripten_set_main_loop(loopHandler, -1, 1);
    return 0;
}

int
initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr,  "%s [%d]: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return -1;
	}
    game.window = SDL_CreateWindow("Snake",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT+5, SDL_WINDOW_SHOWN );
    if (game.window == NULL) {
        fprintf(stderr, "%s [%d]: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return -1;
    }
    game.renderer = SDL_CreateRenderer(game.window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (game.renderer == NULL) {
        fprintf(stderr, "%s [%d]: %s\n",
                __FILE__, __LINE__, SDL_GetError());
        return -1;
    }
    SDL_SetRenderDrawColor(game.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    if(TTF_Init() == -1) {
        fprintf(stderr, "%s [%d]: %s\n",
                __FILE__, __LINE__, TTF_GetError());
        return -1;
    }
    SDL_StartTextInput();
	return 0;
}

int
initMedia()
{
    TTF_Font *f = TTF_OpenFont( "assets/SourceCodePro-Regular.ttf", 14 );
	if(f == NULL) {
		fprintf(stderr, "%s [%d]: %s\n", __FILE__, __LINE__,
                TTF_GetError());
        return -1;
	}
    game.font = f;
    return 0;
}

int
initTextures()
{
    const char *strings[] = {
        "#", "@", "SNAKE",
        "Press Any Button to Start",
        "Arrow keys to move    P to PAUSE    T for TURBO    Q to QUIT",
    };
    SDL_Color textColor = { 0xff, 0xff, 0xff };
    for (int i = 0; i < sizeof strings / sizeof strings[0]; ++i) {

        SDL_Surface *textSurface = TTF_RenderText_Blended(game.font,
                strings[i], textColor);

        if(textSurface == NULL) {
            fprintf(stderr, "%s [%d]: %s\n", __FILE__, __LINE__,
                    TTF_GetError());
            return -1;
        }

        SDL_Texture *t = SDL_CreateTextureFromSurface(
                game.renderer,
                textSurface);

        if(t == NULL) {
            fprintf(stderr,
                    "%s [%d]: %s\n", __FILE__, __LINE__,
                    SDL_GetError());
            return -1;
        }
        game.textures[i].t = t;
        game.textures[i].w = textSurface->w;
        game.textures[i].h = textSurface->h;
        SDL_FreeSurface(textSurface);
    }
    return 0;
}

int
initGame()
{
    memset(&game.b, 0, sizeof(board_t));
    memset(&game.q, 0, sizeof(queue_t));
    struct timeval t;
    if (gettimeofday(&t, NULL) < 0)
        return -1;
    srandom(t.tv_usec);
    game.state = ST_TITLE;
    return 0;
}

void
renderTexture(struct texture *t, int x, int y)
{
    assert(t);
	SDL_Rect rct = { x, y, t->w, t->h };
	SDL_RenderCopy( game.renderer, t->t, NULL, &rct);
}

void
displayTitle()
{
    SDL_SetRenderDrawColor(game.renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(game.renderer);

    struct texture *title = &game.textures[TX_TITLE];
    int titlex = SCREEN_WIDTH / 2 - title->w / 2;
    int titley = SCREEN_HEIGHT / 2 - title->h / 2;
    renderTexture( title, titlex, titley);

    struct texture *hints = &game.textures[TX_HINTS];
    int hintsx = SCREEN_WIDTH / 2 - hints->w / 2;
    int hintsy = SCREEN_HEIGHT - hints->h;
    renderTexture( hints, hintsx, hintsy);

    struct texture *start = &game.textures[TX_START];
    int startx = SCREEN_WIDTH / 2 - start->w / 2;
    int starty = SCREEN_HEIGHT - start->h * 2;
    renderTexture( start, startx, starty);

    SDL_RenderPresent(game.renderer);
}

void
putApple()
{
    int x, y;
    int x0, y0;
    int flag = 0;
    if (game.score == GRID_WIDTH * GRID_HEIGHT)
        return;
    x = x0 = random() % GRID_WIDTH;
    y = y0 = random() % GRID_HEIGHT;
    /*
     * If valid spot not found,
     * linear search until found or give up
     */
    for (;; ++y) {
        if (y == GRID_HEIGHT)
            y = 0;
        for (;; ++x) {
            if (x == x0 && y == y0 && flag) {
                return;
            }
            if (x == GRID_WIDTH) {
                x = 0;
                break;
            }
            if (game.b[y][x] == 0) {
                game.b[y][x] = 2;
                return;
            }
            flag = 1;
        }
    }
}

bool
is_dead(pos_t p, board_t b)
{
    return b[p.y][p.x] == 1 ? true : false;
}

bool
eat_apple(pos_t p, board_t b)
{
    if (b[p.y][p.x] == 2) {
        return true;
    }
    return false;
}

void
nextFrame()
{
    pos_t head = queue_peek(&game.q);
    if (is_dead(head, game.b)) {
        display_score(game.score);
        game.state = ST_DEAD;
        return;
    }
    if (!eat_apple(head, game.b)) {
        game.b[head.y][head.x] = 1;
        pos_t tail = queue_pop(&game.q);
        game.b[tail.y][tail.x] = 0;
    } else {
        game.score++;
        game.b[head.y][head.x] = 1;
        putApple();
    }
    switch(game.dir) {
    case DIR_RIGHT:
        head.x = (head.x < GRID_WIDTH-1 ? head.x + 1 : 0);
        break;
    case DIR_UP:
        head.y = (head.y > 0 ? head.y - 1 : GRID_HEIGHT - 1);
        break;
    case DIR_LEFT:
        head.x = (head.x > 0 ? head.x - 1 : GRID_WIDTH - 1);
        break;
    case DIR_DOWN:
        head.y = (head.y < GRID_HEIGHT - 1 ? head.y + 1 : 0);
        break;
    }
    queue_push(&game.q, head);
}

void
newGame()
{
    game.score = 1;
    game.interval = INTERVAL;
    memset(game.b, 0, sizeof(board_t));
    game.dir = DIR_RIGHT;
    game.q.head = -1;
    game.q.tail = 0;
    game.q.len = GRID_SIZE;
    pos_t start = { GRID_WIDTH / 2, GRID_HEIGHT / 2 };
    putApple();
    putApple();
    putApple();
    putApple();
    putApple();
    putApple();
    putApple();
    putApple();
    putApple();
    queue_push(&game.q, start);
}

void
titleState()
{
    displayTitle();
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_KEYDOWN) {
            game.state = ST_GAME;
            newGame();
            return;
        }
    }
}

void
drawSnakeCell(int cellX, int cellY)
{
    renderTexture(&game.textures[TX_SNAKE],
            CELL_WIDTH * cellX, CELL_HEIGHT * cellY);
}

void
drawAppleCell(int cellX, int cellY)
{
    renderTexture(&game.textures[TX_APPLE],
            CELL_WIDTH * cellX, CELL_HEIGHT * cellY);
}

void
displayGame()
{
    SDL_SetRenderDrawColor(game.renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(game.renderer);
    nextFrame();
    if (game.state != ST_GAME)
        return;
    pos_t head = queue_peek(&game.q);
    drawSnakeCell( head.x, head.y);
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (game.b[y][x] == 1) {
                drawSnakeCell( x, y);
            } else if (game.b[y][x] == 2) {
                drawAppleCell( x, y);
            }
        }
    }
    SDL_RenderPresent(game.renderer);
}

void
gameState()
{
    SDL_Event e;
    bool flag = false;
    while (SDL_PollEvent(&e) != 0) {
        flag = true;
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDL_SCANCODE_Q:
                    game.state = ST_TITLE;
                    return;
                case SDL_SCANCODE_P:
                    game.state = ST_PAUSE;
                    return;
                case SDL_SCANCODE_T:
                    game.interval = (game.interval == TURBO
                            ? INTERVAL
                            : TURBO);
                    return;
                case SDL_SCANCODE_UP:
                    if (game.dir != DIR_DOWN)
                        game.dir = DIR_UP;
                    break;
                case SDL_SCANCODE_DOWN:
                    if (game.dir != DIR_UP)
                        game.dir = DIR_DOWN;
                    break;
                case SDL_SCANCODE_LEFT:
                    if (game.dir != DIR_RIGHT)
                        game.dir = DIR_LEFT;
                    break;
                case SDL_SCANCODE_RIGHT:
                    if (game.dir != DIR_LEFT)
                        game.dir = DIR_RIGHT;
                    break;
                default:
                    break;
            }
        }
        displayGame();
        if (game.state != ST_GAME)
            /* What if we died in the middle of an update? */
            return;
    }
    if (!flag) {
        displayGame();
        if (game.state != ST_GAME)
            return;
    }
    SDL_Delay(game.interval);
}

void
pauseState()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDL_SCANCODE_Q:
                    game.state = ST_TITLE;
                    return;
                case SDL_SCANCODE_P:
                    game.state = ST_GAME;
                    return;
            }
        }
    }
}

void
scoreState()
{
    SDL_Event e;
    display_score(game.score);
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_KEYDOWN) {
            game.state = ST_TITLE;
            return;
        }
    }
}
