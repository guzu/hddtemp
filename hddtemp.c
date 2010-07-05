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

/*
 * Added some SCSI support: Frederic LOCHON <lochon@roulaise.net>
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
#include <linux/hdreg.h>

#include "i18n.h"
#include "ata.h"
#include "scsi.h"
#include "db.h"
#include "hddtemp.h"

#define F_to_C(val) (int)(((double)(val)-32.0)/1.8)
#define C_to_F(val) (int)(((double)(val)*1.8)+32)

#define HDDTEMP_VERSION        "0.3 beta6"
#define PORT_NUMBER            7634
#define SEPARATOR              '|'
#define DELAY                  60.0

char *             database_path = DEFAULT_DATABASE_PATH;
long               portnum;
in_addr_t          listen_addr;
char               separator = SEPARATOR;
int                sk_serv;

struct bustype *   bus[BUS_TYPE_MAX];
int                daemon_mode, debug, quiet, numeric;

/*******************************************************
 *******************************************************/

static void init_bus_types() {
  bus[BUS_ATA] = &ata_bus;
  bus[BUS_SCSI] = &scsi_bus;
}

/*******************************************************
 *******************************************************/


static void show_supported_drives() {
  unsigned char           *tabs, *line;
  struct harddrive_entry  *p;
  int                     max, len;

  max = 0;
  for(p = supported_drives; p; p = p->next) {
    len = strlen(p->regexp);
    if(len > max)
      max = len;
  }
  len = max/8 + 1;
  tabs = malloc(len+1);
  memset(tabs, '\t', len);
  tabs[len] = '\0';

  len = (max/8 + 1) * 8;
  line = malloc(len+1);
  memset(line, '-', len);
  line[len] = '\0';
  
  printf("\n"
	 "Regexp%s| Value | Description\n"
	 "------%s---------------------\n", tabs, line);

  for(p = supported_drives; p; p = p->next) {
    len = strlen(p->regexp);
    printf("%s%s| %5d | %s\n",
	   p->regexp,
	   tabs+(len/8),
	   p->attribute_id,
	   p->description);
  }  
  printf("\n");
}


static enum e_bustype probe_bus_type(struct disk *dsk) {
  
  if(bus[BUS_ATA]->probe(dsk->fd))
    return BUS_ATA;
  else if(bus[BUS_SCSI]->probe(dsk->fd))
    return BUS_SCSI;
  else
    return BUS_UNKNOWN;
}

/*
static int get_smart_threshold_values(int fd, unsigned char* buff) {
  unsigned char cmd[516] = { WIN_SMART, 0, SMART_READ_THRESHOLDS, 1 };
  int ret;

  ret = ioctl(fd, HDIO_DRIVE_CMD, cmd);
  if(ret)
    return ret;

  memcpy(buff, cmd+4, 512);
  return 0;
}
*/



