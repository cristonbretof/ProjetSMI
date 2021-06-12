#ifndef SMISYSTEM_INCLUDES_SMI_TIMER_H_
#define SMISYSTEM_INCLUDES_SMI_TIMER_H_

#include "stm32f4xx.h"

void SYS_timer_init(TIM_TypeDef timer, uint16_t period_ms, uint32_t direction, void *config, uint8_t irq_n);


#endif /* SMISYSTEM_INCLUDES_SMI_TIMER_H_ */
