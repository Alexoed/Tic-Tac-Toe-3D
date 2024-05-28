#include <SDL2/SDL.h>

static float sines[13] = {
0, 0.5, 0.8660254,
1, 0.8660254, 0.5,
0, -0.5, -0.8660254,
-1, -0.8660254, -0.5, 0};

#define bv0 {{0, 0}, {0, 0, 255, 255}}
static SDL_Vertex circle_base[13] = {
bv0, bv0, bv0, bv0, bv0, bv0, bv0,
bv0, bv0, bv0, bv0, bv0, bv0};

void draw_circle(SDL_Renderer *rend, int x, int y, float r)
{
    for (int i = 0; i < 9; ++i){
        circle_base[i].position.x = x + sines[i + 3] * r;
        circle_base[i].position.y = y + sines[i] * r;
    }
    for (int i = 9; i < 13; ++i){
        circle_base[i].position.x = x + sines[i - 9] * r;
        circle_base[i].position.y = y + sines[i] * r;
    }
    SDL_RenderGeometry(rend, NULL, circle_base, 12, NULL, 0);
    SDL_RenderGeometry(rend, NULL, circle_base + 1, 12, NULL, 0);
}
void thick_line(SDL_Renderer *rend, int x1, int y1, int x2, int y2)
{
    int shift = (x2 - x1 < 0) ^ (y2 - y1 < 0) ? 1 : -1;
    SDL_RenderDrawLine(rend, x1 - shift, y1 - 1, x2 - shift, y2 - 1);
    SDL_RenderDrawLine(rend, x1, y1, x2, y2);
    SDL_RenderDrawLine(rend, x1 + shift, y1 + 1, x2 + shift, y2 + 1);
}