/*
** Made by fabien le mentec <texane@gmail.com>
** 
** Started on  Wed Nov 11 16:48:44 2009 texane
** Last update Sat Nov 14 14:56:56 2009 texane
*/



#include <stdint.h>
#include <sys/select.h>
#include "serial.h"
#include "m600.h"
#include "../../common/m600_types.h"



/* global handle */




/* exported */

int m600_initialize(const char* devname)
{

  return 0;

 on_error:

  serial_close(&handle);

  return -1;
}


void m600_cleanup(void)
{
  serial_close(&handle);
}




int m600_read_alarms(m600_alarms_t* alarms)
{
  static const m600_request_t req = M600_REQ_READ_ALARMS;
  m600_reply_t rep;
  size_t size;
  int error;

  error = serial_write(&handle, &req, sizeof(req), &size);
  if (error == -1 || size != sizeof(req))
    return -1;

  if (wait_for_read() == -1)
    return -1;

  error = serial_readn(&handle, (void*)&rep, sizeof(rep));
  if (error == -1)
    return -1;

  *alarms = rep.alarms;

  return 0;
}


int m600_read_card(unsigned int count, m600_cardfn_t on_card, void* ctx)
{
  static const m600_request_t req = M600_REQ_READ_CARD;
  m600_reply_t rep;
  size_t size;
  int error;

  for (; count; --count)
    {
      error = serial_write(&handle, &req, sizeof(req), &size);
      if (error == -1 || size != sizeof(req))
	return -1;

      if (wait_for_read() == -1)
	return -1;

      error = serial_readn(&handle, (void*)&rep, sizeof(rep));
      if (error == -1)
	return -1;

      if (on_card(rep.card_data, rep.alarms, ctx) == -1)
	break;
    }

  return 0;
}
