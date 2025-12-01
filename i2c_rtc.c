#include "stm32f4xx.h"
#include <stdint.h>

#define BQ32002_ADDR  0x68   // 7-bit address

#define REG_SECONDS   0x00
#define REG_MINUTES   0x01
#define REG_HOURS     0x02


uint8_t bcd_to_bin(uint8_t b)
{
    return ((b >> 4) * 10) + (b & 0x0F);
}


void gpio_i2c1_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    GPIOB->MODER &= ~((3<<(8*2)) | (3<<(9*2)));
    GPIOB->MODER |=  ((2<<(8*2)) | (2<<(9*2))); // AF

    GPIOB->AFR[1] &= ~(0xFF);
    GPIOB->AFR[1] |=  (0x44); // AF4

    GPIOB->OTYPER |= (1<<8) | (1<<9);
    GPIOB->PUPDR  &= ~((3<<(8*2)) | (3<<(9*2)));
    GPIOB->PUPDR  |=  ((1<<(8*2)) | (1<<(9*2))); // pull-up
    GPIOB->OSPEEDR |= (3<<(8*2)) | (3<<(9*2));
}


void i2c1_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C1->CR1 = 0;
    I2C1->CR2 = 16;
    I2C1->CCR = 80;
    I2C1->TRISE = 17;

    I2C1->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;
}

void i2c_start(uint8_t addr, uint8_t rw)
{
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = (addr<<1) | (rw & 1);
    while(!(I2C1->SR1 & I2C_SR1_ADDR));

    (void)I2C1->SR1;
    (void)I2C1->SR2;
}

void i2c_stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

uint8_t i2c_read_reg(uint8_t dev, uint8_t reg)
{
    uint8_t data;

    i2c_start(dev, 0);
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = reg;
    while(!(I2C1->SR1 & I2C_SR1_TXE));

    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = (dev<<1) | 1;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));

    I2C1->CR1 &= ~I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    while(!(I2C1->SR1 & I2C_SR1_RXNE));
    data = I2C1->DR;

    i2c_stop();
    I2C1->CR1 |= I2C_CR1_ACK;

    return data;
}


void uart2_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    GPIOA->MODER &= ~((3<<(2*2)) | (3<<(3*2)));
    GPIOA->MODER |=  ((2<<(2*2)) | (2<<(3*2)));

    GPIOA->AFR[0] &= ~((0xF<<(2*4)) | (0xF<<(3*4)));
    GPIOA->AFR[0] |=  ((7<<(2*4)) | (7<<(3*4)));

    USART2->BRR = 0x0683; // 9600 baud
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void uart2_write_char(char c)
{
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void uart2_write_two(int v)
{
    uart2_write_char('0' + v/10);
    uart2_write_char('0' + v%10);
}


void exti_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    GPIOC->MODER &= ~(3<<(13*2));

    SYSCFG->EXTICR[3] &= ~(0xF<<4);
    SYSCFG->EXTICR[3] |=  (2<<4);   // PC13

    EXTI->IMR  |= (1<<13);
    EXTI->FTSR |= (1<<13);

    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void rtc_set_time(uint8_t hour_bcd, uint8_t min_bcd, uint8_t sec_bcd)
{
    // Write starting at seconds register
    i2c_start(BQ32002_ADDR, 0);

    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = REG_SECONDS;  

    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = sec_bcd;      

    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = min_bcd;      

    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = hour_bcd;     

    i2c_stop();
}




void EXTI15_10_IRQHandler(void)
{
    if(EXTI->PR & (1<<13))
    {
        EXTI->PR = (1<<13);

        uint8_t sec_bcd = i2c_read_reg(BQ32002_ADDR, REG_SECONDS) & 0x7F;
        uint8_t min_bcd = i2c_read_reg(BQ32002_ADDR, REG_MINUTES) & 0x7F;

        uint8_t hr_raw = i2c_read_reg(BQ32002_ADDR, REG_HOURS);

        // ---- FIX FOR BQ32002 HOURS ----
        hr_raw &= 0x3F;   // remove 12/24 hr mode bits
        int hr = bcd_to_bin(hr_raw);

        int sec = bcd_to_bin(sec_bcd);
        int min = bcd_to_bin(min_bcd);

        uart2_write_two(hr);
        uart2_write_char(':');
        uart2_write_two(min);
        uart2_write_char(':');
        uart2_write_two(sec);
        uart2_write_char('\n');
    }
}



int main(void)
{
    gpio_i2c1_init();
    i2c1_init();
    uart2_init();
    exti_init();

	  rtc_set_time(0x14, 0x04, 0x00);
    while(1);
}
