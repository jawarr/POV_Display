#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx.h"

// UART Configuration
#define UART_BAUDRATE 115200
#define MAX_CHARS 80
#define MAX_FIELDS 5

// User data structure
typedef struct {
    char buffer[MAX_CHARS];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];
} USER_DATA;

// Function prototypes
void initUart(void);
void setUartBaudRate(uint32_t baudRate);
void putcUart(char c);
void putsUart(char* str);
char getcUart(void);
bool kbhitUart(void);
char* getsUart(USER_DATA *data);
void parseFields(USER_DATA *data);
int compare_strings(const char *str1, const char *str2);
int alphabetToInteger(char* stringToConvert);
char* getFieldString(USER_DATA *data, uint8_t fieldNumber);
int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber);
bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments);

#endif