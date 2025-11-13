// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
void set_leds(uint8_t);
void input_letter(char);
void display_init(void);

extern volatile uint8_t display_data[5]; 

#endif