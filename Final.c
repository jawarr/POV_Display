// main.c
#include <stdio.h>
#include "stm32f4xx.h"
#include "display.h"
#include "fonts.h"
#include "UART.h"
#include "spi_temp_sensor.h"

// each col should be off by 1ms
// 18 sections 

//  PC13 -- turn on/off PC11 (input to motor driver)
//  PC12 -- motor encoder wheel to drive interrupt
//  motor voltage 7.1 (V) | 0.7 - 1 (A)




volatile int8_t string_display[18] = {'A','B','C','D','E','F','G',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};  // this need to be populated, default as empty  
volatile int8_t section = 0; //there are 18 sections  
volatile int8_t motor_run = 0;


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
        
//				// Sample display code, shows each character
//        switch (section)
//        {
//					case 0:  display_letter('a'); break;
//					case 1:  display_letter('b'); break;
//					case 2:  display_letter('c'); break;
//					case 3:  display_letter('d'); break;
//					case 4:  display_letter('e'); break;
//					case 5:  display_letter('f'); break;
//					case 6:  display_letter('g'); break;
//					case 7:  display_letter('h'); break;
//					case 8:  display_letter('i'); break;
//					case 9:  display_letter('j'); break;
//					case 10: display_letter('k'); break;
//					case 11: display_letter('l'); break;
//					case 12: display_letter('m'); break;
//					case 13: display_letter('n'); break;
//					case 14: display_letter('o'); break;
//					case 15: display_letter('p'); break;
//					case 16: display_letter('q'); break;
//					case 17: display_letter('r'); break;
//					default: break;
//        }


				switch (section)
        {
					case 0:  display_letter(string_display[17]); break;      //reverse idx due to clockwise spin (letter are painted backward) 
					case 1:  display_letter(string_display[16]); break;
					case 2:  display_letter(string_display[15]); break;
					case 3:  display_letter(string_display[14]); break;
					case 4:  display_letter(string_display[13]); break;
					case 5:  display_letter(string_display[12]); break;
					case 6:  display_letter(string_display[11]); break;
					case 7:  display_letter(string_display[10]); break;
					case 8:  display_letter(string_display[9]); break;
					case 9:  display_letter(string_display[8]); break;
					case 10: display_letter(string_display[7]); break;
					case 11: display_letter(string_display[6]); break;
					case 12: display_letter(string_display[5]); break;
					case 13: display_letter(string_display[4]); break;
					case 14: display_letter(string_display[3]); break;
					case 15: display_letter(string_display[2]); break;
					case 16: display_letter(string_display[1]); break;
					case 17: display_letter(string_display[0]); break;
					default: break;
        }
        
        //reenable interrupt
        EXTI->PR  = (1u << 12);				
        EXTI->IMR |= (1u << 12);
    }
}
void str_populate(char* str)
{
  
     __disable_irq();   //disable interrupt for loading new str safely
    for (int i = 0; i < 18; i++)
		{
        if(str[i] >= 33 && str[i] <= 126)
				{
            string_display[i] = str[i];
        }
				else 
				{
          string_display[i] = ' ';  // pad with space, not null
        }
    }
     __enable_irq();
}

int main(void) 
{
    display_init();   					// Initialize display (sets up GPIO ports)
    motor_and_display_init();   // For motor control (PC13 to toggle PC11)
    initUart();									// Initialize UART terminal
    spi1_gpio_init();						// Initialize GPIO for SPI1
	  spi1_init_1MHz_mode0();			// Initialize SPI1 for 1MHz, Mode 0
	
    USER_DATA data;   					//user input data 
		
	
		putsUart("\e[0;92m"); 									// bold green text
		putsUart("Enter text to display:\n\r");			
    putsUart("\e[0m");	  									// reset to default text
	
    while (1) 
		{
      memset(&data, 0, sizeof(data)); // ensure the whole structure (buffer, fieldCount, positions) is cleared each loop

      getsUart(&data);    // get data string input
      parseFields(&data); // parse data into fields

      // Check for temperature command
      if (isCommand(&data, "temperature", 1))
      {
        char buffer[18];
        char *text = getFieldString(&data, 1);
        if (compare_strings(text, "c"))
        {
          float temperature = get_temperature();
          snprintf(buffer, sizeof(buffer), "%.2f C", temperature);
          str_populate(buffer);
        }
        else if (compare_strings(text, "f"))
        {
          float temperature = get_temperature();
          float temperature_f = (temperature * 9.0f / 5.0f) + 32.0f;
          snprintf(buffer, sizeof(buffer), "%.2f F", temperature_f);
          str_populate(buffer);
        }
        else
        {
          putsUart("Unknown temperature unit. Use 'c' or 'f'.\r\n");
        }
      }

      else str_populate(data.buffer);    // populate the display str base on the input
		}
}