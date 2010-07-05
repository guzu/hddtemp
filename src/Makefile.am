## Process this file with automake to produce Makefile.in
CLEANFILES = *~

sbin_PROGRAMS = hddtemp

hddtemp_SOURCES = ata.c ata.h \
		  atacmds.c atacmds.h \
		  daemon.c daemon.h \
                  db.c db.h \
		  hddtemp.c hddtemp.h \
		  sata.c sata.h \
		  satacmds.c statcmds.h \
		  scsi.c scsi.h \
		  scsicmds.c scsicmds.h \
		  backtrace.c backtrace.h \
		  utf8.c utf8.h

hddtemp_CFLAGS = -Wall -W -rdynamic
#hddtemp_CFLAGS = -Wall -fomit-frame-pointer -rdynamic
hddtemp_LDADD = @LIBINTL@

localedir = $(datadir)/locale

INCLUDES = -I. -I$(srcdir) -I.. -I../intl -I$(top_srcdir)/intl
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@



