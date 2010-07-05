#!/usr/bin/perl
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

use strict;
my $id=$ARGV[0] || die "USAGE: $0 <field_id>";
my $tmpdir = ".";

opendir(DIR, $tmpdir) || die "can't opendir $tmpdir: $!";
my @logs = grep { /^hddtemp-\d+$/ && -f "$tmpdir/$_" } readdir(DIR);
closedir DIR;


foreach my $log (@logs) {
# field(12)        = 58
	open(HDLOG, "<$tmpdir/$log") || die "can't open file $tmpdir/log: $!";
	if ( $log =~ /^hddtemp\-(\d+)$/ ) {
		my $stamp = $1;
		while (<HDLOG>) {
			if ( $_ =~ /^field\($id\)\s+= (\d+)$/o ) {
				print "$stamp $1\n";
				last;
			}
		}
	}
	close(HDLOG);
}
