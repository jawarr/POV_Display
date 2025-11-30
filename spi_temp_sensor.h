/* spi_temp_sensor.h
 * Public header for the SPI temperature sensor helper.
 * Provides a small API to initialize/read temperature from SPI sensor.
 */

#ifndef SPI_TEMP_SENSOR_H
#define SPI_TEMP_SENSOR_H

#include <stdint.h>

void spi1_gpio_init(void);
void spi1_init_1MHz_mode0(void);

// Read one measurement from the sensor and update internal globals
void sendData_To_Master(void);

// Return the last read temperature in degrees Celsius (calls sendData_To_Master internally)
float get_temperature(void);

// Expose raw/processed results if calling code wants to inspect them
extern uint8_t hold_temperature_data_MSB;
extern uint8_t hold_temperature_data_LSB;
extern uint16_t final_temp_in_bits;
extern uint16_t final_temp_in_10_bits;
extern float   actual_reading;


#endif // SPI_TEMP_SENSOR_H
