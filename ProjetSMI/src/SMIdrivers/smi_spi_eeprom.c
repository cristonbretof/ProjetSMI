#include "smi_spi_eeprom.h"

/* Macro definitions to ease readability (prevents from using SYS_gpio_set_pin) */
#define CS_SELECT GPIOB->ODR &= ~BIT(SPI_CS_PIN)
#define CS_DESELECT GPIOB->ODR |= BIT(SPI_CS_PIN)

#define HOLD_ON GPIOB->ODR &= ~BIT(SPI_HOLD_PIN)
#define HOLD_OFF GPIOB->ODR |= BIT(SPI_HOLD_PIN)

#define WP_SELECT GPIOB->ODR &= ~BIT(SPI_WP_PIN)
#define WP_DESELECT GPIOB->ODR |= BIT(SPI_WP_PIN)


/**
 * @brief Writes a single byte to the TX buffer of the SPI module
 *
 * @details This function is the core of the program. It allows it
 * 			to send data (or dummy bytes) on the data register.
 *
 * @return retval The value contained in the RXNE buffer
 *
*/
static char EEPROM_write_data(char p_data)
{
	volatile int retval;

	/* Write data on buffer */
	SPI1->DR = p_data;
	while(!SPI_TXE_EMPTY);
	while(!SPI_RXNE_NOT_EMPTY);

	/* Read to clear RX buffer */
	retval = SPI_RXNE_BUFFER;

	return retval;
}

/**
 * @brief Disables writes on the data register
 *
 * @details This function sends an instruction to disable the
 * 			programs ability to write on the EEPROM.
 *
*/
static void EEPROM_write_disable(void)
{
	CS_SELECT;
	EEPROM_write_data(SPI_WRDI);
	CS_DESELECT;
}

/**
 * @brief Enables writes on the data register
 *
 * @details This function sends an instruction to enable the
 * 			programs ability to write on the EEPROM.
 *
*/
static void EEPROM_write_enable(void)
{
	CS_SELECT;
	EEPROM_write_data(SPI_WREN);
	CS_DESELECT;
}

/**
 * @brief Initializes the SPI and EEPROM modules
 *
 * @details This function sets all the required configurations for
 * 			both the SPI1 module and the EEPROMs interface with GPIOs.
 * 			It also helps maintaining the WP pin unselected which
 * 			then permits one to change the status register.
 *
*/
void EEPROM_config_mem(void)
{
	/* Initialize RCC for SPI module */
	SYS_gpio_init_rcc();

	/* Set mode of operation for chip select pin 11 (Output) */
	SYS_gpio_init(SPI_CS_PIN, GPIOB, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);

	/* Set mode of operation for HOLD and WP lines on port C */
	SYS_gpio_init(SPI_HOLD_PIN, GPIOB, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);
	SYS_gpio_init(SPI_WP_PIN, GPIOB, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_OUT);

	/* Set mode of operation for GPIO (Output) */
	SYS_gpio_init(SPI_SCK_PIN, GPIOB, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);
	SYS_gpio_init(SPI_MISO_PIN, GPIOB, GPIO_Speed_25MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);
	SYS_gpio_init(SPI_MOSI_PIN, GPIOB, GPIO_Speed_50MHz, GPIO_PuPd_NOPULL, GPIO_Mode_AF);

	/* DIsable Chip Select and WP */
	CS_DESELECT;
	WP_DESELECT;

	/* Set hold to ON */
	HOLD_ON;

	/* Initialize SPI1 module pins on port B */
	SET_BIT(GPIOB->AFR[GPIOx_LOW_AFR], BITX(GPIOx_SPI_POS, 12));
	CLEAR_BIT(GPIOB->AFR[GPIOx_LOW_AFR], BITX(GPIOx_SPI_POS, 13));

	SET_BIT(GPIOB->AFR[GPIOx_LOW_AFR], BITX(GPIOx_SPI_POS, 16));
	CLEAR_BIT(GPIOB->AFR[GPIOx_LOW_AFR], BITX(GPIOx_SPI_POS, 17));

	SET_BIT(GPIOB->AFR[GPIOx_LOW_AFR], BITX(GPIOx_SPI_POS, 20));
	CLEAR_BIT(GPIOB->AFR[GPIOx_LOW_AFR], BITX(GPIOx_SPI_POS, 21));

	/* Configure SPI1 module */
	CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE); // Disable SPI1 module

	CLEAR_BIT(SPI1->CR1, SPI_CR1_BR_0 | SPI_CR1_BR_1); // Set baudrate to fclk/16

	CLEAR_BIT(SPI1->CR1, SPI_CR1_CPOL); // Set polarity to 0
	CLEAR_BIT(SPI1->CR1, SPI_CR1_CPHA); // Set phase to 0

	CLEAR_BIT(SPI1->CR1, SPI_CR1_DFF); // Set DFF to 8-bit

	CLEAR_BIT(SPI1->CR1, SPI_CR1_LSBFIRST); // Set for MSB first

	SET_BIT(SPI1->CR1, SPI_CR1_MSTR); // Set local SPI1 as master mode
	SET_BIT(SPI1->CR2, SPI_CR2_SSOE); // Set NSS to ON to prevent Fault

	SET_BIT(SPI1->CR1, SPI_CR1_SPE); // Enable SPI1 module

	/* Set hold to OFF */
	HOLD_OFF;
}

