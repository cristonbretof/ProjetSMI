#include "smi_mp45dt02.h"

/* Current state of the audio recorder interface intialization */
/*PDM_Filter_Handler_t pdm_filter;*/

static void MEMS_gpio_init(void)
{
	/* Access and enable clocks for GPIOC and GPIOB */
	SET_BIT(RCC->AHB1ENR, RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC);

	SYS_gpio_init(MEMS_CLK_PIN, MEMS_CLK_PORT, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);
	SYS_gpio_init(MEMS_DOUT_PIN, MEMS_DOUT_PORT, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);

	SET_BIT(MEMS_CLK_PORT->AFR[1], 0x00000500);
	SET_BIT(MEMS_DOUT_PORT->AFR[0], 0x00005000);
}

/*static void MEMS_irq_init(void)
{
	 Adjust priority group for audio IRQs
	SET_BIT(SCB->AIRCR, (AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_3));

	 Enable IRQs for SPI2
	NVIC->ICER[SPI2_IRQn >> 5] = (uint32_t)(0x01 << (SPI2_IRQn & (uint8_t)0x1F));
}*/

void start_MEMS_DMA(void)
{
	SET_BIT(MEMS_DMA_STREAM->CR, DMA_SxCR_EN); // Enable DMA2 stream 0

	SET_BIT(MEMS_SPI->I2SCFGR, BIT(10)); // Enable I2S peripheral

	SET_BIT(MEMS_SPI->CR2, SPI_CR2_RXDMAEN); // Enable RX DMA request
}

/*static void MEMS_dma_deinit(void)
{
	 Disable the selected DMAy Streamx
	MEMS_DMA_STREAM->CR &= ~((uint32_t)DMA_SxCR_EN);
	MEMS_DMA_STREAM->CR  = 0;
	MEMS_DMA_STREAM->NDTR = 0;
	MEMS_DMA_STREAM->PAR  = 0;
	MEMS_DMA_STREAM->M0AR = 0;
	MEMS_DMA_STREAM->M1AR = 0;
	MEMS_DMA_STREAM->FCR = (uint32_t)0x00000021;
	MEMS_DMA->LIFCR = DMA_Stream3_IT_MASK;
}*/

static void MEMS_dma_init(void)
{
	/* Enable DMA1 clock */
	RCC->AHB1ENR |= BIT(21);

	/* Disable DMA */
	CLEAR_BIT(MEMS_DMA_STREAM->CR, DMA_SxCR_EN);

	CLEAR_BIT(MEMS_DMA_STREAM->CR, BIT(8));                	// circular mode off
	CLEAR_BIT(MEMS_DMA_STREAM->CR, (BIT(6) | BIT(7)));    	// peripheral to memory
	CLEAR_BIT(MEMS_DMA_STREAM->CR, BIT(9));					// source address not incremented
	SET_BIT(MEMS_DMA_STREAM->CR, BIT(10));                  // Destination address increments

	SET_BIT(MEMS_DMA_STREAM->CR, BIT(13));   // memory size 16 bits
	CLEAR_BIT(MEMS_DMA_STREAM->CR, BIT(14)); // memory size 16 bits

	SET_BIT(MEMS_DMA_STREAM->CR, BIT(12));   // peripheral size 16 bits
	CLEAR_BIT(MEMS_DMA_STREAM->CR, BIT(11));           // peripheral size 16 bits

	/* The peripheral is the flow controller */
	CLEAR_BIT(DMA2_Stream0->CR, BIT(5));

	/* Write to DMA1 Stream3 NDTR register (array size) */
	MEMS_DMA_STREAM->NDTR = NUM_SAMPLES;

	/* Write to DMA1 Stream3 PAR (peripheral address) */
	MEMS_DMA_STREAM->PAR = (uint32_t)&MEMS_SPI->DR;

	/* Write to DMA1 Stream3 M0AR (memory address) */
	MEMS_DMA_STREAM->M0AR = (uint32_t)RAM_BUFFER;

	/* Direct mode by disabling FIFO */
	CLEAR_BIT(MEMS_DMA_STREAM->FCR, DMA_SxFCR_DMDIS);

	/* Enable transfer */
	SET_BIT(MEMS_DMA_STREAM->CR, DMA_SxCR_TCIE);

	/* Enable IRQs for DMA */
	SET_BIT(NVIC->ISER[0], BIT(14));	// Enable stream 0 IRQ in NVIC
}

