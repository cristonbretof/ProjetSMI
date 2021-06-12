/*
 * smi_keypad.h
 *
 *  Created on: Sep 24, 2020
 *      Author: phant
 */

#ifndef SMIDRIVERS_INCLUDES_SMI_KEYPAD_H_
#define SMIDRIVERS_INCLUDES_SMI_KEYPAD_H_

#include <SMIsystem/Includes/smi_gpio.h>
#include "stm32f4xx.h"
#include "smi_lcd_display.h"


/* Defines the STM32 port used for the keypad */
#define KEYP_COLUMN_PORT       GPIOE
#define KEYP_ROW_PORT          GPIOB

/* Defines size of the keypad */
#define NUM_ROWS               4
#define NUM_COLUMNS            3

#define FIRST_ROW              11

/* Defines which pin is linked to which line on keypad */
#define ROW_1           11
#define ROW_2           12
#define ROW_3           13
#define ROW_4           14
#define COLUMN_1        4
#define COLUMN_2        5
#define COLUMN_3        6

/* Initialization function for keypad */
void KEYPAD_init(void);

/* Function used to read membrane keypad matrix */
uint8_t KEYPAD_read_matrix(void);

#endif /* SMIDRIVERS_INCLUDES_SMI_KEYPAD_H_ */
