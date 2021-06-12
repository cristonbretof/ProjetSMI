/*
 * smi_mp45dt02.h
 *
 *  Created on: Nov 21, 2020
 *      Author: phant
 */

#ifndef SMIDRIVERS_INCLUDES_SMI_MP45DT02_H_
#define SMIDRIVERS_INCLUDES_SMI_MP45DT02_H_

#include <stdio.h>

#include "stm32f4xx.h"
#include "smi_gpio.h"
#include "smi_cs43l22.h"
#include "smi_constants.h"

#include "stm32f4xx_dma.h"
#include "stm32f4xx_it.h"

/* Low AFR buffer index */
#define GPIOx_LOW_AFR 0

// Audio input configurations
#define MEMS_SPI                    SPI2

#define MEMS_CLK_PIN                10
#define MEMS_CLK_PORT               GPIOB
#define MEMS_CLK_AFR_POS            5

#define MEMS_DOUT_PIN               3
#define MEMS_DOUT_PORT              GPIOC

#define MEMS_DMA					DMA1
#define MEMS_DMA_STREAM             DMA1_Stream3
#define MEMS_DMA_CHANNEL            DMA_Channel_0
#define MEMS_DMA_IRQ_Handler		DMA1_Stream3_IRQHandler
#define MEMS_DMA_PDATA_LEN          DMA_PeripheralDataSize_HalfWord
#define MEMS_DMA_MDATA_LEN          DMA_MemoryDataSize_HalfWord
#define MEMS_DMA_MODE               DMA_Mode_Normal

#define MEMS_DMA_IRQN               DMA1_Stream3_IRQn
#define MEMS_IRQ_PRIO               0x0F

// Audio general configurations
#define AUDIO_DATA_SIZE             2   // 16-bits audio data size
#define DMA_MAX_SZE                 0xFFFF

// Audio status definition
#define AUDIO_OK                    0
#define AUDIO_ERROR                 1
#define AUDIO_TIMEOUT               2

// AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2)
/* Audio recording frequency in Hz */
#define REC_FREQ                          8000

/* PDM buffer input size */
#define INTERNAL_BUFF_SIZE      64

/* PCM buffer output size */
#define PCM_OUT_SIZE            16

// PDM initialization values
#define LOW_PASS_Freq
#define HIGH_PASS_Freq
#define DECIMATION_FACTOR                     50

// PCM buffer output size
#define CHANNEL_DEMUX_MASK                    0x55

#define NUM_SAMPLES                           100000

#define DMA_Stream0_IT_MASK     (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                           DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                           DMA_LISR_TCIF0)

#define I2SCFGR_CLEAR_MASK      ((uint16_t)0xF040)
#define DMA_Stream3_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 22)
#define AIRCR_VECTKEY_MASK      ((uint32_t)0x05FA0000)

extern __IO uint16_t AudioInVolume;

void MEMS_record_init(void);
void start_MEMS_DMA(void);

#endif /* SMIDRIVERS_INCLUDES_SMI_MP45DT02_H_ */