/**
 * @brief Reads the EEPROM address wise
 *
 * @details This function reads the memory recursively. From a
 * 			specified address to an end address
 * 			(AdresseEEPROM + NbreOctets). The recursion occurs
 * 			when an address is a multiple of 64 which is the
 * 			length of a EEPROM page for the 25LC128.
 *
 * @return 0 on success, 1 if the specified number of bytes exceeds
 * 			the limits of the EEPROM
 *
*/
char LireMemoireEEPROM(unsigned int AdresseEEPROM, unsigned int NbreOctets, unsigned char *Destination)
{
	uint32_t nb_bytes_read = 0;
	uint16_t first_half = AdresseEEPROM >> 8;
	uint16_t second_half = AdresseEEPROM & 0x00FF;

	/* Detect if we require too much of the memory to read from a specific address */
	if (AdresseEEPROM + NbreOctets >= EEPROM_LAST_ADDR)
	{
		return EEPROM_ERROR;
	}

	/* Enable write operations */
	EEPROM_write_enable();

	/* Enable Chip Select */
	CS_SELECT;

	/* Wait until SPI is available */
	while(SPI_BUSY);

	/* Notify for read procedure */
	EEPROM_write_data(SPI_READ);

	/* Send AdresseEEPROM */
	EEPROM_write_data(first_half);
	EEPROM_write_data(second_half);

	while (nb_bytes_read < NbreOctets)
	{
		/* Determine position according to pages in memory */
		if (!((AdresseEEPROM + nb_bytes_read) % EEPROM_PAGE_SIZE) && nb_bytes_read > 0)
		{
			/* Disable Chip Select */
			CS_DESELECT;

			/* Disable write operations */
			EEPROM_write_disable();

			/* Wait until SPI has finished */
			while(SPI_BUSY);

			/* Persist with the read memory procedure */
			char ret_val = LireMemoireEEPROM(AdresseEEPROM + nb_bytes_read, NbreOctets - nb_bytes_read, &Destination[nb_bytes_read]);
			return ret_val;
		}

		/* Read byte */
		Destination[nb_bytes_read] = EEPROM_write_data(SPI_RXNE_BUFFER);
		nb_bytes_read++;
	}

	/* Disable Chip Select */
	CS_DESELECT;

	/* Disable write operations */
	EEPROM_write_disable();

	/* Wait until SPI has finished */
	while(SPI_BUSY);

	return EEPROM_OK;
}

