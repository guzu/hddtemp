/*
 * Copyright (C) 2003  Emmanuel VARAGNAT <hddtemp@guzu.net>
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

#include <features.h>

#if defined(__i386__) && defined(__GLIBC__)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/ptrace.h>
#include <signal.h>
#include <bits/sigcontext.h>
#include <asm/ucontext.h>
#include <execinfo.h>
#include <sys/param.h>

#define MAX_BTSIZE 64

void backtrace_handler(int n, siginfo_t *ist, void *extra) {
  static struct ucontext *puc;
  static void *btinfo[MAX_BTSIZE];
  static char **strings;
  static size_t btsize;
  static int i;
  static unsigned int old_eip, old_ebp;
  static char *strerr = "???";
  static FILE *fstrm;

  if(write(2, "\n", 1) == -1) { /* surely in daemon mode */
    char btpath[MAXPATHLEN];
    snprintf(btpath, MAXPATHLEN, "/tmp/hddtemp.backtrace.%d.XXXXXX", getpid());

    if( (fstrm = fdopen(mkstemp(btpath), "w")) == NULL)
      return;
  }
  else
    fstrm = stderr;

#define SIC_CASE(c) case c: strerr = #c

  puc = (struct ucontext *)extra;
  switch(n) {
  case SIGSEGV:
    switch(ist->si_code) {
      SIC_CASE(SEGV_MAPERR);
      SIC_CASE(SEGV_ACCERR);
    }
    fprintf(fstrm, "=== SEGFAULT (%s) : invalid access to %p, in 0x%08lx\n",
	    strerr,
	    ist->si_addr,
	    puc->uc_mcontext.eip);
    break;
  case SIGILL:
    switch(ist->si_code) {
      SIC_CASE(ILL_ILLOPC);
      SIC_CASE(ILL_ILLOPN);
      SIC_CASE(ILL_ILLADR);
      SIC_CASE(ILL_ILLTRP);
      SIC_CASE(ILL_PRVOPC);
      SIC_CASE(ILL_PRVREG);
      SIC_CASE(ILL_COPROC);
      SIC_CASE(ILL_BADSTK);
    }
    fprintf(fstrm, "=== ILLEGAL INSTR (%s) : invalid access to %p, in 0x%08lx\n",
	    strerr,
	    ist->si_addr,
	    puc->uc_mcontext.eip);
    break;
  case SIGBUS:
    switch(ist->si_code) {
      SIC_CASE(BUS_ADRALN);
      SIC_CASE(BUS_ADRERR);
      SIC_CASE(BUS_OBJERR);
    }
    fprintf(fstrm, "=== BUS ERROR (%p) : invalid access to %p, in 0x%08lx\n",
	    strerr,
	    ist->si_addr,
	    puc->uc_mcontext.eip);
    break;
  }
  fflush(fstrm);

#undef CASE

  old_eip = *(unsigned int*)((void*)&n-4);
  old_ebp = *(unsigned int*)((void*)&n-8);
  *(unsigned int*)((void*)&n-4) = puc->uc_mcontext.eip;
  *(unsigned int*)((void*)&n-8) = puc->uc_mcontext.ebp;    
  btsize = backtrace(btinfo, MAX_BTSIZE);
  *(unsigned int*)((void*)&n-4) = old_eip;
  *(unsigned int*)((void*)&n-8) = old_ebp;

  strings = backtrace_symbols(btinfo, btsize);

  for(i = 1;
      i < btsize - 2;
      i++)
    fprintf(fstrm, " #%d: %s\n", i, strings[i]);
  fflush(fstrm);

  free(strings);
  fclose(fstrm);
}

void backtrace_sigsegv(void) {
  struct sigaction sigst;

  sigst.sa_sigaction = backtrace_handler;
  sigemptyset(&sigst.sa_mask);
  sigaddset(&sigst.sa_mask, SIGILL);
  sigaddset(&sigst.sa_mask, SIGBUS);
  sigst.sa_flags = SA_SIGINFO | SA_ONESHOT;

  if(sigaction(SIGSEGV, &sigst, NULL) == -1)
    perror("sigaction");
}

void backtrace_sigbus(void) {
  struct sigaction sigst;

  sigst.sa_sigaction = backtrace_handler;
  sigemptyset(&sigst.sa_mask);
  sigaddset(&sigst.sa_mask, SIGILL);
  sigaddset(&sigst.sa_mask, SIGSEGV);
  sigst.sa_flags = SA_SIGINFO | SA_ONESHOT;

  if(sigaction(SIGBUS, &sigst, NULL) == -1)
    perror("sigaction");
}

void backtrace_sigill(void) {
  struct sigaction sigst;

  sigst.sa_sigaction = backtrace_handler;
  sigemptyset(&sigst.sa_mask);
  sigaddset(&sigst.sa_mask, SIGSEGV);
  sigaddset(&sigst.sa_mask, SIGBUS);
  sigst.sa_flags = SA_SIGINFO | SA_ONESHOT;

  if(sigaction(SIGILL, &sigst, NULL) == -1)
    perror("sigaction");
}

#else

#warning "Backtracing is not supported for this architexture."

void backtrace_handler(void) { }
void backtrace_sigsegv(void) { }
void backtrace_sigbus(void) { }
void backtrace_sigill(void) { }

#endif
