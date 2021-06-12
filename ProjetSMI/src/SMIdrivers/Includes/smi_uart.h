#ifndef SMIDRIVERS_INCLUDES_SMI_UART_H_
#define SMIDRIVERS_INCLUDES_SMI_UART_H_

#include <SMIsystem/Includes/smi_constants.h>
#include <SMIsystem/Includes/smi_gpio.h>
#include "stm32f4xx.h"
#include "smi_lcd_display.h"

#define GPIOx_UART_SHIFT 7

#define UART_BAUD_RATE 19200
#define UART_DIV (uint16_t)(((SystemCoreClock/4)/(16.0*UART_BAUD_RATE)) * 16.0)

#define COMMAND_LED            0x41

#define	PARAM_LED_OFF          0x30
#define	PARAM_LED_ON           0x31

#define COMMAND_ERASE_LCD_LINE 0x42

#define COMMAND_DISPLAY_CHAR   0x43

void UART_init(void);
void UART_decode(uint8_t command, uint8_t param, uint8_t checksum);

#endif /* SMIDRIVERS_INCLUDES_SMI_UART_H_ */
