// main.c
#include "stm32f4xx.h"
#include "display.h"
#include "fonts.h"

// each col should be off by 1ms
// 18 sections 

//  PC13 -- turn on/off PC11 (input to motor driver)
//  PC12 -- motor encoder wheel to drive interrupt
//  motor voltage 7.1 (V) | 0.7 - 1 (A)



volatile int8_t section = 0; //there are 18 sections  
volatile int8_t motor_run = 0;
//volatile display_data[5] = {};


void motor_and_display_init(){
    RCC->AHB1ENR |= 0x04;   // En portC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		
    GPIOC->MODER &= ~0x0C000000;    //Clear pin mode to input mode
    
    GPIOC->MODER &= ~(3UL << (11 * 2));  // Clear bits 22�23
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
    
    // Motor toggle button interrupt 
    if (EXTI->PR & (1u << 13)) 
		{
        EXTI->IMR &= ~(1u << 13);
        EXTI->PR |= (1u << 13);

        motor_run ^= 1;   //toggle
        GPIOC->ODR ^= (1L << 11);   //toggle PC11 (motor) 

        // re-enable interrupt
        EXTI->IMR |= (1u << 13);
    }
    
    // Motor encoder wheel interrupt
    if (EXTI->PR & (1U<<12)){
        EXTI->IMR &= ~(1u << 12);
        EXTI->PR |= (1u << 12);   //ack
        
        //--------- section max out at 17 -> go back to 0 ----------//
        section = (section + 1) % 18;
        
        switch (section)
        {
					case 0:  display_letter('a'); break;
					case 1:  display_letter('b'); break;
					case 2:  display_letter('c'); break;
					case 3:  display_letter('d'); break;
					case 4:  display_letter('e'); break;
					case 5:  display_letter('f'); break;
					case 6:  display_letter('g'); break;
					case 7:  display_letter('h'); break;
					case 8:  display_letter('i'); break;
					case 9:  display_letter('j'); break;
					case 10: display_letter('k'); break;
					case 11: display_letter('l'); break;
					case 12: display_letter('m'); break;
					case 13: display_letter('n'); break;
					case 14: display_letter('o'); break;
					case 15: display_letter('p'); break;
					case 16: display_letter('q'); break;
					case 17: display_letter('r'); break;
					default: break;
        }
				
//				switch (section)
//        {
//					case 0:  display_letter('s'); break;
//					case 1:  display_letter('t'); break;
//					case 2:  display_letter('u'); break;
//					case 3:  display_letter('v'); break;
//					case 4:  display_letter('w'); break;
//					case 5:  display_letter('x'); break;
//					case 6:  display_letter('y'); break;
//					case 7:  display_letter('z'); break;
//					case 8:  display_letter('0'); break;
//					case 9:  display_letter('1'); break;
//					case 10: display_letter('2'); break;
//					case 11: display_letter('3'); break;
//					case 12: display_letter('4'); break;
//					case 13: display_letter('5'); break;
//					case 14: display_letter('6'); break;
//					case 15: display_letter('7'); break;
//					case 16: display_letter('8'); break;
//					case 17: display_letter('9'); break;
//					default: break;
//        }
//				
//				switch (section)
//        {
//					case 0:  display_letter('.'); break;
//					case 1:  display_letter(','); break;
//					case 2:  display_letter('!'); break;
//					case 3:  display_letter('?'); break;
//					case 4:  display_letter('-'); break;
//					case 5:  display_letter(':'); break;
//					case 6:  display_letter(' '); break;
//					case 7:  display_letter(' '); break;
//					case 8:  display_letter(' '); break;
//					case 9:  display_letter(' '); break;
//					case 10: display_letter(' '); break;
//					case 11: display_letter(' '); break;
//					case 12: display_letter(' '); break;
//					case 13: display_letter(' '); break;
//					case 14: display_letter(' '); break;
//					case 15: display_letter(' '); break;
//					case 16: display_letter(' '); break;
//					case 17: display_letter(' '); break;
//					default: break;
//        }
        
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