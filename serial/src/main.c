/*
** Made by fabien le mentec <texane@gmail.com>
** 
** Started on  Wed Nov 11 14:20:06 2009 texane
** Last update Tue Nov 17 19:46:27 2009 texane
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "serial.h"



static int wait_for_read(serial_handle_t* handle)
{
  int nfds;
  fd_set rds;

  FD_ZERO(&rds);
  FD_SET(handle->fd, &rds);

  nfds = select(handle->fd + 1, &rds, NULL, NULL, NULL);
  if (nfds != 1)
    return -1;

  return 0;
}



int main(int ac, char** av)
{
  static serial_handle_t handle = { -1, };

  {
    static serial_conf_t conf =
      { 9600, 8, SERIAL_PARITY_DISABLED, 1 };

    if (serial_open(&handle, "/dev/ttyUSB0") == -1)
      goto on_error;

    if (serial_set_conf(&handle, &conf) == -1)
      goto on_error;
  }

#if 1

#define STATE_SET_DEVICE 0
#define STATE_ENTER_PROGMODE 1
#define STATE_GET_SIGN_ON 2
#define STATE_READ_SIGN 3
#define STATE_LEAVE_PROGMODE 4
  unsigned int state = STATE_SET_DEVICE;

  while (1)
    {
#if 1

      unsigned char buf[256];
      unsigned int i;

      state = state;

      printf("press\n"); getchar();
      serial_write(&handle, "\x2a", 1, &i);
      wait_for_read(&handle);
      serial_readn(&handle, buf, sizeof(buf));

      printf("ok\n");

      for (i = 0; i < sizeof(buf); ++i)
	if (buf[i] != (unsigned char)i)
	  printf("[!] @%u\n", i);

#else
      unsigned char buf[32];
      unsigned int isize;
      unsigned int osize;

      usleep(500000);

      switch (state)
	{
	case STATE_SET_DEVICE:

	  state = STATE_ENTER_PROGMODE;

	  isize = 22;
	  buf[0] = 0x42;
	  buf[21] = 0x20;
	  osize = 2;

	  break;

	case STATE_ENTER_PROGMODE:
	  state = STATE_GET_SIGN_ON;

	  isize = 2;
	  buf[0] = 0x50;
	  buf[1] = 0x20;
	  osize = 2;

	  break;

	case STATE_GET_SIGN_ON:
	  state = STATE_READ_SIGN;

	  isize = 2;
	  buf[0] = 0x31;
	  buf[1] = 0x20;
	  osize = 9;

	  break;

	case STATE_READ_SIGN:
	  state = STATE_LEAVE_PROGMODE;

	  isize = 2;
	  buf[0] = 0x75;
	  buf[1] = 0x20;
	  osize = 5;

	  break;

	case STATE_LEAVE_PROGMODE:
	  state = STATE_SET_DEVICE;

	  isize = 2;
	  buf[0] = 0x51;
	  buf[1] = 0x20;
	  osize = 2;

	  break;
	}

      /* send command */
      {
	size_t size;
	serial_write(&handle, buf, isize, &size);
      }

      /* wait for the response */
      {
	wait_for_read(&handle);
	serial_readn(&handle, buf, osize);
      }

      /* print response */
      {
	unsigned int i;
	printf("-- state %u:\n", state);
	for (i = 0; i < osize; ++i)
	  printf(" %02x", buf[i]);
	printf("\n");
      }
#endif
    }

#else

  while (1)
    {
      unsigned char buf[3];

      {
	size_t size;
	printf("press\n"); getchar();
	serial_write(&handle, "\xaa", 1, &size);
      }

      /* read echo */
      {
	if (wait_for_read(&handle) == -1)
	  goto on_error;

	if (serial_readn(&handle, (void*)buf, sizeof(buf)) == -1)
	  goto on_error;

	printf("%02x %02x %02x\n", buf[0], buf[1], buf[2]);
      }

      /* read signature */
      {
	if (wait_for_read(&handle) == -1)
	  goto on_error;

	if (serial_readn(&handle, (void*)buf, sizeof(buf)) == -1)
	  goto on_error;

	printf("%02x %02x %02x\n", buf[0], buf[1], buf[2]);
      }
    }
#endif

 on_error:
  {
    serial_close(&handle);
  }

  return 0;
}