static void display_temperature(struct disk *dsk) {
  enum e_gettemp ret;
  char *degree;

  if(dsk->type != ERROR && debug ) {
    printf("\n================= hddtemp %s ==================\n"
	   "Model: %s\n\n", HDDTEMP_VERSION, dsk->model);
    /*    return;*/
  }

  if(dsk->type == ERROR
     || bus[dsk->type]->get_temperature == NULL
     || (ret = bus[dsk->type]->get_temperature(dsk)) == GETTEMP_ERROR )
  {
    fprintf(stderr, "%s: %s\n", dsk->drive, dsk->errormsg);
    return;
  }

  if(debug)
    return;

  degree = degree_sign();
  switch(ret) {
  case GETTEMP_ERROR:
    /* see above */
    break;
  case GETTEMP_NOT_APPLICABLE:
    printf("%s: %s: %s\n", dsk->drive, dsk->model, dsk->errormsg);    
    break;
  case GETTEMP_UNKNOWN:
    if(!quiet)
      printf(_("WARNING: Drive %s doesn't seem to have a temperature sensor.\n"
	       "WARNING: This doesn't mean it hasn't got one.\n"
	       "WARNING: If you are sure it has one, please contact me (coredump@free.fr).\n"
	       "WARNING: See --help, --debug and --drivebase options.\n"), dsk->drive);
    printf(_("%s: %s:  no sensor\n"), dsk->drive, dsk->model);
    break;
  case GETTEMP_GUESS:
    if(!quiet)
      printf(_("WARNING: Drive %s doesn't appear in the database of supported drives\n"
	       "WARNING: But using a common value, it reports something.\n"
	       "WARNING: Note that the temperature shown could be wrong.\n"
	       "WARNING: See --help, --debug and --drivebase options.\n"
	       "WARNING: And don't forget you can add your drive to hddtemp.db\n"), dsk->drive);
    printf(_("%s: %s:  %d%sC or %sF\n"), dsk->drive, dsk->model, dsk->value, degree, degree);
    break;
  case GETTEMP_KNOWN:
    if (! numeric)
       printf("%s: %s: %d%sC\n",
              dsk->drive,
              dsk->model,
              (dsk->db_entry->unit == 'C') ? dsk->value : F_to_C(dsk->value), degree);
    else
       printf("%d\n", (dsk->db_entry->unit == 'C') ? dsk->value : F_to_C(dsk->value));
    break;
  case GETTEMP_NOSENSOR:
    printf(_("%s: %s:  known drive, but it doesn't have a temperature sensor.\n"), dsk->drive, dsk->model);
    break;
  default:
    fprintf(stderr, _("ERROR: %s: %s: unknown returned status\n"), dsk->drive, dsk->model);
    break;
  }
  free(degree);
}


void do_direct_mode(struct disk *ldisks) {
  struct disk *dsk;

  for(dsk = ldisks; dsk; dsk = dsk->next) {
    display_temperature(dsk);
  }
  
  if(debug) {
    printf(_("\n"
	     "If one of the field value seems to match the temperature, you can\n"
	     "send me a report so I can add it to the database of supported drives\n"
	     "and/or you can do it yourself by adding an entry in hddtemp.db and send\n"
	     "me the updated hddtemp.db.\n"
	     "\n"
	     "Don't forget to compare your value(s) with those in the database.\n"
	     "(see option --drivebase)\n"));
  }
}


void daemon_stop(int n) {
  close(sk_serv);
}


