#ifndef I2C_RTC_H
#define I2C_RTC_H

#include <stdint.h>

#endif

/* Device address / registers */
#define BQ32002_ADDR  0x68   /* 7-bit address */
#define REG_SECONDS   0x00
#define REG_MINUTES   0x01
#define REG_HOURS     0x02

uint8_t bcd_to_bin(uint8_t b);

void gpio_i2c1_init(void);
void i2c1_init(void);

void i2c_start(uint8_t addr, uint8_t rw);
void i2c_stop(void);
uint8_t i2c_read_reg(uint8_t dev, uint8_t reg);

void rtc_set_time(uint8_t hour_bcd, uint8_t min_bcd, uint8_t sec_bcd);
