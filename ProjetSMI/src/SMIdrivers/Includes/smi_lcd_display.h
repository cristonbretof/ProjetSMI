#ifndef SMIDRIVERS_INCLUDES_SMI_LCD_DISPLAY_H_
#define SMIDRIVERS_INCLUDES_SMI_LCD_DISPLAY_H_

#include <SMIsystem/Includes/smi_constants.h>
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

#include "smi_lcd_display.h"

/* Defines the STM32 ports used for LCD display */
#define LCD_DATA_PORT   GPIOD
#define LCD_CTRL_PORT   GPIOC

#define N_DATA_PINS     8
#define LENGTH_DATA     22

/* Defines control pins for LCD display */
#define CTRL_E           8
#define CTRL_RS          9

/* Maximum number to be shown on LCD */
#define MAX_LCD_SECONDS  99999

/* Defines instructions for LCD*/
#define CLEAR_DISPLAY       0x001
#define RETURN_HOME         0x002
#define ENTRY_MODE_SET      0x004
#define DISPLAY_ON_OFF      0x008
#define CURSOR_DISP_SHIFT   0x010
#define FUNCTION_SET        0x020
#define SET_CGRAM_ADD       0x040
#define SET_DDRAM_ADD       0x080
#define READ_BUSY_FLAG      0x100
#define WRITE_DATA_TO_RAM   0x200
#define READ_DATA_FROM_RAM  0x400

/* Characters used for alphabetic letters LCD Display (A when i == 0) */
#define LETTER_BASE         0x41
#define LETTER(i)           (uint8_t)(LETTER_BASE + i)

#define LETTER_S            0x53
#define LETTER_M            0x4D
#define LETTER_I            0x49
#define LETTER_C            0x43
#define LETTER_B            0x42

/* Characters used for numbers LCD Display (0 when i == 0) */
#define NUMBER_BASE         0x30
#define NUMBER(i)           (uint8_t)(NUMBER_BASE + i)

#define NUMBER_0            0x30
#define NUMBER_1            0x31
#define NUMBER_2            0x32
#define NUMBER_3            0x33
#define NUMBER_4            0x34
#define NUMBER_5            0x35
#define NUMBER_6            0x36
#define NUMBER_7            0x37
#define NUMBER_8            0x38
#define NUMBER_9            0x39

#define CHAR_STAR           0x2A
#define CHAR_CLEAR          0x20

/* Mode of operation (instruction or data) */
#define INSTRUCTION_MODE    1
#define DATA_MODE           0

typedef struct {
	uint8_t x;
	uint8_t y;
} LCD_cursor_t;

/* Initialization function for LCD display */
void LCD_init(void);

void LCD_write_data(uint16_t data, uint8_t mode_select);
void LCD_write_smi_header(void);
void LCD_write_header(uint64_t count);

void LCD_clear_line_1(void);

#endif /* SMIDRIVERS_INCLUDES_SMI_LCD_DISPLAY_H_ */
