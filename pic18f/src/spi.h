/*
** Made by fabien le mentec <texane@gmail.com>
** 
** Started on  Sat Nov 14 17:18:24 2009 texane
** Last update Tue Nov 17 18:14:57 2009 texane
*/



#ifndef SPI_H_INCLUDED
# define SPI_H_INCLUDED



#include "types.h"


/* atmega128 device signature */

#define SPI_SIGNATURE_ATMEGA128 "\x1e\x97\x02"
#define SPI_SIGNATURE_BYTE_COUNT 3
#define SPI_CALIBRATION_BYTE_COUNT 4


/* exported */

void spi_setup(void);
int spi_start(void);
void spi_stop(void);

void spi_chip_erase(void);
uint16_t spi_read_program(uint16_t);
void spi_load_program_page(uint8_t, uint16_t);
void spi_write_program_page(uint16_t);
uint8_t spi_read_eeprom(uint16_t);
void spi_write_eeprom(uint16_t, uint8_t);
uint8_t spi_read_lock_bits(void);
void spi_write_lock_bits(uint8_t);
void spi_read_signature(uint8_t*);
void spi_write_fuse_bits(uint8_t);
void spi_write_fuse_high_bits(uint8_t);
void spi_write_extended_fuse_bits(uint8_t);
uint8_t spi_read_fuse_bits(void);
uint8_t spi_read_extended_fuse_bits(void);
uint8_t spi_read_fuse_high_bits(void);
void spi_read_calibration_bytes(uint8_t*);
uint8_t spi_write_read(uint8_t*);



#endif /* ! SPI_H_INCLUDED */
