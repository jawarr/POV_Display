#include "stm32f4xx.h"
#include <stdint.h>

// Set initial time first 
#define RTC_ADDR       0x68

#define SECONDS_REG    0x00
#define MINUTES_REG    0x01
#define CENT_HOURS_REG 0x02
#define DAY_REG        0x03
#define DATE_REG       0x04
#define MONTH_REG      0x05
#define YEARS_REG      0x06

// ----------- GPIOB init for I2C1 on PB8/PB9 -----------
void gpio_i2c1_pb8_pb9_init(void)
{
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB8 = I2C1_SCL (AF4), PB9 = I2C1_SDA (AF4)
    // MODER: Alt Function (10)
    GPIOB->MODER &= ~((3u<<(8*2)) | (3u<<(9*2)));
    GPIOB->MODER |=  ((2u<<(8*2)) | (2u<<(9*2)));

    // AFRH: AF4 on PB8/PB9
    GPIOB->AFR[1] &= ~((0xFu<<((8-8)*4)) | (0xFu<<((9-8)*4)));
    GPIOB->AFR[1] |=  ((4u<<((8-8)*4)) | (4u<<((9-8)*4)));

    // Open-drain outputs for I2C
    //**************************ADD YOUR CODE HERE********************************************************
	 // GPIOB->OTYPER |= GPIO_OTYPER_OT8_Msk;
	 // GPIOB->OTYPER |= GPIO_OTYPER_OT9_Msk;
	
	  GPIOB->OTYPER |= (1 << 8);
    GPIOB->OTYPER |= (1 << 9);
    
    // Pull-ups (optional)
    GPIOB->PUPDR &= ~((3u<<(8*2)) | (3u<<(9*2)));
    GPIOB->PUPDR |=  ((1u<<(8*2)) | (1u<<(9*2)));  // 01 = pull-up

    // High speed for cleaner I2C edges
    GPIOB->OSPEEDR |= (3u<<(8*2)) | (3u<<(9*2));   // 11 = very high
}

// ----------- I2C1 init: Standard Mode 100 kHz @ PCLK1 = 16 MHz -----------
void i2c1_init_100k_16mhz(void)
{
    // Enable I2C1 clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Reset then release reset
    RCC->APB1RSTR |=  RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // Make sure I2C is disabled before configuring
    I2C1->CR1 = 0;
    I2C1->CR2 = 0;

    // CR2: PCLK1 frequency in MHz
    I2C1->CR2 = (16u & I2C_CR2_FREQ);

    // CCR: Standard mode (SM, 100 kHz): CCR = Fpclk1/(2*Fscl) = 16MHz/(2*100k) = 80
    //******************************ADD YOUR CODE HERE*********************************************************

	  I2C1->CCR = 80;
	
    // TRISE: in SM, TRISE = Fpclk1(MHz) + 1 = 17
    I2C1->TRISE = 17u;

    // Enable peripheral
    I2C1->CR1 |= I2C_CR1_PE;
		
		I2C1->CR1 |= I2C_CR1_ACK; // ACK Enabled

}

// Generate START and send 7-bit address + R/W bit
// dir_write: 1 = write, 0 = read
int i2c1_start_and_addr7(uint8_t addr7, uint8_t dir_write)
{
    // Generate START
    I2C1->CR1 |= I2C_CR1_START;
    // Wait for SB (start bit set)
    while (!(I2C1->SR1 & I2C_SR1_SB)) { }

    // Send 7-bit address + R/W bit
    uint8_t addr_byte = (addr7 << 1) | (dir_write ? 0u : 1u);
    I2C1->DR = addr_byte;

    // Wait for ADDR (address sent/ack)
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) { }

    // Clear ADDR by reading SR1 then SR2
    (void)I2C1->SR1; (void)I2C1->SR2;
    return 0;
}

void i2c1_stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

void delayMs(int n) {
 int i;
 for (; n > 0; n--)
 for (i = 0; i < 1067; i++) ;
}


