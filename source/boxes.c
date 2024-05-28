#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "include/draw_tools.h"
#include "include/artmath.h"
#include "include/constants.h"

void selectionSortOrderBuilderLocal(
    float data[], uint8_t order[], uint8_t size)
{
    uint8_t temp;
    int position;
    for (int i = 0; i < size; ++i){
        position = i;
        for (int j = i + 1; j < size; ++j){
            data[order[position]] < data[order[j]] ? position = j : 0;
        }
        temp = order[i];
        order[i] = order[position];
        order[position] = temp;
    }
}
// box states
const uint8_t
    EMPTY = 0,
    CROSS = 1,
    ZERO = 2,
    BLOCKED = 3;
struct Box
{
float *x;
float *y;
float *z;
uint8_t *state;
bool highlighted;
};
typedef struct Box Box;

void init_box(Box *b, float x[total_v], float y[total_v],
    float z[total_v], uint8_t *s, int i)
{
    int offset = i * 9;
    
    b->x = x + offset;
    b->y = y + offset;
    b->z = z + offset;
    b->state = s;
    b->highlighted = false;

    b->x[0] = (float) (indexToGridX(i) - 1);
    b->y[0] = (float) (indexToGridY(i) - 1);
    b->z[0] = (float) (indexToGridZ(i) + depth_start);

    b->x[1] = b->x[0] + ch;
    b->y[1] = b->y[0] + ch;
    b->z[1] = b->z[0] + ch;

    b->x[2] = b->x[0] - ch;
    b->y[2] = b->y[0] + ch;
    b->z[2] = b->z[0] + ch;

    b->x[3] = b->x[0] + ch;
    b->y[3] = b->y[0] - ch;
    b->z[3] = b->z[0] + ch;

    b->x[4] = b->x[0] + ch;
    b->y[4] = b->y[0] + ch;
    b->z[4] = b->z[0] - ch;

    b->x[5] = b->x[0] - ch;
    b->y[5] = b->y[0] - ch;
    b->z[5] = b->z[0] + ch;

    b->x[6] = b->x[0] - ch;
    b->y[6] = b->y[0] + ch;
    b->z[6] = b->z[0] - ch;

    b->x[7] = b->x[0] + ch;
    b->y[7] = b->y[0] - ch;
    b->z[7] = b->z[0] - ch;

    b->x[8] = b->x[0] - ch;
    b->y[8] = b->y[0] - ch;
    b->z[8] = b->z[0] - ch;
}
void render_box(SDL_Renderer *rend, Box b)
{
    uint8_t edge[12][2];
    float big[12];
    uint8_t order[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    // + + +
    edge[0][0] = 1;
    edge[0][1] = 2;
    big[0] = b.z[1] + b.z[2];

    edge[1][0] = 1;
    edge[1][1] = 3;
    big[1] = b.z[1] + b.z[3];

    edge[2][0] = 1;
    edge[2][1] = 4;
    big[2] = b.z[1] + b.z[4];
    // - - -
    edge[3][0] = 8;
    edge[3][1] = 7;
    big[3] = b.z[8] + b.z[7];

    edge[4][0] = 8;
    edge[4][1] = 6;
    big[4] = b.z[8] + b.z[6];

    edge[5][0] = 8;
    edge[5][1] = 5;
    big[5] = b.z[8] + b.z[5];
    //
    edge[6][0] = 2;
    edge[6][1] = 6;
    big[6] = b.z[2] + b.z[6];

    edge[7][0] = 6;
    edge[7][1] = 4;
    big[7] = b.z[4] + b.z[6];

    edge[8][0] = 4;
    edge[8][1] = 7;
    big[8] = b.z[4] + b.z[7];

    edge[9][0] = 7;
    edge[9][1] = 3;
    big[9] = b.z[3] + b.z[7];

    edge[10][0] = 3;
    edge[10][1] = 5;
    big[10] = b.z[3] + b.z[5];

    edge[11][0] = 5;
    edge[11][1] = 2;
    big[11] = b.z[2] + b.z[5];

    selectionSortOrderBuilderLocal(big, order, 12);

    uint8_t
        blue = (1 - b.highlighted) << 7,
        not_blue = 127 + (b.highlighted << 7);

    for (int i = 0; i < 6; ++i){
        SDL_SetRenderDrawColor(rend, not_blue, not_blue, blue, 255);
        SDL_RenderDrawLine(
        rend,
        (int) (b.x[edge[ order[i] ][0]]),
        (int) (b.y[edge[ order[i] ][0]]),
        (int) (b.x[edge[ order[i] ][1]]),
        (int) (b.y[edge[ order[i] ][1]])
        );
    }
    switch (*b.state)
    {
    case 0: // EMPTY
        break;
    case 1: //CROSS
        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
        thick_line(rend, b.x[1], b.y[1], b.x[8], b.y[8]);
        thick_line(rend, b.x[2], b.y[2], b.x[7], b.y[7]);
        thick_line(rend, b.x[3], b.y[3], b.x[6], b.y[6]);
        thick_line(rend, b.x[4], b.y[4], b.x[5], b.y[5]);
        break;
    case 2: // ZERO
        draw_circle(rend, b.x[0], b.y[0],
            universal_circle_base / b.z[0]);
        break;
    case 3: // BLOCKED
        SDL_SetRenderDrawColor(rend, not_blue, not_blue, blue, 255);
        SDL_RenderDrawLine(rend, b.x[1], b.y[1], b.x[5], b.y[5]);
        SDL_RenderDrawLine(rend, b.x[1], b.y[1], b.x[6], b.y[6]);
        SDL_RenderDrawLine(rend, b.x[1], b.y[1], b.x[7], b.y[7]);
        SDL_RenderDrawLine(rend, b.x[5], b.y[5], b.x[6], b.y[6]);
        SDL_RenderDrawLine(rend, b.x[6], b.y[6], b.x[7], b.y[7]);
        SDL_RenderDrawLine(rend, b.x[7], b.y[7], b.x[5], b.y[5]);
        SDL_RenderDrawLine(rend, b.x[8], b.y[8], b.x[2], b.y[2]);
        SDL_RenderDrawLine(rend, b.x[8], b.y[8], b.x[3], b.y[3]);
        SDL_RenderDrawLine(rend, b.x[8], b.y[8], b.x[4], b.y[4]);
        SDL_RenderDrawLine(rend, b.x[2], b.y[2], b.x[3], b.y[3]);
        SDL_RenderDrawLine(rend, b.x[3], b.y[3], b.x[4], b.y[4]);
        SDL_RenderDrawLine(rend, b.x[4], b.y[4], b.x[2], b.y[2]);
        break;
    default:
        break;
    }

    for (int i = 6; i < 12; ++i){
        SDL_SetRenderDrawColor(rend, not_blue, not_blue, blue, 255);
        SDL_RenderDrawLine(
        rend,
        (int) (b.x[edge[ order[i] ][0]]),
        (int) (b.y[edge[ order[i] ][0]]),
        (int) (b.x[edge[ order[i] ][1]]),
        (int) (b.y[edge[ order[i] ][1]])
        );
    }
}
bool check_collision(Box b, float x, float y)
{
    float z;
    x = b.x[0] - x;
    y = b.y[0] - y;
    z = universal_circle_base / b.z[0];
    return x*x + y*y < z*z;
}
void selectionSortOrderBuilderGlobal(
    Box data[], uint8_t order[], uint8_t size)
{
    uint8_t temp;
    int pos;
    for (int i = 0; i < size; ++i){
        pos = i;
        for (int j = i + 1; j < size; ++j){
            data[order[pos]].z[0] < data[order[j]].z[0] ? pos = j : 0;
        }
        temp = order[i];
        order[i] = order[pos];
        order[pos] = temp;
    }
}
