/*
  gcc -W -Wall -o crash contribs/crash.c
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
