// fonts.h
#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

// Uppercase letters
extern uint8_t font_A[5];
extern uint8_t font_B[5];
extern uint8_t font_C[5];
extern uint8_t font_D[5];
extern uint8_t font_E[5];
extern uint8_t font_F[5];
extern uint8_t font_G[5];
extern uint8_t font_H[5];
extern uint8_t font_I[5];
extern uint8_t font_J[5];
extern uint8_t font_K[5];
extern uint8_t font_L[5];
extern uint8_t font_M[5];
extern uint8_t font_N[5];
extern uint8_t font_O[5];
extern uint8_t font_P[5];
extern uint8_t font_Q[5];
extern uint8_t font_R[5];
extern uint8_t font_S[5];
extern uint8_t font_T[5];
extern uint8_t font_U[5];
extern uint8_t font_V[5];
extern uint8_t font_W[5];
extern uint8_t font_X[5];
extern uint8_t font_Y[5];
extern uint8_t font_Z[5];

// Numbers
extern uint8_t font_0[5];
extern uint8_t font_1[5];
extern uint8_t font_2[5];
extern uint8_t font_3[5];
extern uint8_t font_4[5];
extern uint8_t font_5[5];
extern uint8_t font_6[5];
extern uint8_t font_7[5];
extern uint8_t font_8[5];
extern uint8_t font_9[5];

// Basic punctuation
extern uint8_t font_period[5];
extern uint8_t font_comma[5];
extern uint8_t font_exclam[5];
extern uint8_t font_question[5];
extern uint8_t font_dash[5];
extern uint8_t font_colon[5];
extern uint8_t font_degree[5];

// Space
extern uint8_t font_space[5];




const uint8_t* get_font(char c);

#endif