/**
 * @brief Writes the EEPROM address wise
 *
 * @details This function writes the memory recursively. From a
 * 			specified address to an end address
 * 			(AdresseEEPROM + NbreOctets). The recursion occurs
 * 			when an address is a multiple of 64 which is the
 * 			length of a EEPROM page for the 25LC128.
 *
 * 			Every write attempt will be contiguous until recursion
 * 			happens. Once this is the case, this function will
 * 			specifically wait for the SPI module to be ready as it
 * 			might cancel the previous write procedure.
 *
 * @return 0 on success, 1 if the specified number of bytes exceeds
 * 			the limits of the EEPROM
 *
*/
char EcrireMemoireEEPROM(unsigned int AdresseEEPROM, unsigned int NbreOctets, unsigned char *Source)
{
	uint32_t nb_bytes_written = 0;
	uint16_t first_half = AdresseEEPROM >> 8;
	uint16_t second_half = AdresseEEPROM & 0x00FF;

	/* Detect if we require too much of the memory to read from a specific address */
	if (AdresseEEPROM + NbreOctets >= EEPROM_LAST_ADDR)
	{
		return EEPROM_ERROR;
	}

	/* Enable write operations */
	EEPROM_write_enable();

	/* Enable Chip Select */
	CS_SELECT;

	/* Wait until SPI is available */
	while(SPI_BUSY);

	/* Notify for write procedure */
	EEPROM_write_data(SPI_WRITE);

	/* Send AdresseEEPROM */
	EEPROM_write_data(first_half);
	EEPROM_write_data(second_half);

	while (nb_bytes_written < NbreOctets)
	{
		/* Determine position according to pages in memory */
		if (!((AdresseEEPROM + nb_bytes_written) % EEPROM_PAGE_SIZE) && nb_bytes_written > 0)
		{
			/* Disable Chip Select */
			CS_DESELECT;

			/* Disable write operations */
			EEPROM_write_disable();

			/* Wait until SPI is available */
			while(EEPROM_read_status() & BIT(0));

			/* Persist with the write memory procedure */
			char ret_val = EcrireMemoireEEPROM(AdresseEEPROM + nb_bytes_written, NbreOctets - nb_bytes_written, &Source[nb_bytes_written]);
			return ret_val;
		}

		/* Write byte */
		EEPROM_write_data(Source[nb_bytes_written]);

		nb_bytes_written++;
	}

	/* Disable Chip Select */
	CS_DESELECT;

	/* Disable write operations */
	EEPROM_write_disable();

	/* Wait until SPI is available */
	while(EEPROM_read_status() & BIT(0));

	return EEPROM_OK;
}

/**
 * @brief Reads the EEPROMs Status register
 *
 * @details This function reads the status register available on the 25LC128.
 *
 * @return status The EEPROMs status
 *
*/
char EEPROM_read_status(void)
{
	char status;

	/* Enable write operations */
	EEPROM_write_enable();

	/* Enable Chip Select */
	CS_SELECT;

	/* Read status register (2 words) */
	EEPROM_write_data(SPI_RDSR);
	status = EEPROM_write_data(DEADBEEF);

	/* Disable Chip Select */
	CS_DESELECT;

	/* Disable write operations */
	EEPROM_write_disable();

	return status;
}

/**
 * @brief Writes the EEPROMs Status register
 *
 * @details This function writes the status register available on
 * 			the 25LC128. It takes a protection mode (status) as a
 * 			parameter and sets it into the EEPROM to protect certain
 * 			areas of the memory array.
 *
*/
void EEPROM_write_status(uint8_t status)
{
	/* Enable write operations */
	EEPROM_write_enable();

	/* Enable Chip Select */
	CS_SELECT;

	/* Change to adequate protection mode */
	EEPROM_write_data(SPI_WRSR);
	EEPROM_write_data(status);

	/* Disable Chip Select */
	CS_DESELECT;

	/* Disable write operations */
	EEPROM_write_disable();

	/* Wait until SPI is available */
	while(EEPROM_read_status() & BIT(0));
}

