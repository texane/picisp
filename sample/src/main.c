/* Moving LED example

   CodeVisionAVR C Compiler
   (C) 2000-2007 HP InfoTech S.R.L.
   www.hpinfotech.ro

   Chip: ATmega128
   Memory Model: SMALL
   Data Stack Size: 128 bytes

   8 LEDs are connected between the PORTC
   outputs and +5V using 1K current
   limiting resistors
   The LEDs anodes are connected to +5V
   
   On the STK500 it's only necessary to
   connect the PORTC and LEDS headers
   together with a 10-wire cable
*/

// I/O register definitions for MEGA128
#include <avr/io.h>


// quartz crystal frquency [Hz]
#define xtal 16000000
// moving LED frequency [Hz]
#define fmove 2

static void toggle_led(void)
{
}


/* unsigned char foo[300] = {0x2a, }; */


int main(void)
{
  static unsigned char led = 0;
  static unsigned int i;
  static unsigned int j;

  DDRB=0xFF;

  while (1)
    {
      PORTB = led;

      led ^= (1 << 3);

      for (j = 0; j < 0xf0; ++j)
	for (i = 0; i < 0xf0; ++i)
	  __asm__ __volatile__ ("nop\t\n");
    }

  return 0;
} 
