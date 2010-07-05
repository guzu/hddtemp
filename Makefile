#####################################################

INSTALL_ROOT_DIR=

#####################################################

CC=cc
OBJS=hddtemp.o i18n.o db.o scsi.o ata.o scsicmds.o

#CFLAGS=-Wall -O2 -DARCH_I386 -D__I18N__ -fomit-frame-pointer -g
CFLAGS=-Wall -O2 -DARCH_I386 -D__I18N__ -fomit-frame-pointer
#LDFLAGS=-Wl,-static

all: hddtemp

hddtemp: i18n $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $(OBJS)
	strip hddtemp

i18n:
	xgettext -o po/hddtemp.pot -d hddtemp -C -a *.c *.h

clean:
	find . -type f -a \( -name "hddtemp" -o -name "*.o" -o -name "*~" -o -name "#*#" \) -exec rm -f {} \;
	rm -f po/hddtemp.pot

install:
	install -d /usr/local/sbin
	install hddtemp /usr/local/sbin
	install hddtemp.1 /usr/local/man/man1
	@ if [ -f hddtemp.db ]; then \
	install hddtemp.db /etc; \
	else \
	echo -e "\nWARNING:\nWARNING: don't forget to get hddtemp.db and copy it to /etc\nWARNING:"; \
	fi
	@echo
	@echo "If you want normal users to use hddtemp, you can set UID bit on hddtemp"
	@echo "(chmod u+s /usr/local/sbin/hddtemp) or alternatively use sudo (see 'man sudo'"
	@echo "and 'man sudoers')."
	@echo
