//#include "send_text_uart.h"

//// Simple delay function
//static void _delay_cycles(volatile uint32_t cycles)
//{
//    while(cycles--);
//}

//void initUart(void)
//{
//    // Enable GPIOA clock
//    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//    
//    // Enable USART2 clock (USART2 is on PA2=TX, PA3=RX)
//    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
//    
//    _delay_cycles(1000);
//    
//    // Configure GPIO pins for USART2
//    // PA2 (TX) and PA3 (RX) in alternate function mode
//    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
//    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);  // Alternate function mode
//    
//    // Set alternate function AF7 for USART2
//    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
//    GPIOA->AFR[0] |= (7 << (2 * 4)) | (7 << (3 * 4));  // AF7 for both pins
//    
//    // Configure USART2
//    USART2->BRR = SystemCoreClock / UART_BAUDRATE;  // Set baud rate
//    USART2->CR1 = USART_CR1_TE | USART_CR1_RE |    // Enable transmitter and receiver
//                  USART_CR1_UE;                    // Enable USART
//}

//void setUartBaudRate(uint32_t baudRate)
//{
//    USART2->BRR = SystemCoreClock / baudRate;
//}

//void putcUart(char c)
//{
//    // Wait until transmit data register is empty
//    while (!(USART2->SR & USART_SR_TXE));
//    USART2->DR = c;
//}

//void putsUart(char* str)
//{
//    uint8_t i = 0;
//    while (str[i] != '\0')
//        putcUart(str[i++]);
//}

//char getcUart(void)
//{
//    // Check for overrun error
//    if (USART2->SR & USART_SR_ORE) {
//        //char hex_buffer[9];
//        int i, j;
//        
//        // Clear the overrun error by reading SR and DR
//        volatile uint32_t tmp = USART2->SR;
//        tmp = USART2->DR;
//        (void)tmp;
//        
//        const char *message = "OVERRUN ERROR 0x";
//        for (i = 0; message[i] != '\0'; i++) {
//            putcUart(message[i]);
//        }
//        
//        // In STM32, we don't have the same status register format
//        // So we'll just indicate an error occurred
//        for (j = 0; j < 4; j++) {
//            putcUart('E');
//        }
//        putcUart('\n');
//    }
//    
//    // Wait until data is received
//    while (!(USART2->SR & USART_SR_RXNE));
//    return (char)(USART2->DR & 0xFF);
//}

//bool kbhitUart(void)
//{
//    return (USART2->SR & USART_SR_RXNE);
//}

//char* getsUart(USER_DATA *data)
//{
//    int count = 0;
//    char character;
//    
//    while(1)
//    {
//        character = getcUart();
//        
//        // Handle backspace (8) and delete (127)
//        if ((character == 8 || character == 127) && count > 0)
//        {
//            count = count - 1;
//            // Echo backspace
//            putcUart(8);
//            putcUart(' ');
//            putcUart(8);
//        }
//        // Handle enter (13) or newline (10)
//        else if (character == 13 || character == 10)
//        {
//            data->buffer[count] = '\0';
//            putcUart('\r');
//            putcUart('\n');
//            return data->buffer;
//        }
//        // Handle printable characters
//        else if (character >= 32 && count < MAX_CHARS - 1)
//        {
//            data->buffer[count] = character;
//            count = count + 1;
//            putcUart(character); // Echo character
//        }
//        else if (count >= MAX_CHARS - 1)
//        {
//            data->buffer[count] = '\0';
//            putcUart('\r');
//            putcUart('\n');
//            return data->buffer;
//        }
//    }
//}

//void parseFields(USER_DATA *data)
//{
//    uint8_t i;
//    data->fieldCount = 0;
//    char prevChar = 'd'; // 'd' for delimiter

//    for(i = 0; ((data->buffer[i] != '\0') && (data->fieldCount < MAX_FIELDS)); i++)
//    {
//        // Check for alphabetic characters
//        if (((data->buffer[i] >= 'A') && (data->buffer[i] <= 'Z')) || 
//            ((data->buffer[i] >= 'a') && (data->buffer[i] <= 'z')))
//        {
//            if (prevChar == 'd')
//            {
//                data->fieldPosition[data->fieldCount] = i;
//                data->fieldType[data->fieldCount] = 'a';
//                data->fieldCount = data->fieldCount + 1;
//                prevChar = 'a';
//            }
//        }
//        // Check for numeric characters
//        else if ((data->buffer[i] >= '0') && (data->buffer[i] <= '9'))
//        {
//            if (prevChar == 'd')
//            {
//                data->fieldPosition[data->fieldCount] = i;
//                data->fieldType[data->fieldCount] = 'n';
//                data->fieldCount = data->fieldCount + 1;
//                prevChar = 'n';
//            }
//        }
//        else
//        {
//            data->buffer[i] = '\0';
//            prevChar = 'd';
//        }
//    }
//}

//int compare_strings(const char *str1, const char *str2)
//{
//    while (*str1 != '\0' && *str2 != '\0')
//    {
//        if (*str1 != *str2)
//        {
//            return 0;
//        }
//        str1++;
//        str2++;
//    }
//    return (*str1 == '\0' && *str2 == '\0');
//}

//int alphabetToInteger(char* stringToConvert)
//{
//    int result = 0;
//    int j = 0;
//    int digit = 0;
//    
//    for (j = 0; stringToConvert[j] != '\0'; j++)
//    {
//        digit = stringToConvert[j] - '0';
//        result = result * 10 + digit;
//    }
//    return result;
//}

//char* getFieldString(USER_DATA *data, uint8_t fieldNumber)
//{
//    if (fieldNumber < data->fieldCount)
//    {
//        return &data->buffer[data->fieldPosition[fieldNumber]];
//    }
//    return NULL;
//}

//int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber)
//{
//    if ((fieldNumber < data->fieldCount) && (data->fieldType[fieldNumber] == 'n'))
//    {
//        return alphabetToInteger(&data->buffer[data->fieldPosition[fieldNumber]]);
//    }
//    return 0;
//}

//bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments)
//{
//    if ((compare_strings(strCommand, &data->buffer[data->fieldPosition[0]])) && 
//        (data->fieldCount - 1 >= minArguments))
//    {
//        return true;
//    }
//    return false;
//}

//int main(void)
//{
//	USER_DATA data;

//	initUart();
//	
//	while(1)
//	{
//		
//		getsUart(&data);
//    parseFields(&data);
//		
//		uint8_t i;
//    for (i = 0; i < data.fieldCount; i++)
//    {
//          putcUart(data.fieldType[i]);
//          putcUart('\t');
//          putsUart(&data.buffer[data.fieldPosition[i]]);
//          putcUart('\n');
//    }
//				
//		bool valid = false;
//		uint32_t result = 0;
//				
//		if (isCommand(&data, "set", 2))
//    {
//          int32_t add = getFieldInteger(&data, 1);
//          int32_t data69870 = getFieldInteger(&data, 2);
//          valid = true;
//              // do something with this information
//          result = add - data69870;
//          if (result > 0)
//          {
//              putsUart("pass");
//          }
//          else
//          {
//              putsUart("fail");
//          }

//    }
//		
//		if (!valid)
//		{
//			putsUart("Inavlid Comment");
//		}
//		
//	}
//	
//}
