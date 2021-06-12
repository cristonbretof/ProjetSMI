/* Includes */
#include <stdbool.h>
#include "circ_buffer.h"
#include "smi_constants.h"
#include "smi_mp45dt02.h"
#include "smi_gpio.h"

#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

// DEBUG CONSTANTS
#define DEBUG_PIN_RECORD   14
#define DEBUG_PIN_PLAYBACK 15
#define DEBUG_PIN_IDLE     12

#define FALL               0
#define RISE               1

#define BUTTON_PIN         0

typedef enum {
	INPUT = 0,
	OUTPUT
} state_t;

static bool PROCESSING = false;

void DMA1_Stream3_IRQHandler(void)
{
	MEMS_DMA->LIFCR = DMA_LIFCR_CTCIF3;  // clear the interrupt flag
	SYS_gpio_set_pin(DEBUG_PIN_PLAYBACK, GPIOD);
	SYS_gpio_reset_pin(DEBUG_PIN_RECORD, GPIOD);
}

void SPI2_IRQHandler(void)
{
	volatile uint16_t dataRx;
	volatile uint8_t status = SPI_GetITStatus(SPI2, SPI_I2S_IT_RXNE);
	if (status != RESET)
	{
		dataRx = SPI_I2S_ReceiveData(SPI2);
		dataRx = dataRx;
		SPI2->SR |= BIT(0);
	}
	SYS_gpio_set_pin(DEBUG_PIN_IDLE, GPIOD);
}

void EXTI0_IRQHandler(void)
{
	/* Reset EXT interrupt line */
	EXTI->PR = EXTI_Line0;

	/* If button is pressed and we are in Idle */
	if (!PROCESSING && SYS_gpio_read_pin(BUTTON_PIN, GPIOA) == RISE)
	{
		/* Set value to play-back at start of program */
		SYS_gpio_set_pin(DEBUG_PIN_RECORD, GPIOD);
		SYS_gpio_reset_pin(DEBUG_PIN_IDLE, GPIOD);
		PROCESSING = true;

		/* Start routine for I2S MEMS recording */
		start_MEMS_DMA();
	}
}

void BUTTON_init(void)
{
	/* Initialize button GPIO */
	SYS_gpio_init(BUTTON_PIN, GPIOA, GPIO_Speed_25MHz, GPIO_PuPd_DOWN, GPIO_Mode_IN);

	/* Give clock for SYSCFG */
	RCC->APB2ENR |= BIT(14);

	 /* Put the EXTI0 4 first bits to 0 to select port A source */
	SYSCFG->EXTICR[0] &= 0xFFFFFFF0;

	/* Clear EXTI line configuration */
	EXTI->IMR |= EXTI_Line0;
	EXTI->EMR &= ~EXTI_Line0;

	/* Enable rising edge and falling edge */
	EXTI->RTSR |= EXTI_Line0;
/*	EXTI->FTSR |= EXTI_Line0;*/

	/* Enable interrupt */
	NVIC->ISER[EXTI0_IRQn >> 5] = (uint32_t)(1 << (EXTI0_IRQn & (uint32_t)0x1F));
}

/**
 * @brief Initialize SysTick Timer
 *
 * @details This function only really sets the Count Down value for The SysTick
 * LOAD register and enables interruptions on said Timer.
 *
*/
void SysTick_init(uint32_t frequency)
{
	// SysTick_CTRL_CLKSOURCE_Msk |	// Use SystemCoreClock as clock source
	SysTick->CTRL = (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
	SysTick->LOAD = HSE_VALUE/(frequency)-1;
}

int main(void)
{
	// BEGIN SYSTEM CONFIGURATION
	/* Initialize button and debug pin RCC */
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN);

	/* Initialize recording module */
	MEMS_record_init();

	/* Initialize the Systick Timer */
	SysTick_init(1000000);

	/* Setup debug LEDs to determine if success of program or failure of program */
	SYS_gpio_init(DEBUG_PIN_IDLE, GPIOD, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(DEBUG_PIN_RECORD, GPIOD, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(DEBUG_PIN_PLAYBACK, GPIOD, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);

	/* Initialize button interrupt */
	BUTTON_init();

	/* Set value to play-back at start of program */
	SYS_gpio_set_pin(DEBUG_PIN_IDLE, GPIOD);
	// END SYSTEM CONFIGURATION

	while (1)
	{
		// Nothing
	}
}
