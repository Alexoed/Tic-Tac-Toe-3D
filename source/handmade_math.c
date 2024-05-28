#include <stdint.h>

const double piD = 3.1415926535;
const float pi = piD;
const float tau = piD * 2;
const float half_pi = piD * 0.5;

float sinff(float x){
    float sign;
    sign = x < pi ? 1 : -1;
    x += x < pi ? 0 : -pi;
    float result = (x + pi) * x * (x - pi) * (x - tau) * 0.018181818;
    return result * sign;
}
float cosff(float x){
    x = x > half_pi ? -sinff(x - half_pi) : sinff(x + half_pi);
    return x;
}
float tanff(float x){
    return sinff(x) / cosff(x);
}
float radians(float x){
    return x * 0.0174532925;
}
uint8_t indexToGridX(uint8_t i){ return i % 3; }
uint8_t indexToGridY(uint8_t i){ return (i / 3) % 3; }
uint8_t indexToGridZ(uint8_t i){ return i / 9; }
