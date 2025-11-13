// main.c
#include "stm32f4xx.h"
#include "display.h"
#include "fonts.h"

// each col should be off by 1ms
// 18 sections 

//  PC13 -- turn on/off PC11 (motor connected to)
//  PC 12 -- slit interupt 
//  motor voltage 7.1 (V) | 0.7 - 1 (A)



volatile int8_t section = 0; //there are 18 sections  
volatile int8_t motor_run = 0;
//volatile display_data[5] = {};

void delay_us(int n) {
	 int i;
	 for (; n > 0; n--)
			for (i = 0; i < 1; i++);
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
	int i;
	for (; n > 0; n--)
	for (i = 0; i < 1067; i++) ;
}

void motor_and_display_init(){
    RCC->AHB1ENR |= 0x04;   // En portC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		
    GPIOC->MODER &= ~0x0C000000;    //Clear pin mode to input mode
    
    GPIOC->MODER &= ~(3UL << (11 * 2));  // Clear bits 22–23
    GPIOC->MODER |=  (1UL << (11 * 2));  // Set bit 22 to 1 (output mode)
		
    // Map EXTI13 -> Port C
    SYSCFG->EXTICR[3] &= ~(0xFu << 4);
    SYSCFG->EXTICR[3] |=  (0x2u << 4);      // 0x2 = PC

    EXTI->IMR  |=  (1u << 13);
    EXTI->RTSR &= ~(1u << 13);
    EXTI->FTSR |=  (1u << 13);              // falling edge (press)
    EXTI->PR    =  (1u << 13);              // clear pending
    
  
    //-------- for PC12 --------//
    GPIOC->MODER &= ~0x03000000;  //input mode
    SYSCFG->EXTICR[3] &= ~(0xFu);
		SYSCFG->EXTICR[3] |=  (0x2u);    			  // 0x2 = PC

		EXTI->IMR  |=  (1u << 12);
		EXTI->RTSR &= ~(1u << 12);
		EXTI->FTSR |=  (1u << 12);              // falling edge (press)
		EXTI->PR    =  (1u << 12);              // clear pending
    
    
    
    //en interrupt 
    NVIC_SetPriority(EXTI15_10_IRQn, 1);
    NVIC_EnableIRQ(EXTI15_10_IRQn);     //en
    
    
}


//---- This is for toggle motor and also displaying ----// 


void EXTI15_10_IRQHandler(void){	
    
    // This is for motor 
    if (EXTI->PR & (1u << 13)) {
        EXTI->IMR &= ~(1u << 13);
        EXTI->PR |= (1u << 13);

        motor_run ^= 1;   //toggle
        GPIOC->ODR ^= (1L << 11);   //toggle PC11 (motor) 

        // re-enable interrupt
        EXTI->IMR |= (1u << 13);
    }
    
    // This is for The slit hehe 
    if (EXTI->PR & (1U<<12)){
      EXTI->IMR &= ~(1u << 12);
      EXTI->PR |= (1u << 12);   //ack
        
      //--------- section max out at 17 -> go back to 0 ----------//
      section = (section + 1) % 18;
      
      
      if (section == 8) {
          int8_t i ;
          for (i = 4; i >= 0; i--){       //flips due to spinning in clock_wise
              //set_leds(display_data[i]);  // display each column
              GPIOC->ODR |= font_D[i];
              delayMs(1);   //1ms delay between each pattern 
              GPIOC->ODR &= ~(0x3FF) ;       //turn off before another pattern
              
          }
      }
      
      if (section == 9) {
          int8_t i ;
          for (i = 4; i >= 0; i--){
              //set_leds(display_data[i]);  // display each column
              GPIOC->ODR |= font_C[i];
              delayMs(1);   //1ms delay between each pattern 
              GPIOC->ODR &= ~(0x3FF) ;       //turn off before another pattern
              
          }
      }
      if (section == 10) {
          int8_t i ;
          for (i = 4; i >= 0; i--){
              //set_leds(display_data[i]);  // display each column
              GPIOC->ODR |= font_B[i];
              delayMs(1);   //1ms delay between each pattern 
              GPIOC->ODR &= ~(0x3FF) ;       //turn off before another pattern
              
          }
      }
      if (section == 11) {
          int8_t i ;
          for (i = 4; i >= 0; i--){ 
              //set_leds(display_data[i]);  // display each column
              GPIOC->ODR |= font_A[i];
              delayMs(1);   //1ms delay between each pattern 
              GPIOC->ODR &= ~(0x3FF) ;       //turn off before another pattern
              
          }
      }
        //reenable interrupt
        EXTI->PR  = (1u << 12);				
        EXTI->IMR |= (1u << 12);
    }
}



//Everytime the arms past this point (PA0 input), this "home" point is used calc where the arms will be at the future time. 
int main(void) {
    //Initialize display (sets up both GPIO ports)
    display_init();   
    motor_and_display_init();    // For motor control (PC13 to toggle PC11)
    
    input_letter('A');
    

    while (1) {
        //nothing 
    }
}