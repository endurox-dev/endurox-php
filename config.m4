dnl $Id: config.m4,v 1.4 2001/07/24 03:08:55 bfoddy Exp $

ndrx_config_ubf_flag="0"
ndrx_config_ubf32_flag="0"

AC_MSG_CHECKING(for Endurox support)
AC_ARG_WITH(endurox,
[  --with-endurox[=DIR]  Include Endurox support.  DIR is the Endurox home
                          directory, defaults to $NDRXDIR.],
[
  if test "$withval" != "no"; then
    AC_MSG_RESULT(yes)
    if test -z "$ENDUROX_LIBS"; then
      PHP_EXTENSION(endurox)
    fi
    AC_DEFINE(HAVE_ENDUROX,1,[ ])
    if test "$withval" = "yes"; then
      ENDUROX_INCDIR=$NDRXDIR/include
      ENDUROX_LIBDIR=$NDRXDIR/lib
    else
      ENDUROX_INCDIR=$withval/include
      ENDUROX_LIBDIR=$withval/lib
    fi


	    AC_DEFINE(NDRX_UBF32, 1, [ ])
	    AC_MSG_RESULT(yes)
		ndrx_config_ubf32_flag="1"


	    AC_DEFINE(NDRX_UBF, 1, [ ])
	    AC_MSG_RESULT(yes)
		ndrx_config_ubf_flag="1"

	
	PHP_ADD_INCLUDE($ENDUROX_INCDIR)
	PHP_ADD_LIBPATH($ENDUROX_LIBDIR)
	PHP_ADD_LIBRARY(wsc)
	PHP_ADD_LIBRARY(buft)
	PHP_ADD_LIBRARY(nwi)
	PHP_ADD_LIBRARY(nws)
	PHP_ADD_LIBRARY(gp)
	PHP_ADD_LIBRARY(ubf)
	PHP_ADD_LIBRARY(ubf32)
	ENDUROX_LIBS="-lwsc -lbuft -lnwi -lnws -lubf -lubf32 -lgp"
    old_LDFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS -L$ENDUROX_LIBDIR"
    LDFLAGS=$old_LDFLAGS
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