void MEMS_record_init(void)
{
	/* Initialize GPIOs and alternative functions */
	MEMS_gpio_init();

	/* Setup PLL for I2S */
	RCC_PLLI2SCmd(ENABLE);

	/* Give clock to SPI2 */
	SET_BIT(RCC->APB1ENR, BIT(14));

	/* Temporarily disable I2S peripheral */
	CLEAR_BIT(SPI2->I2SCFGR, BIT(10));

	// Master receive selected
	SET_BIT(SPI2->I2SCFGR, BIT(9) | BIT(8));
	//SPI2->I2SCFGR |= BIT10; //Activate the I2S perihperal
	// Disable Asynchron Mode

	// Select I2Sx Standard Mode
	SET_BIT(SPI2->I2SCFGR, BIT(11));

	/* Set to 16 bits */
	CLEAR_BIT(SPI2->I2SCFGR, BIT(2) | BIT(1));

	SET_BIT(SPI2->I2SCFGR, BIT(5)); //LSB justified
	CLEAR_BIT(SPI2->I2SCFGR, BIT(4)); //LSB justified

	CLEAR_BIT(SPI2->I2SCFGR, BIT(3)); //Polarity low

	CLEAR_BIT(SPI2->I2SPR, BIT(9)); //Master clk disable
	SET_BIT(SPI2->I2SPR, (uint8_t)30); //Prescaler

	MEMS_dma_init();
}

/*void MEMS_record_init(uint32_t frequency)
{
	uint16_t tmpreg = 0, i2sdiv = 2, i2sodd = 0;
	uint32_t tmp = 0, i2sclk = 0;
	uint32_t pllm = 0, plln = 0, pllr = 0;

	 Enable CRC for I2S
	RCC->APB1ENR |= RCC_AHB1ENR_CRCEN;



	 Initialize GPIOs for audio recorder
	MEMS_gpio_init();

	 Initialize IRQ module for audio recorder
	MEMS_dma_init();

	 Enable RCC clock for I2S
	RCC->APB1ENR |= RCC_APB1Periph_SPI2;

	 Reset I2S peripheral
	CLEAR_BIT(RCC->APB1RSTR, RCC_APB1Periph_SPI2);

	 Setup RCC configurations for I2S
	SET_BIT(RCC->CFGR, RCC_CFGR_I2SSRC);

	 Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits
	MEMS_SPI->I2SCFGR &= I2SCFGR_CLEAR_MASK;
	MEMS_SPI->I2SPR = 0x0002;

	 Get the I2SCFGR register value
	tmpreg = MEMS_SPI->I2SCFGR;

	 Set PLLI2S as I2S clock source
	if ((RCC->CFGR & RCC_CFGR_I2SSRC) != 0)
	{
		RCC->CFGR &= ~(uint32_t)RCC_CFGR_I2SSRC;
	}

	 Get the PLLI2SN value
	plln = (uint32_t)(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN) >> 6) & \
					  (RCC_PLLI2SCFGR_PLLI2SN >> 6));

	 Get the PLLI2SR value
	pllr = (uint32_t)(((RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR) >> 28) & \
					  (RCC_PLLI2SCFGR_PLLI2SR >> 28));

	 Get the PLLM value
	pllm = (uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLM);

	 Get the I2S source clock value
	i2sclk = (uint32_t)(((HSE_VALUE / pllm) * plln) / pllr);

	 MCLK output enabled
	tmp = (uint16_t)(((((i2sclk / 256) * 10) / frequency)) + 5);

	 Remove the flatting point
	tmp = tmp / 10;

	 Check the parity of the divider
	i2sodd = (uint16_t)(tmp & 0x0001);

	 Compute the i2sdiv prescaler
	i2sdiv = (uint16_t)((tmp - i2sodd) / 2);

	 Get the Mask for the Odd bit (SPI_I2SPR[8]) register
	i2sodd = (uint16_t) (i2sodd << 8);

	 Test if the divider is 1 or 0 or greater than 0xFF
	if ((i2sdiv < 2) || (i2sdiv > 0xFF))
	{
		 Set the default values
		i2sdiv = 2;
		i2sodd = 0;
	}

	 Write to SPIx I2SPR register the computed value
	MEMS_SPI->I2SPR = (uint16_t)((uint16_t)i2sdiv | (uint16_t)(i2sodd | I2S_MCLKOutput_Enable));

	 Configure the I2S with the SPI_InitStruct values
	tmpreg |= (uint16_t)((uint16_t)SPI_I2SCFGR_I2SMOD | (uint16_t)(I2S_Mode_MasterRx | \
				  (uint16_t)(I2S_Standard_LSB | (uint16_t)(I2S_DataFormat_16b | \
				  (uint16_t)I2S_CPOL_High))));

	 Write to SPIx I2SCFGR
	MEMS_SPI->I2SCFGR = tmpreg;
}*/

