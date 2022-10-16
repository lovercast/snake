#pragma once
#include <assert.h>
#include <stdbool.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <sys/time.h>

#define SCREEN_WIDTH    640  /* pixels */
#define SCREEN_HEIGHT   480  /* pixels */
#define GRID_WIDTH       64  /*  cells */
#define GRID_HEIGHT      40  /*  cells */
#define GRID_SIZE      2560  /* cells^2 */
#define CELL_WIDTH       10  /* pixels */
#define CELL_HEIGHT      12  /* pixels */
#define INTERVAL        100  /*     ms */
#define TURBO            25  /*     ms */
#define HINTERVAL        80  /*     ms */
#define VINTERVAL        80  /*     ms */
#define MAX_TEXTURES 8
#define DEAD_LOOPS       25

typedef int board_t[GRID_HEIGHT][GRID_WIDTH];

enum Dir {
    DIR_RIGHT,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
};

typedef struct pos {
    int x; int y;
} pos_t;

typedef struct queue {
    int head;
    int tail;
    int len;
    pos_t body[GRID_SIZE];
} queue_t;

enum gameState {
    ST_TITLE,
    ST_PAUSE,
    ST_GAME,
    ST_DEAD,
    ST_SCORE,
};

enum TEXTURE_TYPES {
    TX_SNAKE = 0,
    TX_APPLE = 1,
    TX_TITLE = 2,
    TX_START = 3,
    TX_HINTS = 4,
};

struct texture {
    SDL_Texture *t;
    int w;
    int h;
};

struct game {
    TTF_Font *font;
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct texture textures[MAX_TEXTURES];
    enum gameState state;
    queue_t q;
    board_t b;
    enum Dir dir;
    int score;
    int interval;
};

static inline void
queue_push(queue_t *q, pos_t p)
{
    q->head = (q->head < q->len-1) ? q->head+1 : 0;
    q->body[q->head] = p;
}

static inline pos_t
queue_pop(queue_t *q)
{
    pos_t take = q->body[q->tail];
    q->tail = (q->tail < q->len-1) ? q->tail+1 : 0;
    return take;
}

static inline bool
queue_iter(queue_t *q, int *ptr, pos_t *out)
{
    if (q->head >= q->tail) {
        if (*ptr > q->head || *ptr < q->tail)
            return false;
    } else if (q->head < q->tail) {
        if (*ptr < q->tail && *ptr > q->head)
            return false;
    }
    *out = q->body[*ptr];
    *ptr = (*ptr < q->len-1) ? *ptr+1 : 0;
    return true;
}

static inline pos_t
queue_peek(queue_t *q)
{
    assert(q->head >= 0);
    return q->body[q->head];
}
