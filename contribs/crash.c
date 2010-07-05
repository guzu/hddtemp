/*
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

/*
 *  gcc -W -Wall -o crash contribs/crash.c
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
  struct sockaddr_in saddr;
  struct hostent *he;

  int s;
  
  s = socket (AF_INET, SOCK_STREAM, 0);
  
  memset (&saddr, 0, sizeof (struct sockaddr_in));
  
  he = gethostbyname ("127.0.0.1");
  
  memcpy (&(saddr.sin_addr.s_addr), he->h_addr, he->h_length);
  
  saddr.sin_family = AF_INET;
  saddr.sin_port   = htons (7634);
  
  connect (s, (struct sockaddr *) &saddr, (socklen_t) sizeof (saddr));
  
  return 0;
}
