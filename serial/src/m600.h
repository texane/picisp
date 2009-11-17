/*
** Made by fabien le mentec <texane@gmail.com>
** 
** Started on  Wed Nov 11 17:09:47 2009 texane
** Last update Wed Nov 11 17:38:05 2009 texane
*/


#ifndef M600_H_INCLUDED
# define M600_H_INCLUDED



#include <stdint.h>
#include "../../common/m600_types.h"



typedef int (*m600_cardfn_t)(const uint16_t*, m600_alarms_t, void*);


int m600_initialize(const char*);
void m600_cleanup(void);
int m600_read_alarms(m600_alarms_t*);
int m600_read_card(unsigned int, m600_cardfn_t, void*);



#endif /* ! M600_H_INCLUDED */
