#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/artmath.h"
#include "include/constants.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "include/draw_tools.h"
#include "include/boxes.h"
#include "include/gamelogic.h"

int main(int argc, char *argv[])
{
    CAMERA_fow_horizontal = radians(90);
    CAMERA_sight_screen_width = tanff(CAMERA_fow_horizontal * 0.5f) * 2.0f;
    CAMERA_fow_vertical = radians(60);
    CAMERA_sight_screen_height = tanff(CAMERA_fow_vertical * 0.5f) * 2.0f;

    uint8_t *states_array;
    uint8_t states_grid[3][3][3];
    states_array = &states_grid[0][0][0];
    
    for (int i = 0; i < box_amount; ++i)
        states_array[i] = EMPTY;
    //states_grid[z][y][x]

    { // parsing arguments
    bool center_blocked = false;
    int ai_makes_first_move = 0;

    for (int i = 1; i < argc; ++i)
        switch (argv[i][0])
        {
        case 'b': // blocked center
            center_blocked = true;
            break;
        case 'f':
            center_blocked = false;
            break;
        case 'a': // ai makes first move
            ai_makes_first_move = 1;
            break;
        case 'd': // ai makes first move
            ai_makes_first_move = 2;
            break;
        case 'p': // player makes first move
            ai_makes_first_move = false;
            break;
        default:
            printf("Received unappropriated key: %c\n", argv[i][0]);
            return 1;
        }
    if (center_blocked)
        states_array[13] = BLOCKED;
    while (ai_makes_first_move--)
        states_array[choose_wisely(states_array)] = CROSS;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *wind = SDL_CreateWindow("Tic-Tac-Toe-3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0); // f: SDL_WINDOW_VULKAN
    if (!wind){
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    Uint32 r_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, r_flags);
    //SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND); // transparency
    if (!rend){
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 1;
    }

    if (TTF_Init())
        printf("Error initialising ttf: %s\n", SDL_GetError());

    SDL_Rect msg_rect = {SCREEN_HW - 200, SCREEN_HEIGHT - 80, 400, 70};
    SDL_Texture *winMsg[3];
    {
    TTF_Font *sans = TTF_OpenFont("ComicMono.ttf", 24);
    if (!sans)
        printf("Error creating font: %s\n", SDL_GetError());

    SDL_Color Yellow = {255, 255, 0};
    SDL_Surface* temp;

    temp = TTF_RenderText_Solid(sans, "Cross win!", Yellow);
    winMsg[CROSS - 1] = SDL_CreateTextureFromSurface(rend, temp);
    temp = TTF_RenderText_Solid(sans, "Zero win! ", Yellow);
    winMsg[ZERO - 1] = SDL_CreateTextureFromSurface(rend, temp);
    temp = TTF_RenderText_Solid(sans, "   Tie!   ", Yellow);
    winMsg[2] = SDL_CreateTextureFromSurface(rend, temp);

    TTF_CloseFont(sans);
    SDL_FreeSurface(temp);
    }

    // secondary boxes cords (after turning)
    float *vertexT_x;
    vertexT_x = malloc(sizeof(float) * total_v);
    float *vertexT_y;
    vertexT_y = malloc(sizeof(float) * total_v);
    float *vertexT_z;
    vertexT_z = malloc(sizeof(float) * total_v);

    Box *b_all;
    b_all = malloc(sizeof(Box) * box_amount);

    for (int i = 0; i < box_amount; ++i)
        init_box(&b_all[i], vertexT_x, vertexT_y,
            vertexT_z, states_array + i, i);

    // original boxes cords
    float *vertexO_x;
    vertexO_x = malloc(sizeof(float) * total_v);
    float *vertexO_y;
    vertexO_y = malloc(sizeof(float) * total_v);
    float *vertexO_z;
    vertexO_z = malloc(sizeof(float) * total_v);

    for (int i = 0; i < total_v; ++i){
        vertexO_x[i] = vertexT_x[i];
        vertexO_y[i] = vertexT_y[i];
        vertexO_z[i] = vertexT_z[i];
    }

    uint8_t render_order[box_amount];
    for (int i = 0; i < box_amount; ++i)
        render_order[i] = i;

    unsigned short turn = 0; // cap - 65535
    float radians = 0, sine = 0, cosine = 1;

    float
        temp_tx, temp_ty, temp_tz,
        cord_scale = scale_base, add_x, add_y, add_z,
        temp_ox, temp_oy, temp_oz;

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    int wheel_scale = 0;

    bool
        left_pressed = false,
        right_pressed = false,
        rmb_pressed = false,
        scene_turned = false,
        running = true;

    int8_t hl_i = -1, ai_move, game_state = EMPTY;

    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(event.button.button)
                    {
                    case 1:
                        if (game_state != EMPTY)
                            break;
                        // todo: ии занял последнюю клетку и вышла ничья (?)
                        if (hl_i != -1 && *b_all[hl_i].state == EMPTY){
                            *b_all[hl_i].state = ZERO;
                            game_state = check_array(states_array, hl_i);
                            if (game_state != EMPTY)
                                break;

                            ai_move = choose_wisely(states_array);
                            if (ai_move != -1){
                                *b_all[ai_move].state = CROSS;
                                game_state = check_array(states_array, ai_move);
                            }
                            else {
                                game_state = -1;
                            }
                        }
                        break;
                    case 3: rmb_pressed = true; break;
                    default: break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == 3)
                        rmb_pressed = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_ESCAPE: running = false; break;
                    case SDL_SCANCODE_LEFT: left_pressed = true; break;
                    case SDL_SCANCODE_RIGHT: right_pressed = true; break;
                    case SDL_SCANCODE_C:
                        for (int i = 0; i < box_amount; ++i)
                            states_array[i] = EMPTY;
                        break;
                    default: break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_LEFT: left_pressed = false; break;
                    case SDL_SCANCODE_RIGHT: right_pressed = false; break;
                    default: break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (rmb_pressed){
                        turn += (mouse_x - event.motion.x) << 5;
                        scene_turned = true;
                    }
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y){
                        wheel_scale += event.wheel.y;
                        wheel_scale = wheel_scale < 0 ? 0 : wheel_scale;
                        wheel_scale = wheel_scale > 10 ? 10 : wheel_scale;
                        cord_scale = scale_base + (float) wheel_scale / 10.0f;
                    } else {
                        turn += event.wheel.x << 10;
                        scene_turned = true;
                    }
                    break;

                default:
                    break;
            }
        }
        turn += (right_pressed - left_pressed) << 8;
        if ((left_pressed ^ right_pressed) | scene_turned){
            radians = (float) (turn) * 9.5875262e-5f;
            cosine = cosff(radians); sine = sinff(radians);
            scene_turned = false;
        }

        // 3D render
        for (int i = 0; i < total_v; ++i){
            temp_ox = vertexO_x[i];
            temp_oy = vertexO_y[i];
            temp_oz = vertexO_z[i] - depth_mid;

            if (i % 9 == 0){
                add_x = temp_ox * cord_scale - temp_ox;
                add_y = temp_oy * cord_scale - temp_oy;
                add_z = temp_oz * cord_scale - temp_oz;
            }

            temp_ox += add_x;
            temp_oy += add_y;
            temp_oz += add_z;

            temp_tx = temp_ox * cosine - temp_oz * sine;
            temp_tz = temp_oz * cosine + temp_ox * sine + depth_mid;

            temp_tx = 0.5f - temp_tx / (
                temp_tz * CAMERA_sight_screen_width);
            temp_tx *= SCREEN_WIDTH;

            temp_ty = temp_oy;
            temp_ty = 0.5f - temp_ty / (
                temp_tz * CAMERA_sight_screen_height);
            temp_ty *= SCREEN_HEIGHT;

            vertexT_x[i] = temp_tx;
            vertexT_y[i] = temp_ty;
            vertexT_z[i] = temp_tz;
        }

        if (hl_i != -1) b_all[hl_i].highlighted = false;
        hl_i = -1;   
        for (int i = 0; i < box_amount; ++i){
            if (check_collision(b_all[i], mouse_x, mouse_y))
                hl_i = ((hl_i == -1) || (b_all[hl_i].z[0] > b_all[i].z[0])) ? i : hl_i;
        }
        if (hl_i != -1) b_all[hl_i].highlighted = true;
        
        selectionSortOrderBuilderGlobal(b_all, render_order, box_amount);

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);

        for (int i = 0; i < box_amount; ++i)
            render_box(rend, b_all[render_order[i]]);

        if (game_state != EMPTY){
            if (game_state == -1)
                SDL_RenderCopy(rend, winMsg[2], NULL, &msg_rect);
            else
                SDL_RenderCopy(rend, winMsg[game_state - 1], NULL, &msg_rect);
        }

        SDL_RenderPresent(rend);
        SDL_Delay(1000/SCREEN_FPS);
    }

    SDL_DestroyTexture(winMsg[CROSS - 1]);
    SDL_DestroyTexture(winMsg[ZERO - 1]);
    SDL_DestroyTexture(winMsg[2]);

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);
    SDL_Quit();

    free(b_all);

    free(vertexT_x);
    free(vertexT_y);
    free(vertexT_z);

    free(vertexO_x);
    free(vertexO_y);
    free(vertexO_z);

    return 0;
}
