/*
 * Copyright (C) 2002  Emmanuel VARAGNAT <hddtemp@guzu.net>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __DB_H__
#define __DB_H__

#ifndef DEFAULT_DATABASE_PATH
#define DEFAULT_DATABASE_PATH  "/usr/share/misc/hddtemp.db"
#endif

struct harddrive_entry {
  char                   *regexp;
  short int              attribute_id;
  char                   *description;
  unsigned char          unit;
  struct harddrive_entry *next;
};

struct harddrive_entry *is_a_supported_drive(const char* model);
void display_supported_drives();
void load_database(const char* filename);
void free_database(void);

#endif
