// fonts.h
#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

extern  uint8_t font_A[5];
extern  uint8_t font_B[5];
extern  uint8_t font_C[5];
extern  uint8_t font_D[5];
extern  uint8_t font_E[5];
extern  uint8_t font_F[5];
extern  uint8_t font_G[5];


extern  uint8_t font_1[5];
extern  uint8_t font_2[5];
extern  uint8_t font_space[5];

const uint8_t* get_font(char c);

#endif