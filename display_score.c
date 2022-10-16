#include <SDL.h>
#include "emsnake.h"

extern struct game game;

char nums_zero[] = { /* 10 */
    0,0,1,1,1,1,1,1,0,0,
    0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,1,1,1,1,1,0,
    0,0,1,1,1,1,1,1,0,0,
};

char nums_one[] = { /* 8 */
    0,0,0,1,1,1,0,0,
    0,0,1,1,1,1,0,0,
    0,1,1,1,1,1,0,0,
    0,0,0,1,1,1,0,0,
    0,0,0,1,1,1,0,0,
    0,0,0,1,1,1,0,0,
    0,0,0,1,1,1,0,0,
    0,0,0,1,1,1,0,0,
};

char nums_two[] = { /* 10 */
    0,0,1,1,1,1,1,1,0,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,0,0,0,0,1,1,1,1,0,
    0,0,0,0,1,1,1,1,0,0,
    0,0,1,1,1,1,0,0,0,0,
    0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,0,
};

char nums_three[] = { /* 9 */
    0,0,1,1,1,1,1,0,0,
    0,1,1,1,1,1,1,1,0,
    0,1,1,0,0,1,1,1,0,
    0,0,0,0,1,1,1,0,0,
    0,0,0,0,0,1,1,1,0,
    0,1,1,0,0,1,1,1,0,
    0,1,1,1,1,1,1,1,0,
    0,0,1,1,1,1,1,0,0,
};


char nums_four[] = { /* 11 */
    0,0,0,0,0,1,1,1,0,0,0,
    0,0,0,0,1,1,1,1,0,0,0,
    0,0,0,1,0,1,1,1,0,0,0,
    0,1,1,0,0,1,1,1,0,0,0,
    0,1,1,1,1,1,1,1,1,1,0,
    0,0,0,0,0,1,1,1,0,0,0,
    0,0,0,0,0,1,1,1,0,0,0,
    0,0,0,0,0,1,1,1,0,0,0,
};

char nums_five[] = { /* 9 */
    0,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,0,
    0,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,0,
    0,0,0,0,0,1,1,1,0,
    0,1,1,0,0,0,1,1,0,
    0,1,1,1,0,1,1,1,0,
    0,0,1,1,1,1,1,0,0,
};

char nums_six[] = { /* 10 */
    0,0,1,1,1,1,1,1,0,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,0,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,0,1,1,1,1,1,1,0,0,
};

char nums_seven[] = {
    0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,0,
    0,0,0,0,0,1,1,1,0,0,
    0,0,0,0,0,1,1,1,0,0,
    0,0,0,0,1,1,1,0,0,0,
    0,0,0,0,1,1,1,0,0,0,
    0,0,0,1,1,1,0,0,0,0,
    0,0,0,1,1,1,0,0,0,0,
};

char nums_eight[] = {
    0,0,1,1,1,1,1,1,0,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,0,1,1,1,1,1,1,0,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,0,1,1,1,1,1,1,0,0,
};

char nums_nine[] = { /* 10 */
    0,0,1,1,1,1,1,1,0,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,0,1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,1,1,1,0,
    0,1,1,1,0,0,1,1,1,0,
    0,0,1,1,1,1,1,1,0,0,
};

int display_score(int score)
{
    int dig[5];
    char *dig_strings[5];
    int pow = 10;
    int widths[] =  { 10,8,10,9,11,9,10,10,10,10, };

    char *nums[] = {
        nums_zero, nums_one, nums_two,
        nums_three, nums_four, nums_five,
        nums_six, nums_seven, nums_eight, nums_nine
    };

    /* How many digits in the score? */
    while (pow * 10 < score)  {
        pow *= 10;
    }

    int i, width = 0;
    for (i = 0; pow > 0; ++i, pow /= 10) {
        dig[i] = (score % (pow * 10)) / pow;
        dig_strings[i] = nums[dig[i]];
        width += widths[dig[i]];
    }
    int len = i;
    int height = 8;
    int startx = (SCREEN_WIDTH / 2 - width * CELL_WIDTH / 2);
    int starty = (SCREEN_HEIGHT / 2 - height * CELL_HEIGHT / 2) ;

    SDL_Rect rct = {
        .x = startx,
        .y = starty,
        .w = CELL_WIDTH,
        .h = CELL_HEIGHT
    };

    SDL_RenderClear( game.renderer );
    for (int h = 0; h < 8; ++h) {
        rct.x = startx;
        rct.y = starty + h * CELL_HEIGHT;
        for (int i = 0; i < len; ++i) {
            int d = dig[i];
            int w = widths [d];
            for (int j = 0; j < w; ++j) {
                rct.x += CELL_WIDTH;
                if (nums[d][h * w + j]) {
                    SDL_SetRenderDrawColor(game.renderer,
                            0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderFillRect(game.renderer, &rct);
                    SDL_SetRenderDrawColor(game.renderer,
                            0x00, 0x00, 0x00, 0x00);
                } else {
                    SDL_RenderFillRect(game.renderer, &rct);
                }
            }
        }
    }
    SDL_RenderPresent( game.renderer );
    return 1;
}
