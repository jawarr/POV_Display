#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "spi_temp_sensor.h"


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
void spi1_gpio_init(void)
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

void spi1_init_1MHz_mode0(void)
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
	
	
	cs_high();
}

// Return a fresh reading (in degrees C).  This function performs a sensor read
// by calling sendData_To_Master() then returns the processed float value.
float get_temperature(void)
{
	// read the sensor (updates actual_reading)
	sendData_To_Master();
	return actual_reading;
}
