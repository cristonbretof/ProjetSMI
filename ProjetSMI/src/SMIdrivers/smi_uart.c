#include "smi_uart.h"

/**
 * @brief Initialze USART2 module as well as IRQ for this same peripheral
 *
 * @details This function sets GPIOA pins required for transmission and
 * reception of signals for USART2. It then enables the USART2 IRQ and its
 * different modules such as Tx and Rx pins.
 *
*/
void UART_init(void)
{
	/* Enable clock register for USART2 */
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);

	/* Configure GPIOA pins for USART2 */
	SYS_gpio_init(2, GPIOA, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);
	SYS_gpio_init(3, GPIOA, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);

	/* Configure alternate function low register */
	CLEAR_BIT(GPIOA->AFR[0], 0xF000);
	CLEAR_BIT(GPIOA->AFR[0], 0x0F00);

	SET_BIT(GPIOA->AFR[0], 0x7000);
	SET_BIT(GPIOA->AFR[0], 0x0700);

	/* Set Baud rate to 19200 bauds per second */
	SET_BIT(USART2->BRR, UART_DIV);

	USART2->CR1 = 0x0004;

	/* Even parity and control enabled */
	CLEAR_BIT(USART2->CR1, USART_CR1_PS);
	SET_BIT(USART2->CR1, USART_CR1_PCE);

	/* Ensure default values for other control registers */
	USART2->CR2 = 0x0000;
	USART3->CR3 = 0x0000;

	/* Enable USART with transmission and reception features */
	SET_BIT(USART2->CR1, USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE);

	NVIC->ISER[USART2_IRQn >> 5] = (1 << (USART2_IRQn & 0x1F));
}

/**
 * @brief Decodes message coming from UART peripheral
 *
 * @details This function takes all the required message parts
 * and determines what to do with them depending on their values.
 *
 * @param[in] command The command received on the UART Rx line
 * @param[in] param   The optional parameter used for certain commands
 * @param[in] command The checksum used to determine the validity of
 * 		      the message
*/
void UART_decode(uint8_t command, uint8_t param, uint8_t checksum)
{
	uint8_t compare = command + param;
	if (!(checksum - compare))
	{
		if (command == COMMAND_LED)
		{
			if (param == 0x30)
			{
				SYS_gpio_reset_pin(LED_RED, GPIOD);
			}
			else if (param == 0x31)
			{
				SYS_gpio_set_pin(LED_RED, GPIOD);
			}
		}
		else if (command == COMMAND_ERASE_LCD_LINE)
		{
			LCD_clear_line_1();
		}
		else if (command == COMMAND_DISPLAY_CHAR)
		{
			LCD_write_data(param, DATA_MODE);
		}
	}
}
