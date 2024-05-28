#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "include/artmath.h"
#include "include/constants.h"
#include "include/boxes.h"

#define check_this if (a == b && b == c && a != EMPTY) return a;
uint8_t check_array(uint8_t states[box_amount], uint8_t i)
{
    uint8_t
        z = indexToGridZ(i) * 9,
        y = indexToGridY(i) * 3,
        x = indexToGridX(i);
    uint8_t a, b, c;
    // z-lines
    a = states[y+x]; b = states[9+y+x]; c = states[18+y+x];
    check_this

    // y-lines
    a = states[z+x]; b = states[z+x+3]; c = states[z+x+6];
    check_this

    // x-lines
    a = states[z+y]; b = states[z+y+1]; c = states[z+y+2];
    check_this

    // z-diagonals
    b = states[z+4];

    a = states[z]; c = states[z+8];
    check_this
    a = states[z+6]; c = states[z+2];
    check_this

    // y-diagonals
    b = states[y+10];

    a = states[y]; c = states[y+20];
    check_this
    a = states[y+18]; c = states[y+2];
    check_this

    // x-diagonals
    b = states[x+12];

    a = states[x]; c = states[x+24];
    check_this
    a = states[x+6]; c = states[x+18];
    check_this

    // 3D slices
    b = states[13];

    a = states[0]; c = states[26];
    check_this
    a = states[2]; c = states[24];
    check_this
    a = states[6]; c = states[20];
    check_this
    a = states[18]; c = states[8];
    check_this

    return EMPTY;
}
int8_t choose(uint8_t states[box_amount])
{
    for (int i = 0; i < box_amount; ++i){
        if (states[i] == EMPTY)
            return i;
    }
    return -1;
}
static uint8_t sweet_spots[27] =
{
    13, // central
    0, 2, 6, 8, 18, 20, 24, 26, // corner
    4, 10, 12, 14, 16, 22, // mid-side
    1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25 // mid-edge
};
int8_t choose_wisely(uint8_t states[box_amount])
{
    // copy states
    uint8_t temp[box_amount];
    for (int i = 0; i < box_amount; ++i) temp[i] = states[i];
    // count free spots
    uint8_t counter = 0;
    uint8_t available_spots[box_amount];

    for (int i = 0; i < box_amount; ++i)
        if (states[i] == EMPTY)
            available_spots[counter++] = i;

    //check for one-turn wins
    for (int i = 0; i < counter; ++i){
        temp[available_spots[i]] = CROSS;
        if (check_array(temp, available_spots[i]) == CROSS)
            return available_spots[i];
        temp[available_spots[i]] = EMPTY;
    }
    // prevent player fom winning
    for (int i = 0; i < counter; ++i){
        temp[available_spots[i]] = ZERO;
        if (check_array(temp, available_spots[i]) == ZERO)
            return available_spots[i];
        temp[available_spots[i]] = EMPTY;
    }
    // if this line is reached, no one-turn wins/blocks were found
    // going for sweet-spots then
    for (int i = 0; i < box_amount; ++i)
        if (states[sweet_spots[i]] == EMPTY)
            return sweet_spots[i];

    return -1; // no possible turns
}
