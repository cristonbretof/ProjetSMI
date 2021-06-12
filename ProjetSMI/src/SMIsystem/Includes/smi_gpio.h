#ifndef SMISYSTEM_INCLUDES_SMI_GPIO_H_
#define SMISYSTEM_INCLUDES_SMI_GPIO_H_

#include "stm32f4xx.h"

/* Low AFR buffer index */
#define GPIOx_LOW_AFR 0

/* SPI AFR value to place at bit X */
#define GPIOx_SPI_POS  5

#define LED_GREEN        12		// Pin number 12
#define LED_ORANGE       13		// Pin number 13
#define LED_RED          14   // Pin number 14 on Discovery board
#define LED_BLUE         15   // Pin number 15 on Discovery board

/* Function declarations */
void SYS_gpio_init_rcc(void);

void SYS_gpio_init(uint8_t pin, GPIO_TypeDef *port, uint32_t speed, uint32_t pull_mode, uint32_t dir_mode);
void SYS_gpio_deinit(uint8_t pin, GPIO_TypeDef *port);

void SYS_gpio_reset_pin(uint8_t pin, GPIO_TypeDef *port);
uint32_t SYS_gpio_read_pin(uint8_t pin, GPIO_TypeDef *port);
void SYS_gpio_set_pin(uint8_t pin, GPIO_TypeDef *port);
void SYS_gpio_toggle_pin(uint8_t pin, GPIO_TypeDef *port);


#endif /* SMISYSTEM_INCLUDES_SMI_GPIO_H_ */
