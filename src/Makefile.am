## Process this file with automake to produce Makefile.in
CLEANFILES = *~

sbin_PROGRAMS = hddtemp

hddtemp_SOURCES = ata.c ata.h \
                  db.c db.h \
		  hddtemp.c hddtemp.h \
		  scsi.c scsi.h \
		  scsicmds.c scsicmds.h \
		  utf8.c utf8.h 

hddtemp_CFLAGS = -Wall -fomit-frame-pointer
hddtemp_LDADD = @LIBINTL@

localedir = $(datadir)/locale

INCLUDES = -I. -I$(srcdir) -I.. -I../intl -I$(top_srcdir)/intl
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@



