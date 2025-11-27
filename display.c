// display.c
#include "display.h"
#include "fonts.h"
#include "stm32f4xx.h"

volatile uint8_t display_data[5] = {0};   //global so it can be loaded. 



/* 16 MHz SYSCLK */
void delayMs(int n) 
{
	int i;
	for (; n > 0; n--)
	for (i = 0; i < 1067; i++) ;
}

// Initialize GPIO for both arms
void display_init(void) {
    // Enable GPIO clocks for both ports
    RCC->AHB1ENR |= 0x04; //en portC
  
    GPIOC->MODER &= ~(0x3FF);  // Clear bits 0-9
    GPIOC->MODER |= 0x55555;   // Set pins 0-9 to output (01)
}

//Set pattern for the arm
void set_leds(uint8_t arm1)
{   
    GPIOC->ODR = (GPIOC->ODR & ~0x001F) | (arm1 & 0x001F);
}

//used to load display data into array    <-- dont need this for rn 
void input_letter(char letter) 
{
    const uint8_t* f = get_font(letter);
    for (int j = 0; j < 5; j++) 
		{
        display_data[j] = f[j];
    }   
}

void display_letter(char letter)
{
	const uint8_t* f = get_font(letter);
	for (int i = 4; i >=	0; i--) {     //reverse due to clockwise spin
		set_leds(f[i]);
		delayMs(1);
		set_leds(0x00);
	}
}