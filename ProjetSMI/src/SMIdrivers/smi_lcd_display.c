#include <SMIsystem/Includes/smi_gpio.h>
#include "smi_lcd_display.h"

static LCD_cursor_t cursor_position;

static uint8_t LCD_data_pins[N_DATA_PINS] = {0,1,2,3,6,7,8,9};
static uint8_t LCD_digits[10] = {NUMBER_0, NUMBER_1, NUMBER_2, NUMBER_3, NUMBER_4,
								NUMBER_5, NUMBER_6, NUMBER_7, NUMBER_8, NUMBER_9};
static uint8_t LCD_remainders[5] = {0, 0, 0, 0, 0};

/**
 * @brief Function that clears line 1 of LCD display
 *
 * @details This function iterates through the whole line and sets an empty char (0x20)
 *
*/
void LCD_clear_line_1(void)
{
	/* Set number of bits to 0 on line 1 */
	cursor_position.x = 0;

	/* Switch to line 1 if not already there */
	LCD_write_data((SET_DDRAM_ADD | BIT(6)), INSTRUCTION_MODE);

	for (int i=0; i!=LENGTH_DATA; i++)
	{
		/* Clean all dots on this specific character */
		LCD_write_data((WRITE_DATA_TO_RAM | 0x20), DATA_MODE);

		/* Wait 10ms */
		SYS_waitUS(10000);
	}
	/* Set the vertical cursor position to 1 and the horizontal cursor position to 0 */
	cursor_position.x = 0;
	cursor_position.y = 1;
}

/**
 * @brief Function that writes the first line (SMI header)
 *
 * @details This function resets the SMI header and then returns to line 1
 *
*/
void LCD_write_smi_header(void)
{
	/* Switch to line 0 if not already there */
	LCD_write_data(RETURN_HOME, INSTRUCTION_MODE);
	cursor_position.x = 0;
	cursor_position.y = 0;

	/* Three instructions to write SMI */
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_S), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_M), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_I), DATA_MODE);

	/* Underscore character */
	LCD_write_data((WRITE_DATA_TO_RAM | 0x5F), DATA_MODE);

	/* My initials */
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_C), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_B), DATA_MODE);

	/* Return to line 1 */
	LCD_write_data((SET_DDRAM_ADD | BIT(6)), INSTRUCTION_MODE);
	cursor_position.y = 1;
}

/**
 * @brief Function that writes the first line (SMI header)
 *
 * @details This function resets the SMI header and then returns to line 1
 *
*/
void LCD_write_header(uint64_t count)
{
	int iter = 0;
	uint8_t temp_x = cursor_position.x;

	/* Switch to line 0 if not already there */
	LCD_write_data(RETURN_HOME, INSTRUCTION_MODE);
	cursor_position.x = 0;
	cursor_position.y = 0;

	/* Three instructions to write SMI */
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_C), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LETTER_B), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | 0x20), DATA_MODE);

	/* This method is long and kind of rough for the CPU */
	while (count > 0 && count < MAX_LCD_SECONDS)
	{
		LCD_remainders[iter] = count % 10;
		count = (unsigned int)(count/10);
		iter++;
	}

	/* Write sequence of bytes */
	LCD_write_data((WRITE_DATA_TO_RAM | LCD_digits[LCD_remainders[4]]), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LCD_digits[LCD_remainders[3]]), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LCD_digits[LCD_remainders[2]]), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LCD_digits[LCD_remainders[1]]), DATA_MODE);
	LCD_write_data((WRITE_DATA_TO_RAM | LCD_digits[LCD_remainders[0]]), DATA_MODE);

	/* Return to line 1 */
	cursor_position.y = 1;
	cursor_position.x = temp_x;
	LCD_write_data((SET_DDRAM_ADD | (uint8_t)(64 + cursor_position.x)), INSTRUCTION_MODE);
}