// Write one register, one byte
int i2c1_write_reg1(uint8_t addr7, uint8_t reg, uint8_t data)
{
    i2c1_start_and_addr7(addr7, 1);  // write

    // Send register address
    while (!(I2C1->SR1 & I2C_SR1_TXE)) { }
    I2C1->DR = reg;
    
    // Wait for TXE/BTF before sending data
    while (!(I2C1->SR1 & I2C_SR1_TXE)) { }
    I2C1->DR = data;

    // Wait for byte transfer finished
    while (!(I2C1->SR1 & I2C_SR1_BTF)) { }

    // STOP
    i2c1_stop();
    return 0;
}


// Read one register, one byte
uint8_t i2c1_read_reg1(uint8_t addr7, uint8_t reg)
{
    uint8_t data;

    // STEP 1: Send device address (write mode)
    i2c1_start_and_addr7(addr7, 1);

    // Tell it which register we want
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = reg;
    while(!(I2C1->SR1 & I2C_SR1_TXE));

    // STEP 2: Repeated START
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));

    // STEP 3: Address + READ
    I2C1->DR = (addr7 << 1) | 1;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));

    // Disable ACK (only one byte)
    I2C1->CR1 &= ~I2C_CR1_ACK;
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    // STEP 4: Wait for data
    while(!(I2C1->SR1 & I2C_SR1_RXNE));
    data = I2C1->DR;

    // STOP
    i2c1_stop();

    // Re-enable ACK for future reads
    I2C1->CR1 |= I2C_CR1_ACK;

    return data;
}

uint8_t seconds_in_8_bits = 0;
uint8_t lower_8_bits_sec_reg = 0;
uint8_t upper_8_bits_sec_reg = 0;
int final_seconds = 0;


//Send the device address (write) to tell the RTC which register we want.

//Send the register address - write (like SECONDS_REG = 0x00).

//Send a repeated start and switch to read mode.

//Read one byte from that register.

//Send a STOP condition.

void read_seconds_reg(void)
{
	 //i2c1_write_just_register(RTC_ADDR, SECONDS_REG);
	seconds_in_8_bits = i2c1_read_reg1(RTC_ADDR, SECONDS_REG);
	
	lower_8_bits_sec_reg = seconds_in_8_bits & 0x0F;
	upper_8_bits_sec_reg = (seconds_in_8_bits >> 4) & 0x7;
	
	final_seconds = (upper_8_bits_sec_reg * 10) + lower_8_bits_sec_reg;
	
}


uint8_t write_data = 0;
void write_timer (void)
{
		write_data = 0xFF;	
} 

uint8_t minutes_in_8_bits = 0;
uint8_t lower_8_bits_min_reg = 0;
uint8_t upper_8_bits_min_reg = 0;
int final_minutes = 0;

void read_minutes_reg(void)
{
	minutes_in_8_bits = i2c1_read_reg1(RTC_ADDR, MINUTES_REG);
	
	lower_8_bits_min_reg = minutes_in_8_bits & 0x0F;
	upper_8_bits_min_reg = (minutes_in_8_bits >> 4) & 0x7;
	
	final_minutes = (upper_8_bits_min_reg * 10) + lower_8_bits_min_reg;
	
}

uint8_t cent_hours_in_8_bits = 0;
uint8_t lower_8_bits_cent_hours_reg = 0;
uint8_t upper_8_bits_cent_hours_reg = 0;
int final_cent_hours = 0;

void read_cent_hours(void)
{
	cent_hours_in_8_bits = i2c1_read_reg1(RTC_ADDR, CENT_HOURS_REG);
	
	lower_8_bits_cent_hours_reg = cent_hours_in_8_bits & 0x0F;
	upper_8_bits_cent_hours_reg = (cent_hours_in_8_bits >> 4) & 0x3;
	
	final_cent_hours = (upper_8_bits_cent_hours_reg * 10) + lower_8_bits_cent_hours_reg;
	
}


uint8_t day_reg_in_8_bits = 0;

