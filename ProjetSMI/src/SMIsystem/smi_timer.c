#include <SMIsystem/Includes/smi_timer.h>

/**
 * @brief Initialize a Timer
 *
 * @details This function handles the configuration of the timer. It enables it first,
 * then it sets the Auto-Reload and the Prescaler registers. Next, it Clears all previous
 * usage of interrupts on the timer and procedes to set the Count Up mode as well as the
 * enable bit for interruptions of this specific timer. It ends by Setting the proper
 * interrupt on a NVIC register and enables the TIM0 timer.
 *
*/
void SYS_timer_init(TIM_TypeDef timer, uint16_t period_ms, uint32_t direction, void *config, uint8_t irq_n)
{
	//SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM0EN);

	/* Adjust prescaler and auto-reload to match SystemCoreClock = 53.76 MHz */
	//timer->ARR = period_ms;
	//timer->PSC = SystemCoreClock/2-1;
	//TIM2->ARR = 5376;    // Multiple of SystemCoreClock to return 10000 PSC
	//TIM2->PSC = 10000/4; // Divide by 4 the PSc of 10000 (2 for APB2 configurations and 2 to get 500ms)

	/* Set initial interrupt status to 0 (OFF) */
	//CLEAR_REG(timer->SR);

	/* Activate Count Up mode */
	//CLEAR_BIT(timer->CR1, TIM_CR1_DIR);

	/* Activate interruptions on timer */
	//if (config == NULL)
	//{
	//	SET_BIT(timer->DIER, TIM_DIER_UIE);
	//}
	//else
	//{
	//	SET_BIT(timer->DIER, &config);
	//}

	/* Add IRQ number to NVIC array at specific place */
	//SET_BIT(NVIC->ISER[0], (1 << irq_n));

	/* Enable Timer (Start) */
	//SET_BIT(timer->CR1, TIM_CR1_CEN);
}