/**
 * @brief Function that configures all necessary GPIOs
 *
 * @details This function sets all LCD pins as output and without pull resistors.
 * It then starts the initialization procedure for the LCD display.
 *
*/
void LCD_init(void)
{
	/* Initialize Data pins */
	SYS_gpio_init(0, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(1, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(2, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(3, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(6, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(7, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(8, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(9, LCD_DATA_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);

	/* Initialize all pins related to control registers of LCD display */
	SYS_gpio_init(CTRL_E, LCD_CTRL_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(CTRL_RS, LCD_CTRL_PORT, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);

	/* Turn all IOs to LOW */
	SYS_gpio_reset_pin(0, LCD_DATA_PORT);
	SYS_gpio_reset_pin(1, LCD_DATA_PORT);
	SYS_gpio_reset_pin(2, LCD_DATA_PORT);
	SYS_gpio_reset_pin(3, LCD_DATA_PORT);
	SYS_gpio_reset_pin(6, LCD_DATA_PORT);
	SYS_gpio_reset_pin(7, LCD_DATA_PORT);
	SYS_gpio_reset_pin(8, LCD_DATA_PORT);
	SYS_gpio_reset_pin(9, LCD_DATA_PORT);
	SYS_gpio_reset_pin(CTRL_E, LCD_CTRL_PORT);
	SYS_gpio_reset_pin(CTRL_RS, LCD_CTRL_PORT);

	/* Set initial cursor position */
	cursor_position.x = 0;
	cursor_position.y = 0;

	/* Function set instruction (8bit transfer and 2 line display) */
	LCD_write_data((FUNCTION_SET | BIT(4)), INSTRUCTION_MODE);

	/* First timeout of 10ms */
	SYS_waitUS(10000);

	/* Function set instruction (8bit transfer and 2 line display) */
	LCD_write_data((FUNCTION_SET | BIT(4)), INSTRUCTION_MODE);

	/* Second timeout of 200us */
	SYS_waitUS(10000);

	/* Function set instruction (8bit transfer and 2 line display) */
	LCD_write_data((FUNCTION_SET | BIT(4)), INSTRUCTION_MODE);

	/* Last wait time of initiation procedure */
	SYS_waitUS(10000);
	/* END OF INITIALIZATION */

	/* Function set instruction (8bit transfer and 2 line display) */
	LCD_write_data((FUNCTION_SET | BIT(4) | BIT(3)), INSTRUCTION_MODE);
	SYS_waitUS(10000);

	/* Clear display first */
	LCD_write_data(CLEAR_DISPLAY, INSTRUCTION_MODE);
	SYS_waitUS(10000);

	/* Set display to off state */
	LCD_write_data(DISPLAY_ON_OFF | BIT(2) | BIT(1) | BIT(0), INSTRUCTION_MODE);
	SYS_waitUS(10000);

	/* Clear display first */
	LCD_write_data(CLEAR_DISPLAY, INSTRUCTION_MODE);
	SYS_waitUS(10000);

	/* Enable right shift mode for cursor */
	LCD_write_data((ENTRY_MODE_SET | BIT(1)), INSTRUCTION_MODE);
}

/**
 * @brief Function that writes a character given on the LCD board
 *
 * @details This function receives a hexadecimal value that represents a
 * 			character for the LCD board. It also handles both instructions
 * 			and character data.
 *
 * @param data The character (or instruction) that has to be sent to LCD board
 * @param mode_select Determines if its an instruction or a character
 *
*/
void LCD_write_data(uint16_t data, uint8_t mode_select)
{
	if (cursor_position.x == LENGTH_DATA &&
			data != CHAR_CLEAR)
	{
		return;
	}

	/* Set mode of operation (instructions or data) */
	switch (mode_select)
	{
	/* Instruction mode */
	case INSTRUCTION_MODE:
		SYS_gpio_reset_pin(CTRL_E, LCD_CTRL_PORT);
		SYS_gpio_reset_pin(CTRL_RS, LCD_CTRL_PORT);
		break;
	/* Data mode */
	case DATA_MODE:
		SYS_gpio_reset_pin(CTRL_E, LCD_CTRL_PORT);
		SYS_gpio_set_pin(CTRL_RS, LCD_CTRL_PORT);
		cursor_position.x++;
		break;
	/* Is hopefully never reached */
	default:
		break;
	}

	/* Set data bits for LCD */
	for (int i=0; i!=N_DATA_PINS; i++)
	{
		if (data & (1 << i))
		{
			SYS_gpio_set_pin(LCD_data_pins[i], LCD_DATA_PORT);
		}
		else
		{
			SYS_gpio_reset_pin(LCD_data_pins[i], LCD_DATA_PORT);
		}
	}

	/* Start write procedure */
	SYS_gpio_set_pin(CTRL_E, LCD_CTRL_PORT);
	/* Wait for no less than 1us */
	SYS_waitUS(1000);
	SYS_gpio_reset_pin(CTRL_E, LCD_CTRL_PORT);
	SYS_waitUS(1000);
}