void read_day_reg(void)
{
	day_reg_in_8_bits = i2c1_read_reg1(RTC_ADDR, DAY_REG);
	
	
	switch (day_reg_in_8_bits & 0x07)
  {
    case 1: /* Sunday */ break;
    case 2: /* Monday */ break;
    case 3: /* Tuesday */ break;
    case 4: /* Wednesday */ break;
    case 5: /* Thursday */ break;
    case 6: /* Friday */ break;
    case 7: /* Saturday */ break;
  }

}

void init_test_timer_readings(void)
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
	
		
	GPIOC->OTYPER &= ~0x03FFu;  // push-pull
  GPIOC->PUPDR  &= ~(0x3FFFFu);      // no pulls (outputs don’t need pulls)
	
}

void uart2_init(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA2 = TX (AF7), PA3 = RX (AF7)
    GPIOA->MODER &= ~((3u << (2*2)) | (3u << (3*2)));
    GPIOA->MODER |=  ((2u << (2*2)) | (2u << (3*2)));  // AF mode

    GPIOA->AFR[0] &= ~((0xF << (2*4)) | (0xF << (3*4)));
    GPIOA->AFR[0] |=  ((7u << (2*4)) | (7u << (3*4))); // AF7 = USART2

    // Enable USART2 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    USART2->BRR = 0x0683;   // 16 MHz / 9600 baud
    USART2->CR1 |= USART_CR1_TE;  // Transmitter enable
    USART2->CR1 |= USART_CR1_UE;  // USART enable
}

void uart2_write_char(char c)
{
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void uart2_write_string(const char *s)
{
    while (*s)
        uart2_write_char(*s++);
}

void uart_write_two_digits(int value)
{
    uart2_write_char('0' + (value / 10));
    uart2_write_char('0' + (value % 10));
}


void EXTI15_10_IRQHandler()
{
	if (EXTI->PR & EXTI_PR_PR13) {
        EXTI->PR = EXTI_PR_PR13;
        read_seconds_reg();
		    read_minutes_reg();
        read_cent_hours();
		
		     // Print format HH:MM:SS
        uart_write_two_digits(final_cent_hours);
        uart2_write_char(':');
        uart_write_two_digits(final_minutes);
        uart2_write_char(':');
        uart_write_two_digits(final_seconds);
        uart2_write_char('\n');
		
        GPIOC->ODR = (GPIOC->ODR & ~0xFF) | (final_seconds & 0xFF);
    }
	
}

void interrupt_init()
{
  RCC->APB2ENR |= (0x4000); // Setting up the clock for the External Interrupt
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;  //Mapping the button at PC13 to EXTI4 
	EXTI->IMR |= EXTI_IMR_MR13_Msk; //Unmask the interrupt on EXTI line 13
	EXTI->FTSR |= EXTI_FTSR_TR13_Msk; //Interrupt will be requested at the falling edge 
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

int main(void)
{
	gpio_i2c1_pb8_pb9_init();
	i2c1_init_100k_16mhz();
	uart2_init();
	
	init_test_timer_readings();
	interrupt_init();
	
  i2c1_write_reg1(RTC_ADDR, 0x00, 0x00); // seconds = 00 (CH=0)
	delayMs(5);
  i2c1_write_reg1(RTC_ADDR, 0x01, 0x00); // minutes = 00
	delayMs(5);
  i2c1_write_reg1(RTC_ADDR, 0x02, 0x12); // hours = 12
	delayMs(5);
  i2c1_write_reg1(RTC_ADDR, 0x03, 0x02); // Monday
	delayMs(5);
  i2c1_write_reg1(RTC_ADDR, 0x04, 0x01); // date = 1
	delayMs(5);
  i2c1_write_reg1(RTC_ADDR, 0x05, 0x01); // month = Jan
	delayMs(5);
  i2c1_write_reg1(RTC_ADDR, 0x06, 0x25); // year = 25
	delayMs(5);
	
	//i2c1_write_reg1(RTC_ADDR, 0x07, 0x00);
  delayMs(5);

	while(1);
	
}



