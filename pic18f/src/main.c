/*
** Made by fabien le mentec <texane@gmail.com>
** 
** Started on  Sun Sep 20 14:08:30 2009 texane
** Last update Tue Nov 17 19:52:04 2009 texane
*/



#include <pic18fregs.h>
#include "config.h"
#include "int.h"
#include "osc.h"
#include "serial.h"
#include "types.h"
#include "stk500.h"



/* timer1 routines */

static void tmr0_start_1s(void)
{
#if 0

  /* use 1:256 prescaler. assumed the insn clock being
     8mhz, then Fosc/4 is 2mhz, giving a timer freq of
     7,8125khz frequency. */

  unsigned int countdown = 0xffff - 7812;

#if 0
  {
    /* prescaler */
    T0CONbits.PSA = 0;
    T0CONbits.T0PS0 = 1;
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 1;

    /* inc on low to high */
    T0CONbits.T0SE = 0;

    /* use insn clock (Fosc/4) */
    T0CONbits.T0CS = 0;    

    /* 16 bits counter */
    T0CONbits.T08BIT = 0;
  }
#else
  {
    T0CON = 7;
  }
#endif

  /* writing to the register must occur
     in this order since writing the low
     part make the real update. */

  TMR0H = (countdown >> 8) & 0xff;
  TMR0L = (countdown >> 0) & 0xff;

#endif
}


static void tmr0_stop(void)
{
#if 0

  T0CON = 0;
  INTCONbits.TMR0IF = 0;

#endif
}


static int tmr0_get_if(void)
{
#if 0
  return TMR0bits.IF;
#else
  return 0;
#endif
}


/* serial helper routines */

static int read_uint8(unsigned char* c, unsigned char do_timeout)
{
  int res = 0;

  if (do_timeout)
    tmr0_start_1s();

  while (serial_pop_fifo(c) == -1)
    {
      /* wait for serial data */
      osc_set_power(OSC_PMODE_IDLE);

      if (do_timeout && tmr0_get_if())
	{
	  res = -1;
	  break;
	}
    }

  if (do_timeout)
    tmr0_stop();

  return res;
}


static unsigned int read_uint16(void)
{
  unsigned char buf[2];

  read_uint8(&buf[0], 0);
  read_uint8(&buf[1], 0);

  /* little endian */
  return ((unsigned int)buf[1] << 8) | (unsigned int)buf[0];
}


static void read_buf(unsigned char* buf, unsigned int size)
{
  for (; size; --size, ++buf)
    read_uint8(buf, 0);
}



/* main */

#if 0

int main(void)
{
  unsigned char buf[256];
  unsigned int i;

  osc_setup();
  int_setup();
  serial_setup();

  while (1)
    {
      read_uint8(buf, 0);

      for (i = 0; i < sizeof(buf); ++i)
	buf[i] = (unsigned char)i;

      serial_write(buf, sizeof(buf));
    }

 on_error:
  while (1)
    ;

  return 0;
}

#else /* stk500 */

int main(void)
{
  stk500_error_t error;
  unsigned int isize;
  unsigned int osize;
  unsigned char do_timeout;
  unsigned char buf[STK500_BUF_MAX_SIZE];

  osc_setup();
  int_setup();
  serial_setup();
  stk500_setup();

  while (1)
    {
      /* process new command */

      error = STK500_ERROR_MORE_DATA;

      do_timeout = 0;

      isize = 0;
      osize = 0;

      while (error == STK500_ERROR_MORE_DATA)
	{
	  if (read_uint8(buf + isize, do_timeout) == -1)
	    {
	      /* has timeouted */
	      stk500_timeout(buf, isize, &osize);
	      error = STK500_ERROR_SUCCESS;
	      break;
	    }

	  do_timeout = 1;

	  ++isize;

	  error = stk500_process(buf, isize, &osize);
	}

      if ((error == STK500_ERROR_SUCCESS) && (osize))
	serial_write(buf, osize);
    }
}

#endif
