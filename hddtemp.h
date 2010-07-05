/*
 * Copyright (C) 2002  Emmanuel VARAGNAT <coredump@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __HDDTEMP_H__
#define __HDDTEMP_H__

#include <time.h>
#include "db.h"

#ifdef ARCH_I386
typedef unsigned short u16;
#endif

#define MAX_ERRORMSG_SIZE      128
#define DEFAULT_ATTRIBUTE_ID   194

enum e_bustype { ERROR, BUS_UNKNOWN, BUS_ATA, BUS_SCSI, BUS_TYPE_MAX };
enum e_gettemp {
  GETTEMP_ERROR,            /* Error */
  GETTEMP_NOT_APPLICABLE,   /* */
  GETTEMP_UNKNOWN,          /* Drive is not in database */
  GETTEMP_GUESS,            /* Not in database, but something was guessed, user must
			       check that the temperature returned is correct */
  GETTEMP_KNOWN,            /* Drive appear in database */
  GETTEMP_NOSENSOR          /* Drive appear in database but is known to have no sensor */
};

struct disk {
  struct disk *            next;

  int                      fd;
  const char *             drive;
  const char *             model;
  enum e_bustype           type;
  int                      value;
  struct harddrive_entry * db_entry;

  char                     errormsg[MAX_ERRORMSG_SIZE];
  enum e_gettemp           ret;
  time_t                   last_time;
};

struct bustype {
  int (*probe)(int);
  const char *(*model)(int);
  enum e_gettemp (*get_temperature)(struct disk *);
};


extern struct bustype *   bus[BUS_TYPE_MAX];
extern char               errormsg[MAX_ERRORMSG_SIZE];
extern int                daemon_mode, debug, quiet;


#endif
