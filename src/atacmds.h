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

#ifndef ATACMDS_H_
#define ATACMDS_H_

int ata_enable_smart(int device);
int ata_get_smart_values(int device, unsigned char* buff);
unsigned char* ata_search_temperature(const unsigned char* smart_data, int attribute_id);
enum e_powermode ata_get_powermode(int device);
int ata_get_packet (int device);

#endif
