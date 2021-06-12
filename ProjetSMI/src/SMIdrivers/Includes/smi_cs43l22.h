#ifndef SMIDRIVERS_INCLUDES_SMI_CS43L22_H_
#define SMIDRIVERS_INCLUDES_SMI_CS43L22_H_

#include <stdint.h>
#include "stm32f4xx.h"
#include "smi_mp45dt02.h"

// Audio output configurations
#define JACK_SPI                    SPI3

#define DMA1_Stream7                ((DMA_Stream_TypeDef *) DMA1_Stream7_BASE)
#define DMA_Stream7_IT_MASK         (uint32_t)(DMA_Stream3_IT_MASK | (uint32_t)0x20000000)

#define JACK_WS_PIN                 4
#define JACK_SCK_PIN                10
#define JACK_SD_PIN                 12
#define JACK_MCK_PIN                7

#define JACK_SCK_SD_GPIO_PORT       GPIOC
#define JACK_WS_GPIO_PORT           GPIOA
#define JACK_MCK_GPIO_PORT          GPIOC

#define JACK_DMA                    DMA1
#define JACK_DMA_STREAM             DMA1_Stream7
#define JACK_DMA_CHANNEL            DMA_Channel_0
#define JACK_DMA_PERIPH_DATA_SIZE   DMA_PeripheralDataSize_HalfWord
#define JACK_DMA_MEM_DATA_SIZE      DMA_MemoryDataSize_HalfWord

#define JACK_DMA_IRQ                DMA1_Stream7_IRQn
#define JACK_IRQHandler             DMA1_Stream7_IRQHandler
#define JACK_IRQ_PRIO               0x0E

#endif /* SMIDRIVERS_INCLUDES_SMI_CS43L22_H_ */
