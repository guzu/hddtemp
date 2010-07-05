#!/bin/sh
#
# Small script that provides the temperature of all local hard disks
# 
# This program is copyright 2004 by Javier Fernandez-Sanguino <jfs@debian.org>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# For more information please see
#  http://www.gnu.org/licenses/licenses.html#GPL

#set -e

PATH=/sbin:/usr/sbin:/bin:/usr/bin

if [ ! -x "`which hddtemp`" ] ; then
	echo "ERROR: Hddtemp is not available"
	exit 1
fi


# NOTE, you could actually change this to 
# ls /dev/hd? /dev/sd?
# but then you would need to remove the cruft of non-existant drives...
df -l |cut -f 1 -d " " |grep /dev/ |sed -e 's/[[:digit:]]$//g' |sort -u |
while read drive; do
	# TODO: 
	case "$drive" in
	    /dev/sd*|/dev/hd*)
	    # NOTE: Scsi devices might be error-prone, since many non-HDD
	    # devices uses SCSI or SCSI emulation (CD-ROMs, USB mass storage..)
		hddtemp $drive
		;;
	    /dev/md*)
	    # TODO: it could actually look somewher for the information
	    # of the disks that make up the raid, maybe looking it up
	    # at /proc/mdstat
	    	echo "RAID devices currently not supported ($drive)"
		;;
	    /dev/vg*)
	    	echo "LVM devices currently not supported ($drive)"
		;;
	    /dev/cdrom*|/dev/fd*)
	    # Some common non-HD elements which might be mounted,
	    # we skip these
	    	;;
	    *)
	    	echo "Unknown drive currently not supported ($drive)"
		;;
	esac
done

exit 0
