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

#ifndef __DB_H__
#define __DB_H__

#ifndef DEFAULT_DATABASE_PATH
#define DEFAULT_DATABASE_PATH  "/usr/share/misc/hddtemp.db"
#endif

struct harddrive_entry {
  const char             *regexp;
  short int              attribute_id;
  const char             *description;
  unsigned char          unit;
  struct harddrive_entry *next;
};

extern struct harddrive_entry   *supported_drives;

struct harddrive_entry *is_a_supported_drive(const char* model);
void load_database(const char* filename);

#endif
