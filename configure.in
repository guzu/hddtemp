dnl Process this file with autoconf to produce a configure script.

# Initialization
AC_INIT
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

# Determine the host and build type. The target is always a PIC.
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(hddtemp, 0.3-beta15)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# append the host alias to the tools for cross compiling
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(netinet/in.h)
AC_CHECK_TYPE(in_addr_t, ,[AC_DEFINE_UNQUOTED([in_addr_t], [uint32_t], [Define to 'uint32_t' if <netinet/in.h> does not define.])], [#include <netinet/in.h>])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

dnl internationalization macros
AM_GNU_GETTEXT_VERSION
AM_GNU_GETTEXT
#AM_GNU_GETTEXT([external])

dnl reasonable guesses for where stuff is installed
dnl if we don't do that now, the config.h will use ${prefix}
dnl for DEFAULT_DATABASE_PATH.
if test "x$prefix" = "xNONE"; then
   prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE"; then
   exec_prefix=$prefix
fi

AC_MSG_CHECKING([Location of hddtemp.db])
DEFAULT_DATABASE_PATH=
#db_path=\"`eval echo ${datadir}`/misc/hddtemp.db\"
db_path=\"/usr/share/misc/hddtemp.db\"
AC_ARG_WITH(db_path,
               [  --with-db-path=PATH \
                Default location of hddtemp.db file [/usr/share/misc/hddtemp.db]],
               [  if test -d "$withval"; then
                     AC_MSG_FAILURE([new file name for db-path])
                  fi
                  DEFAULT_DATABASE_PATH="\"$withval\"" 
               ],
               [  DEFAULT_DATABASE_PATH="$db_path" ])
AC_MSG_RESULT($DEFAULT_DATABASE_PATH)
AC_DEFINE_UNQUOTED([DEFAULT_DATABASE_PATH], [$DEFAULT_DATABASE_PATH], [Default location of drive info database])

#CFLAGS="$CFLAGS -Wall -fomit-frame-pointer -Wcast-align"
CFLAGS="$CFLAGS -Wall -W -Wcast-align"

# Substitute configuration variables
AC_CONFIG_FILES([Makefile  intl/Makefile \
po/Makefile.in \
m4/Makefile \
src/Makefile \
doc/Makefile \
])
AC_OUTPUT

AC_MSG_RESULT([

hddtemp-$VERSION is now configured for $canonical_host_type

  Build:                $build
  Host:                 $host
  Source directory:     $srcdir
  Installation prefix:  $prefix
  C compiler:           $CC $CFLAGS
  
])
