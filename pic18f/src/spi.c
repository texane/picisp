/*
** Made by fabien le mentec <texane@gmail.com>
** 
** Started on  Sat Nov 14 17:18:15 2009 texane
** Last update Sun Nov 22 17:43:05 2009 texane
*/



#include <pic18fregs.h>
#include "types.h"
#include "osc.h"
#include "spi.h"



/* pinouts */

#define SPI_PIN_MOSI PORTAbits.RA0 /* PE0, serial data in */
#define SPI_TRIS_MOSI TRISAbits.TRISA0

#define SPI_PIN_RESET PORTAbits.RA1 /* reset */
#define SPI_TRIS_RESET TRISAbits.TRISA1

#define SPI_PIN_SCK PORTAbits.RA2 /* PB1, serial clock */
#define SPI_TRIS_SCK TRISAbits.TRISA2

#define SPI_PIN_MISO PORTAbits.RA3 /* PE1, serial data out */
#define SPI_TRIS_MISO TRISAbits.TRISA3


/* wait routine */

static void wait_msecs(unsigned int msecs)
{
  /* atmega128 specs, table 128 */
#define Twd_fuse 5
#define Twd_flash 5
#define Twd_eeprom 10
#define Twd_erase 10

#define Fosc_target 16000000
#define Fosc_prog OSC_FREQ

#define MSECS_TO_INSNS(T) ((Fosc_prog * (T)) / 1000)

  unsigned int i;

  for (i = MSECS_TO_INSNS(msecs); i; --i)
    ;
}


/* spi io routine */

static unsigned char write_read_byte(unsigned char ibuf)
{
  /* one bit each transmitted from the prog
     to the target and vice versa at each
     clock cycle. clock is driven by the
     programmer. when writing data to the
     target, data is clocked on the rising
     edge of SCK when reading, clocked on
     falling edge.
   */

  unsigned char obuf;
  int i;

  for (obuf = 0, i = 7; i >= 0; --i)
    {
      SPI_PIN_MOSI = (ibuf >> i) & 1;
      SPI_PIN_SCK = 1;

      __asm nop __endasm;
      __asm nop __endasm;

      obuf |= (SPI_PIN_MISO << i);

      SPI_PIN_SCK = 0;
    }

  return obuf;
}


/* global cmd buffer */

#define SPI_CMD_BYTE_COUNT 4

static unsigned char cmd[SPI_CMD_BYTE_COUNT];


#define write_read_cmd(A, B, C, D)		\
do {						\
  cmd[0] = write_read_byte(A);			\
  cmd[1] = write_read_byte(B);			\
  cmd[2] = write_read_byte(C);			\
  cmd[3] = write_read_byte(D);			\
} while (0)


/* exported */

void spi_setup(void)
{
  /* have to do configure the ports as digital
     io, otherwise they will be in analog mode
   */

  ADCON0 = 0;
  ADCON1 = 0xf;
  ADCON2 = 0;

  SPI_TRIS_MOSI = 0;
  SPI_TRIS_MISO = 1;
  SPI_TRIS_SCK = 0;
  SPI_TRIS_RESET = 0;

  SPI_PIN_SCK = 0;
  SPI_PIN_RESET = 1;
}


int spi_start(void)
{
  unsigned int n;
  unsigned int i;

  for (n = 0; n < 5; ++n)
    {
      /* enter programming mode */
      SPI_PIN_RESET = 0;

      /* wait at least 20ms (0xffff will do) */
      for (i = 0; i < 0xffff; ++i)
	;

      /* programming enable command */
      write_read_cmd(0xac, 0x53, 0, 0);

      /* did the device reply back */
      if (cmd[2] == 0x53)
	return 0;

      /* leave programming mode */
      SPI_PIN_RESET = 1;
    }

  return -1;
}


void spi_stop(void)
{
  /* leave programming mode */
  SPI_PIN_RESET = 1;
}


void spi_chip_erase(void)
{
  /* erase eeprom and flash */
  write_read_cmd(0xac, 0x80, 0, 0);

  wait_msecs(Twd_erase);
}


