#ifndef SMIDRIVERS_INCLUDES_SMI_SPI_EEPROM_H_
#define SMIDRIVERS_INCLUDES_SMI_SPI_EEPROM_H_

#include <SMIsystem/Includes/smi_constants.h>
#include <SMIsystem/Includes/smi_gpio.h>

/* Pin definitions for SPI module */
#define SPI_SCK_PIN   3
#define SPI_MISO_PIN  4
#define SPI_MOSI_PIN  5
#define SPI_CS_PIN    1
#define SPI_HOLD_PIN  8
#define SPI_WP_PIN    7

/* SPI module states */
#define SPI_BUSY           (SPI1->SR & SPI_SR_BSY)
#define SPI_TXE_EMPTY      (SPI1->SR & SPI_SR_TXE)
#define SPI_RXNE_NOT_EMPTY (SPI1->SR & SPI_SR_RXNE)

/* RXNE buffer access (the data contained in the register) */
#define SPI_RXNE_BUFFER (SPI1->DR & 0xFF)

/* The different instructions available on the 25LC128 */
#define SPI_READ  (uint8_t)0x03
#define SPI_WRITE (uint8_t)0x02
#define SPI_WRDI  (uint8_t)0x04
#define SPI_WREN  (uint8_t)0x06
#define SPI_RDSR  (uint8_t)0x05
#define SPI_WRSR  (uint8_t)0x01

/* Dummy byte */
#define DEADBEEF  (uint8_t)0xFF

/* Different protection modes available on 25LC128 */
#define UNPROTECTED_MODE             0b10000000
#define UPPER_QUARTER_PROTECTED_MODE 0b10000100
#define UPPER_HALF_PROTECTED_MODE    0b10001000
#define ALL_PROTECTED_MODE           0b10001100

/* Read-only bit placements for status register data */
#define WRITE_ENABLE_LATCH           0b00000010      // When WEL flag is set to 1 in status register
#define WEL_N_WIP                    0b00000011      // When WEL flag and WIP flags are set to 1 in status register

/* EEPROM dimensions */
#define EEPROM_PAGE_SIZE 64
#define EEPROM_LAST_ADDR 0x3FFF

/* Simple error codes */
#define EEPROM_ERROR 1
#define EEPROM_OK    0

/* Function declarations */
void EEPROM_config_mem(void);
char LireMemoireEEPROM(unsigned int AdresseEEPROM, unsigned int NbreOctets, unsigned char *Destination);
char EcrireMemoireEEPROM(unsigned int AdresseEEPROM, unsigned int NbreOctets, unsigned char *Source);
void EEPROM_write_status(uint8_t status);
char EEPROM_read_status(void);

#endif /* SMIDRIVERS_INCLUDES_SMI_SPI_EEPROM_H_ */
