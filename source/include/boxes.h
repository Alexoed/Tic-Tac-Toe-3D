#ifndef _BOXES_H_
#define _BOXES_H_

extern const uint8_t
    EMPTY,
    CROSS,
    ZERO,
    BLOCKED;
struct Box
{
float *x;
float *y;
float *z;
uint8_t *state;
bool highlighted;
};
typedef struct Box Box;
void init_box(Box *b, float x[total_v], float y[total_v], float z[total_v], uint8_t *s, int i);
void render_box(SDL_Renderer *rend, Box b);
bool check_collision(Box b, float x, float y);
void selectionSortOrderBuilderGlobal(Box data[], uint8_t order[], uint8_t size);

#endif