uint16_t spi_read_program(uint16_t addr)
{
  uint16_t value = 0;

#define LOW_BYTE(W) (uint8_t)(((W) >> 0) & 0xff)
#define HIGH_BYTE(W) (uint8_t)(((W) >> 8) & 0xff)

  /* read low byte */
  write_read_cmd(0x20, HIGH_BYTE(addr), LOW_BYTE(addr), 0);
  value = cmd[3];

  /* read high byte */
  write_read_cmd(0x28, HIGH_BYTE(addr), LOW_BYTE(addr), 0);
  value |= (uint16_t)cmd[3] << 8;

  return value;
}


void spi_load_program_page(uint8_t addr, uint16_t value)
{
  /* write value in program memory page at addr */

  /* data low must be loaded before data high is applied */
  write_read_cmd(0x40, 0, addr, LOW_BYTE(value));
  write_read_cmd(0x48, 0, addr, HIGH_BYTE(value));
}


void spi_write_program_page(uint16_t addr)
{
  /* write program memory page at addr */
  write_read_cmd(0x4c, HIGH_BYTE(addr), LOW_BYTE(addr), 0);

  wait_msecs(Twd_flash);
}


uint8_t spi_read_eeprom(uint16_t addr)
{
  write_read_cmd(0x82, HIGH_BYTE(addr), LOW_BYTE(addr), 0);
  return cmd[3];
}


void spi_write_eeprom(uint16_t addr, uint8_t value)
{
  /* write eeprom at addr */
  write_read_cmd(0xc0, HIGH_BYTE(addr), LOW_BYTE(addr), value);

  wait_msecs(Twd_eeprom);
}


uint8_t spi_read_lock_bits(void)
{
  write_read_cmd(0x58, 0, 0, 0);

  return cmd[3];
}


void spi_write_lock_bits(uint8_t value)
{
  write_read_cmd(0xac, 0xe0, 0, (3 << 6) | value);

  wait_msecs(Twd_fuse);
}


void spi_read_signature(uint8_t* buf)
{
  /* addr the byte address */

  unsigned int i;

  for (i = 0; i < SPI_SIGNATURE_BYTE_COUNT; ++i)
    {
      write_read_cmd(0x30, 0, i, 0);
      buf[i] = cmd[3];
    }
}


void spi_write_fuse_bits(uint8_t value)
{
  write_read_cmd(0xac, 0xa0, 0, value);

  wait_msecs(Twd_fuse);
}


void spi_write_fuse_high_bits(uint8_t value)
{
  write_read_cmd(0xac, 0xa8, 0, value);

  wait_msecs(Twd_fuse);
}


void spi_write_extended_fuse_bits(uint8_t value)
{
  write_read_cmd(0xac, 0xa4, 0, value);

  wait_msecs(Twd_fuse);
}


uint8_t spi_read_fuse_bits(void)
{
  write_read_cmd(0x50, 0x00, 0, 0);

  return cmd[3];
}


uint8_t spi_read_extended_fuse_bits(void)
{
  write_read_cmd(0x50, 0x08, 0, 0);

  return cmd[3];
}


uint8_t spi_read_fuse_high_bits(void)
{
  write_read_cmd(0x58, 0x08, 0, 0);

  return cmd[3];
}


void spi_read_calibration_bytes(uint8_t* buf)
{
  unsigned int i;

  for (i = 0; i < SPI_CALIBRATION_BYTE_COUNT; ++i)
    {
      write_read_cmd(0x38, 0, i, 0);
      buf[i] = cmd[3];
    }
}


uint8_t spi_write_read(uint8_t* buf)
{
  write_read_cmd(buf[0], buf[1], buf[2], buf[3]);

  if (buf[0] == 0xac)
    {
      if (buf[1] == 0x80)
	wait_msecs(Twd_erase);
      else if (buf[1] == 0xe0)
	wait_msecs(Twd_fuse);
      else if (buf[1] == 0xa0)
	wait_msecs(Twd_fuse);
      else if (buf[1] == 0xa8)
	wait_msecs(Twd_fuse);
      else if (buf[1] == 0xa4)
	wait_msecs(Twd_fuse);
    }
  else if (buf[0] == 0x4c)
    {
      wait_msecs(Twd_flash);
    }
  else if (buf[0] == 0xc0)
    {
      wait_msecs(Twd_eeprom);
    }

  return cmd[3];
}
