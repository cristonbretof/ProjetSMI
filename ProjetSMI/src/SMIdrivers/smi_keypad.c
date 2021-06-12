#include "smi_keypad.h"

static uint32_t previous_keyp_value = 0;

/* 2D array that holds all possible keys on a 3x4 membrane keypad */
static uint8_t key_map[NUM_ROWS][NUM_COLUMNS] =
{
		{NUMBER_1, NUMBER_2, NUMBER_3},
		{NUMBER_4, NUMBER_5, NUMBER_6},
		{NUMBER_7, NUMBER_8, NUMBER_9},
		{CHAR_STAR, NUMBER_0, CHAR_CLEAR}
};

/**
 * @brief Initialization function for keypad
 *
 * @details Makes sure every GPIO that has o do with the keypad
 * 			is initialized and set at the proper value.
 *
*/
void KEYPAD_init(void)
{
	/* Initialize all input GPIOs with internal pull-up resistors */
	SYS_gpio_init(ROW_1, KEYP_ROW_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_IN);
	SYS_gpio_init(ROW_2, KEYP_ROW_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_IN);
	SYS_gpio_init(ROW_3, KEYP_ROW_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_IN);
	SYS_gpio_init(ROW_4, KEYP_ROW_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_IN);

	/* Initialize all output GPIOs with internal pull-up resistors */
	SYS_gpio_init(COLUMN_1, KEYP_COLUMN_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_OUT);
	SYS_gpio_init(COLUMN_2, KEYP_COLUMN_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_OUT);
	SYS_gpio_init(COLUMN_3, KEYP_COLUMN_PORT, GPIO_Speed_25MHz, GPIO_PuPd_UP, GPIO_Mode_OUT);

	/* Set all column pins off (set to HIGH) */
	SYS_gpio_set_pin(COLUMN_1, KEYP_COLUMN_PORT);
	SYS_gpio_set_pin(COLUMN_2, KEYP_COLUMN_PORT);
	SYS_gpio_set_pin(COLUMN_3, KEYP_COLUMN_PORT);
}

/**
 * @brief Static function that enables a specified column
 *
 * @details This function simply takes a specified function, sets it to LOW to
 * 			follow pull-up functionality and sets every other columns to HIGH.
 *
 * @param column Column to be reset
 *
*/
static void KEYPAD_enable_column(uint8_t column)
{
	/* Ensure every column is set to HIGH before setting dedicated column to LOW */
	SYS_gpio_set_pin(COLUMN_1, KEYP_COLUMN_PORT);
	SYS_gpio_set_pin(COLUMN_2, KEYP_COLUMN_PORT);
	SYS_gpio_set_pin(COLUMN_3, KEYP_COLUMN_PORT);
	switch (column)
	{
	/* Every case ensures "column" is set to LOW */
	case COLUMN_1:
		SYS_gpio_reset_pin(column, KEYP_COLUMN_PORT);
		SYS_gpio_set_pin(COLUMN_2, KEYP_COLUMN_PORT);
		SYS_gpio_set_pin(COLUMN_3, KEYP_COLUMN_PORT);
		break;
	case COLUMN_2:
		SYS_gpio_reset_pin(column, KEYP_COLUMN_PORT);
		SYS_gpio_set_pin(COLUMN_1, KEYP_COLUMN_PORT);
		SYS_gpio_set_pin(COLUMN_3, KEYP_COLUMN_PORT);
		break;
	case COLUMN_3:
		SYS_gpio_reset_pin(column, KEYP_COLUMN_PORT);
		SYS_gpio_set_pin(COLUMN_1, KEYP_COLUMN_PORT);
		SYS_gpio_set_pin(COLUMN_2, KEYP_COLUMN_PORT);
		break;
	default:
		break;
	}
}

/**
 * @brief Static function that reads the state of every row
 *
 * @details It iterates through all the different rows of the membrane keypad
 * 			and determines the very first row that has a pressed button
 * 			(pin state to LOW)
 *
 * @return Either a row index or a placeholder value that disables the row for a loop
 *
*/
static uint8_t KEYPAD_read_rows(void)
{
	uint32_t row_value = 0;
	/* Fetch pin state on every iteration and assert if said pin is LOW */
	for (int i=0; i!=NUM_ROWS; i++)
	{
		row_value = SYS_gpio_read_pin(i+FIRST_ROW, KEYP_ROW_PORT);
		/* If row value is LOW, return the row index */
		if (!row_value)
		{
			return i;
		}
	}
	/* In any other case, return placeholder value to notify no row has been read */
	return 0xFF;
}

/**
 * @brief Static function that reads the keypad matrix once
 *
 * @details This function is the main function of this driver. It enables
 * 			consecutively all columns and then reads every row at once. If any column/row pair
 * 			contains a pressed key, the function will simply assert its redundancy
 * 			and return the pressed key contained in the BOF 2D array.
 *
 * @return Either 0 or a keypad value
 *
*/
uint8_t KEYPAD_read_matrix(void)
{
	uint8_t row = 0;
	uint8_t keyp_value = 0;

	/* Enable column 1 and read all rows */
	KEYPAD_enable_column(COLUMN_1);
	row = KEYPAD_read_rows();
	if(row != 0xFF)
	{
		keyp_value = key_map[row-1][COLUMN_1-1];
		goto end;
	}

	/* Enable column 2 and read all rows */
	KEYPAD_enable_column(COLUMN_2);
	row = KEYPAD_read_rows();
	if(row != 0xFF)
	{
		keyp_value = key_map[row-1][COLUMN_2-1];
		goto end;
	}

	/* Enable column 3 and read all rows */
	KEYPAD_enable_column(COLUMN_3);
	row = KEYPAD_read_rows();
	if(row != 0xFF)
	{
		keyp_value = key_map[row-1][COLUMN_3-1];
		goto end;
	}

	/* If, at some point, the previously pressed key is released,
	 * make sure to reinitialize the previous_keyp_value variable
	 * to zero to allow multiple iterations of the same key */
	if (keyp_value == 0)
	{
		previous_keyp_value = 0;
	}

	return 0;

	end:

	/* Redundant character detection */
	if (keyp_value == previous_keyp_value)
	{
		keyp_value = 0;
	}
	/* Change the previously saved value */
	else
	{
		previous_keyp_value = keyp_value;
	}
	return keyp_value;
}