void do_daemon_mode(struct disk *ldisks) {
  struct sockaddr_in saddr;
  struct disk *      dsk;
  int                cfd;
  /*char               sepsep[2] = { separator, separator };*/
  int                i;
  struct tm *        time_st;
  int                on = 1;

  if((sk_serv = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
 
  if((sk_serv = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
   
  // Allow local port reuse in TIME_WAIT
  if (setsockopt(sk_serv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
    perror("setsockopt");	  
    exit(1);
  }	  
	  
  memset(&saddr, 0, sizeof(struct sockaddr_in));
  /*    
	if((he = gethostbyaddr("127.0.0.1")) == NULL) {
	perror("gethostbyname");
	exit(1);
	}
  */
  
  saddr.sin_addr.s_addr = listen_addr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(portnum);
  
  if(bind(sk_serv, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
    perror("bind");
    exit(1);
  }
  
  switch(fork()) {
  case -1:
    perror("fork");
    exit(2);
    break;
  case 0:
    break;
  default:
    exit(0);
  }

  /* close standard input and output */
  for(i = 0; i < 3; i++) {
    if(i != sk_serv)
      close(i);
  }

  /* redirect signals */
  for(i = 0; i <= _NSIG; i++) {
    signal(i, daemon_stop);
  }

  /* timer initialisation */
  for(dsk = ldisks; dsk; dsk = dsk->next) {
    time(&dsk->last_time);
    time_st = gmtime(&dsk->last_time);
    time_st->tm_year -= 1;
    dsk->last_time = mktime(time_st);
  }

  /* start to serve */
  while(1) {
    struct sockaddr_in caddr;
    socklen_t sz_caddr;
    
    if(listen(sk_serv, 5) == -1) {
      break;
    }
    
    memset(&caddr, 0, sizeof(struct sockaddr_in));
    sz_caddr = sizeof(struct sockaddr_in);
    if((cfd = accept(sk_serv, (struct sockaddr *)&caddr, &sz_caddr)) == -1) {
      close(cfd);
      break;
    }
    
    for(dsk = ldisks; dsk; dsk = dsk->next) {
      char msg[128];
      int n;

      if(difftime(time(NULL), dsk->last_time) > DELAY) {
	dsk->value = -1;

	if(dsk->type == ERROR)
	  dsk->ret = GETTEMP_ERROR;
	else
	  dsk->ret = bus[dsk->type]->get_temperature(dsk);

	time(&dsk->last_time);
      }

      switch(dsk->ret) {
      case GETTEMP_NOT_APPLICABLE:
	n = snprintf(msg, sizeof(msg), "%s%c%s%c%s%c%c",
		     dsk->drive, separator,
		     dsk->model, separator,
		     "NA",       separator,
		     '*');
	break;
      case GETTEMP_GUESS:
      case GETTEMP_UNKNOWN:
	n = snprintf(msg, sizeof(msg), "%s%c%s%c%s%c%c",
		     dsk->drive, separator,
		     dsk->model, separator,
		     "UNK",     separator,
		     '*');
	break;
      case GETTEMP_KNOWN:
	n = snprintf(msg, sizeof(msg), "%s%c%s%c%d%c%c",
		     dsk->drive,                                                     separator,
		     dsk->model,                                                     separator,
		     (dsk->db_entry->unit == 'C') ? dsk->value : F_to_C(dsk->value), separator,
		     dsk->db_entry->unit);
	break;
      case GETTEMP_NOSENSOR:
	n = snprintf(msg, sizeof(msg), "%s%c%s%c%s%c%c",
		     dsk->drive, separator,
		     dsk->model, separator,
		     "NOS",      separator,
		     '*');
	break;
      case GETTEMP_ERROR:
      default:
	n = snprintf(msg, sizeof(msg), "%s%c%s%c%s%c%c",
		     dsk->drive,                        separator,
		     (dsk->model) ? dsk->model : "???", separator,
		     "ERR",                             separator,
		     '*');
	break;
      }
      
      write(cfd,&separator, 1);
      write(cfd, &msg, n);
      write(cfd,&separator, 1);
    }
    close(cfd);
  }
  close(sk_serv);
}


int main(int argc, char* argv[]) {
  int           i, c, lindex = 0;
  int           show_db;
  struct disk * ldisks;

  show_db = debug = numeric = quiet = 0;
  portnum = PORT_NUMBER;
  listen_addr = htonl(INADDR_ANY);

  /* Parse command line */
  optind = 0;
  while (1) {
    static struct option long_options[] = {
      {"help",      0, NULL, 'h'},
      {"quiet",     0, NULL, 'q'},
      {"daemon",    0, NULL, 'd'},
      {"drivebase", 0, NULL, 'b'},
      {"debug",     0, NULL, 'D'},
      {"file",      1, NULL, 'f'},
      {"listen",    1, NULL, 'l'},
      {"version",   0, NULL, 'v'},
      {"port",      1, NULL, 'p'},
      {"separator", 1, NULL, 's'},
      {"numeric",   0, NULL, 'n'},
      {0, 0, 0, 0}
    };
 
    c = getopt_long (argc, argv, "bDdf:l:hp:qs:vn", long_options, &lindex);
    if (c == -1)
      break;
    
    switch (c) {
      case 'q':
	quiet = 1;
	break;
      case 'b':
	show_db = 1;
	break;
      case 'd':
	daemon_mode = 1;
	break;
      case 'D':
	debug = 1;
	break;
      case 'f':
	database_path = optarg;
	break;
      case 's':
	separator = optarg[0];
	if(separator == '\0') {
	  fprintf(stderr, _("ERROR: invalid separator.\n"));
	  exit(1);
	}
	break;
      case 'p':
	{
	  char *end = NULL;

	  portnum = strtol(optarg, &end, 10);

	  if(errno == ERANGE || end == optarg || *end != '\0' || portnum < 1) {
	    fprintf(stderr, "ERROR: invalid port number.\n");
	    exit(1);
	  }
	}
	break;
      case 'l':
	listen_addr = inet_addr(optarg);
	if (listen_addr == -1)
	{
          fprintf(stderr, "ERROR: invalid listen address.\n");
	  exit(1);
        }	  
	break;
      case '?':
      case 'h':
	printf(_(" Usage: hddtemp [OPTIONS] DISK1 [DISK2]...\n"
		 "\n"
		 "   hddtemp displays the temperature of drives supplied in argument.\n"
		 "   Drives must support S.M.A.R.T.\n"
		 "\n"
		 "  -b   --drivebase   :  display database file content that allow hddtemp to\n"
		 "                        recognize supported drives.\n"
		 "  -D   --debug       :  display various S.M.A.R.T. fields and their values.\n"
		 "                        Useful to find a value that seems to match the\n"
		 "                        temperature and/or to send me a report.\n"
		 "                        (done for every drive supplied).\n"
		 "  -d   --daemon      :  run hddtemp in daemon mode (port %d by default)\n"
		 "  -f   --file=FILE   :  specify database file to use.\n"
		 "  -l   --listen=addr :  listen on a specific interface.\n"
                 "  -n   --numeric     :  print only the temperature.\n"
		 "  -p   --port=#      :  port to listen to (in daemon mode).\n"
		 "  -s   --separator=C :  separator to use between fields (in daemon mode).\n"
		 "  -q   --quiet       :  do not check if the drive is supported.\n"
		 "  -v   --version     :  display hddtemp version number.\n"
		 "\n"
		 "Report bugs or new drives to <coredump@free.fr>.\n"),
	       PORT_NUMBER);
      case 'v':
	printf("hddtemp version %s\n", HDDTEMP_VERSION);
	exit(0);
	break; 
      case 'n':
        numeric = 1;
        break;
      default:
	exit(1);
      }
  }
  
  if(show_db) {
     load_database(database_path);
     show_supported_drives();
     exit(0);
  }
  
  if(geteuid()) {
    fprintf(stderr,
	    _("  ERROR: You must be root to run the command,\n"
	      "  ERROR: or the root must set the suid bit for the executable.\n"));
    exit(1);
  }

  if(argc - optind <= 0) {
    fprintf(stderr, _("Too few arguments: you must specify one drive, at least.\n"));
    exit(1);
  }

  if(debug) {
    /*    argc = optind + 1;*/
    quiet = 1;
  }

  if(debug && daemon_mode) {
    fprintf(stderr, _("ERROR: can't use --debug and --daemon options together.\n"));
    exit(1);
  }

  load_database(database_path);
  init_bus_types();

  /* collect disks informations */
  ldisks = NULL;
  for(i = argc - 1; i >= optind; i--) {
    struct disk *dsk = (struct disk *) malloc(sizeof(struct disk));

    dsk->drive = argv[i];
    dsk->next = ldisks;
    ldisks = dsk;    

    errno = 0;
    dsk->errormsg[0] = '\0';
    if( (dsk->fd = open(dsk->drive, O_RDONLY | O_NONBLOCK)) < 0) {
      snprintf(dsk->errormsg, MAX_ERRORMSG_SIZE, "open: %s\n", strerror(errno));
      dsk->type = ERROR;
      continue;
    } else {
      dsk->type = probe_bus_type(dsk);
    }

    if(dsk->type == BUS_UNKNOWN) {
      fprintf(stderr, "ERROR: %s: can't determine bus type (or this bus type is unknown)\n", dsk->drive);

      ldisks = dsk->next;
      free(dsk);
      continue;
    }

    dsk->model = bus[dsk->type]->model(dsk->fd);
    dsk->db_entry = is_a_supported_drive(dsk->model);
    dsk->value = -1;
  }

  if(daemon_mode) {
    /*    cleanup_database(ldisks);*/
    do_daemon_mode(ldisks);
  }
  else {
    do_direct_mode(ldisks);
  }

  return 0;
}
