#include "smi_gpio.h"

/**
 * @brief Function used to initialize the GPIO register, and more specifically, all
 * 		  clocks related to the used GPIO ports in the application.
*/
void SYS_gpio_init_rcc(void)
{
	/* Enable GPIOB (for SPI) and GPIOD (LEDs) peripheral RCC (clock configuration) */
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN);
}

/**
 * @brief Function used to set a pin as a GPIO.
 *
 * @details This function does not contain all the necessary initializations
 * 			to fully configure a GPIO, but the necessary steps are present.
 * 			These steps are as follows:
 * 			- GPIO direction mode
 * 			- GPIO speed of operation
 * 			- GPIO pull mode
 *
 * @param pin The pin that needs to be initialized
 * @param port A pointer to the GPIO port struct
 * @param speed The GPIO speed
 * @param pull_mode The GPIO pull mode
 * @param dir_mode The direction of the GPIO
*/
void SYS_gpio_init(uint8_t pin, GPIO_TypeDef *port, uint32_t speed, uint32_t pull_mode, uint32_t dir_mode)
{
	/* Configure direction of GPIO */
	CLEAR_BIT(port->MODER, GPIO_MODER_MODER0 << pin*2);
	SET_BIT(port->MODER, dir_mode << pin*2);

	/* Configure speed of GPIO */
	CLEAR_BIT(port->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0 << pin*2);
	SET_BIT(port->OSPEEDR, speed << pin*2);

	/* Configure Pull-up/Pull-down register of GPIO */
	CLEAR_BIT(port->PUPDR, GPIO_PUPDR_PUPDR0 << pin*2) ;
	SET_BIT(port->PUPDR, pull_mode << pin*2);
}

/**
 * @brief Function the allows a GPIO to be deinitialized (currently unused)
 *
 * @details It simply clears all the previously initialized registers the
 * 			application needed to use.
 *
 * @param pin The GPIO pin to deinitialize
 * @param port A pointer to the GPIO port struct
 *
*/
void SYS_gpio_deinit(uint8_t pin, GPIO_TypeDef *port)
{
	/* Clear all used sub-registers related to GPIO pin on specific port */
	CLEAR_BIT(port->MODER, GPIO_MODER_MODER0 << pin*2);
	CLEAR_BIT(port->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0 << pin*2);
	CLEAR_BIT(port->PUPDR, GPIO_PUPDR_PUPDR0 << pin*2);
}

/**
 * @brief Function that resets a given pin
 *
 * @details This function simply resets the output value of a given GPIO to LOW
 *
 * @param pin The GPIO pin to reset
 * @param port A pointer to the GPIO port struct
 *
*/
void SYS_gpio_reset_pin(uint8_t pin, GPIO_TypeDef *port)
{
	CLEAR_BIT(port->ODR, 0x1 << pin);
}

/**
 * @brief Function that sets a given pin
 *
 * @details This function simply sets the output value of a given GPIO to HIGH
 *
 * @param pin The GPIO pin to set
 * @param port A pointer to the GPIO port struct
 *
*/
void SYS_gpio_set_pin(uint8_t pin, GPIO_TypeDef *port)
{
	SET_BIT(port->ODR, 0x1 << pin);
}

/**
 * @brief Function that toggles a given pin
 *
 * @details This function simply toggles the output value of a given GPIO to HIGH
 *
 * @param pin The GPIO pin to toggle
 * @param port A pointer to the GPIO port struct
 *
*/
void SYS_gpio_toggle_pin(uint8_t pin, GPIO_TypeDef *port)
{
	port->ODR ^= (0x1 << pin);
}


/**
 * @brief Function that reads the current state of a GPIO
 *
 * @details This function determines what the value is on a given GPIO pin and port.
 *
 * @param pin The GPIO pin to read
 * @param port A pointer to the GPIO port struct
 *
 * @return The GPIO state (HIGH/LOW)
 *
*/
uint32_t SYS_gpio_read_pin(uint8_t pin, GPIO_TypeDef *port)
{
	return (READ_BIT(port->IDR, 0x1 << pin) == 0 ? 0:1);
}

