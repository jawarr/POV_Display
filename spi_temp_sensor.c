#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>


#define CS_PORT GPIOA
#define CS_PIN  4

static inline void cs_low(void)  
{ 
  CS_PORT->BSRR = (uint32_t)(1u << (CS_PIN + 16)); 
}

static inline void cs_high(void) 
{ 
  CS_PORT->BSRR = (uint32_t)(1u << CS_PIN); 


}

// ---------------- SPI hygiene ----------------
static void spi1_flush_rx_ovr(void) 
{
    // Drain RX FIFO
    while (SPI1->SR & SPI_SR_RXNE) (void)(*(volatile uint8_t*)&SPI1->DR);
    // Clear OVR if set: read DR then SR
    if (SPI1->SR & SPI_SR_OVR)
    {
        (void)(*(volatile uint8_t*)&SPI1->DR);
        (void)SPI1->SR;
    }
}

static uint8_t spi1_txrx(uint8_t b)
{
    while (!(SPI1->SR & SPI_SR_TXE)) { }
    *(volatile uint8_t*)&SPI1->DR = b;

    while (!(SPI1->SR & SPI_SR_RXNE)) { }
    uint8_t r = *(volatile uint8_t*)&SPI1->DR;

    while (SPI1->SR & SPI_SR_BSY) { }   // ensure last bit out when we might toggle CS
    return r;
}

// ---------------- GPIO + SPI init ----------------
static void gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    // PA5/6/7 -> AF5 (SPI1)
    GPIOA->MODER   &= ~((3u<<(5*2)) | (3u<<(6*2)) | (3u<<(7*2)));
    GPIOA->MODER   |=  ((2u<<(5*2)) | (2u<<(6*2)) | (2u<<(7*2)));
    GPIOA->AFR[0]  &= ~((0xFu<<(5*4)) | (0xFu<<(6*4)) | (0xFu<<(7*4)));
    GPIOA->AFR[0]  |=  ((5u<<(5*4)) | (5u<<(6*4)) | (5u<<(7*4)));
    GPIOA->OSPEEDR |=  ((3u<<(5*2)) | (3u<<(6*2)) | (3u<<(7*2)));
    GPIOA->OTYPER  &= ~((1u<<5) | (1u<<6) | (1u<<7));
    GPIOA->PUPDR   &= ~((3u<<(5*2)) | (3u<<(6*2)) | (3u<<(7*2)));

    // PA4 -> CS as push-pull output, idle high
    GPIOA->MODER   &= ~(3u<<(CS_PIN*2));
    GPIOA->MODER   |=  (1u<<(CS_PIN*2));
    GPIOA->OSPEEDR |=  (3u<<(CS_PIN*2));
    GPIOA->OTYPER  &= ~(1u<<CS_PIN);
    GPIOA->PUPDR   &= ~(3u<<(CS_PIN*2));
    cs_high();
	
	  RCC->AHB1ENR |= 0x00000004;
	  GPIOC->MODER &= ~0x0C000000; //Clear bit 10 and 11 of the pin 
	  GPIOC->MODER |= 0x00000000; //Set pin as input 
	  
}

static void spi1_init_1MHz_mode0(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    // Master, SSM/SSI (software NSS), BR=/16 -> 1 MHz @ 16 MHz, CPOL=0, CPHA=0
    SPI1->CR1 = SPI_CR1_MSTR
              | SPI_CR1_SSM
              | SPI_CR1_SSI
              | (3u << SPI_CR1_BR_Pos);  // /16

    SPI1->CR1 |= SPI_CR1_SPE;
    spi1_flush_rx_ovr();
}

uint8_t hold_temperature_data_MSB = 0;
uint8_t hold_temperature_data_LSB = 0;
uint16_t final_temp_in_bits = 0;
uint16_t final_temp_in_10_bits = 0;
float actual_reading = 0; 

void sendData_To_Master(void)
{
	cs_low();
	hold_temperature_data_MSB = spi1_txrx(0x0000);
	hold_temperature_data_LSB = spi1_txrx(0x0000);
	
	final_temp_in_bits = (hold_temperature_data_MSB << 8) | (hold_temperature_data_LSB);
	final_temp_in_10_bits = final_temp_in_bits >> 5;
	
	
	// hold_temperature_data will hold a 16 bit count value from whom we need to extract 10 bits only - D5 to D14
	actual_reading = final_temp_in_10_bits * 0.25;
	
	//printf("The outside temperature is: %d", actual_reading);
	
	cs_high();
}

void init_test_temp_readings(void)
{
	RCC->AHB1ENR |= 0x4;
	
	// Set up PC0
	GPIOC->MODER &= ~0x03;
	GPIOC->MODER |= 0x01;
		
	//PC1
	GPIOC->MODER &= ~0xC;
	GPIOC->MODER |= 0x4;	
	
	//PC2
	GPIOC->MODER &= ~0x30;
	GPIOC->MODER |= 0x10;
		
	//PC3
	GPIOC->MODER &= ~0xC0;
	GPIOC->MODER |= 0x40;
		
	
	//PC4
	GPIOC->MODER &= ~0x300;
	GPIOC->MODER |= 0x100;
		
	
	//PC5
	GPIOC->MODER &= ~0xC00;
	GPIOC->MODER |= 0x400;
		
	
	//PC6
	GPIOC->MODER &= ~0x3000;
	GPIOC->MODER |= 0x1000;
		
	
	//PC7
	GPIOC->MODER &= ~0xC000;
	GPIOC->MODER |= 0x4000;
		
	
	//PC8
	GPIOC->MODER &= ~0x30000;
	GPIOC->MODER |= 0x10000;
		
	
	//PC9 
	GPIOC->MODER &= ~0xC0000;
	GPIOC->MODER |= 0x40000;
		
	GPIOC->OTYPER &= ~0x03FFu;  // push-pull
  GPIOC->PUPDR  &= ~(0x3FFFFu);      // no pulls (outputs don’t need pulls)
	
}

void EXTI15_10_IRQHandler()
{
	EXTI->IMR &= ~EXTI_IMR_MR13_Msk; //Unmask the interrupt on EXTI line 13
	if (EXTI->PR & EXTI_PR_PR13){
    EXTI->PR = EXTI_PR_PR13;         // clear pending
		sendData_To_Master();
		
		GPIOC->ODR = (GPIOC->ODR & ~0x3FF) | (final_temp_in_10_bits & 0x3FF);
	

	}
	EXTI->IMR |= EXTI_IMR_MR13_Msk; //Unmask the interrupt on EXTI line 13
	
}

void interrupt_init()
{
  RCC->APB2ENR |= (0x4000); // Setting up the clock for the External Interrupt
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;  //Mapping the button at PC13 to EXTI4 
	EXTI->IMR |= EXTI_IMR_MR13_Msk; //Unmask the interrupt on EXTI line 13
	EXTI->FTSR |= EXTI_FTSR_TR13_Msk; //Interrupt will be requested at the falling edge 
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

int main (void)
{
	gpio_init();
	spi1_init_1MHz_mode0();
	interrupt_init();
	init_test_temp_readings();
	
	while(1);
}