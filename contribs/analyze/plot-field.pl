#!/usr/bin/perl

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
