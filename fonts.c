// fonts.c
#include "fonts.h"

// Character bitmaps (5 columns each, 5-bit high, bit 0 = bottom pixel, bit 4 = top pixel, black = on = 1)
// These are derived from the pixel font in the provided image by analyzing the character map.
// The image shows a 5x5 pixel font with black pixels indicating 'on'.
// For each character, I examined the grid:
// - Rows numbered 0 (top) to 4 (bottom).
// - Columns 0 (left) to 4 (right).
// - For each column, read pixels from top to bottom, but store with bit 0 as bottom pixel (row 4), bit 1 as row 3, etc., up to bit 4 as top (row 0).
// Example for 'A':
// Row 0 (top): 0 1 1 1 0       (LSB)
// Row 1:       1 0 0 0 1
// Row 2:       1 1 1 1 1
// Row 3:       1 0 0 0 1
// Row 4 (bot): 1 0 0 0 1       (MSB)

//                        16   8   4   2   1
//                        

// Uppercase letters
uint8_t font_A[5] = {0x1E, 0x05, 0x05, 0x05, 0x1E};       
uint8_t font_B[5] = {0x1F, 0x15, 0x15, 0x15, 0x1B};
uint8_t font_C[5] = {0x1F, 0x11, 0x11, 0x11, 0x11};
uint8_t font_D[5] = {0x1F, 0x11, 0x11, 0x11, 0x0E};   
uint8_t font_E[5] = {0x1F, 0x15, 0x15, 0x11, 0x11};  
uint8_t font_F[5] = {0x1F, 0x05, 0x05, 0x01, 0x01};
uint8_t font_G[5] = {0x1F, 0x11, 0x15, 0x15, 0x1D};
uint8_t font_H[5] = {0x1F, 0x04, 0x04, 0x04, 0x1F};
uint8_t font_I[5] = {0x00, 0x11, 0x1F, 0x11, 0x00};
uint8_t font_J[5] = {0x18, 0x10, 0x10, 0x1F, 0x00};
uint8_t font_K[5] = {0x1F, 0x04, 0x0A, 0x11, 0x00};      
uint8_t font_L[5] = {0x1F, 0x10, 0x10, 0x10, 0x10};
uint8_t font_M[5] = {0x1F, 0x02, 0x04, 0x02, 0x1F};
uint8_t font_N[5] = {0x1F, 0x02, 0x04, 0x08, 0x1F};
uint8_t font_O[5] = {0x1F, 0x11, 0x11, 0x11, 0x1F};
uint8_t font_P[5] = {0x1F, 0x05, 0x05, 0x05, 0x07};
uint8_t font_Q[5] = {0x1F, 0x11, 0x15, 0x19, 0x1F}; 
uint8_t font_R[5] = {0x1F, 0x05, 0x05, 0x0D, 0x17}; 
uint8_t font_S[5] = {0x02, 0x15, 0x15, 0x15, 0x08};
uint8_t font_T[5] = {0x01, 0x01, 0x1F, 0x01, 0x01}; 
uint8_t font_U[5] = {0x1F, 0x10, 0x10, 0x10, 0x1F}; 
uint8_t font_V[5] = {0x07, 0x08, 0x10, 0x08, 0x07};
uint8_t font_W[5] = {0x0F, 0x10, 0x0C, 0x10, 0x0F};
uint8_t font_X[5] = {0x11, 0x0A, 0x04, 0x0A, 0x11}; 
uint8_t font_Y[5] = {0x01, 0x02, 0x1C, 0x02, 0x01}; 
uint8_t font_Z[5] = {0x11, 0x19, 0x15, 0x13, 0x11};

// Numbers
uint8_t font_0[5] = {0x0E, 0x11, 0x11, 0x11, 0x0E};
uint8_t font_1[5] = {0x00, 0x12, 0x1F, 0x10, 0x00};
uint8_t font_2[5] = {0x08, 0x15, 0x15, 0x15, 0x02};
uint8_t font_3[5] = {0x00, 0x15, 0x15, 0x15, 0x0A};
uint8_t font_4[5] = {0x07, 0x04, 0x04, 0x04, 0x1F};
uint8_t font_5[5] = {0x17, 0x15, 0x15, 0x15, 0x08};
uint8_t font_6[5] = {0x0E, 0x15, 0x15, 0x15, 0x08};
uint8_t font_7[5] = {0x01, 0x01, 0x19, 0x05, 0x03};
uint8_t font_8[5] = {0x0A, 0x15, 0x15, 0x15, 0x0A};
uint8_t font_9[5] = {0x02, 0x15, 0x15, 0x15, 0x0A};

// Basic punctuation
uint8_t font_period[5]   = {0x00, 0x00, 0x10, 0x00, 0x00}; // .
uint8_t font_comma[5]    = {0x00, 0x00, 0x10, 0x08, 0x00}; // ,
uint8_t font_exclam[5]   = {0x00, 0x00, 0x17, 0x00, 0x00}; // !
uint8_t font_question[5] = {0x01, 0x01, 0x15, 0x05, 0x07}; // ?
uint8_t font_dash[5]     = {0x00, 0x04, 0x04, 0x04, 0x00}; // -
uint8_t font_colon[5]    = {0x00, 0x00, 0x0A, 0x00, 0x00}; // :

// Space
uint8_t font_space[5] = {0x00, 0x00, 0x00, 0x00, 0x00};


const uint8_t* get_font(char c) {
    switch (c) {
        case 'A': case 'a': return font_A;
        case 'B': case 'b': return font_B;
        case 'C': case 'c': return font_C;
        case 'D': case 'd': return font_D;
        case 'E': case 'e': return font_E;
        case 'F': case 'f': return font_F;
        case 'G': case 'g': return font_G;
        case 'H': case 'h': return font_H;
        case 'I': case 'i': return font_I;
        case 'J': case 'j': return font_J;
        case 'K': case 'k': return font_K;
        case 'L': case 'l': return font_L;
        case 'M': case 'm': return font_M;
        case 'N': case 'n': return font_N;
        case 'O': case 'o': return font_O;
        case 'P': case 'p': return font_P;
        case 'Q': case 'q': return font_Q;
        case 'R': case 'r': return font_R;
        case 'S': case 's': return font_S;
        case 'T': case 't': return font_T;
        case 'U': case 'u': return font_U;
        case 'V': case 'v': return font_V;
        case 'W': case 'w': return font_W;
        case 'X': case 'x': return font_X;
        case 'Y': case 'y': return font_Y;
        case 'Z': case 'z': return font_Z;

        case '0': return font_0;
        case '1': return font_1;
        case '2': return font_2;
        case '3': return font_3;
        case '4': return font_4;
        case '5': return font_5;
        case '6': return font_6;
        case '7': return font_7;
        case '8': return font_8;
        case '9': return font_9;

        case '.': return font_period;
        case ',': return font_comma;
        case '!': return font_exclam;
        case '?': return font_question;
        case '-': return font_dash;
        case ':': return font_colon;

        default: return font_space;  // Unknown chars show as space
    }
}