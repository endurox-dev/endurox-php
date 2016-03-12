dnl $Id: config.m4,v 1.4 2001/07/24 03:08:55 bfoddy Exp $

tux_config_fml_flag="0"
tux_config_fml32_flag="0"

AC_MSG_CHECKING(for Tuxedo support)
AC_ARG_WITH(tuxedo,
[  --with-tuxedo[=DIR]  Include Tuxedo support.  DIR is the Tuxedo home
                          directory, defaults to $TUXDIR.],
[
  if test "$withval" != "no"; then
    AC_MSG_RESULT(yes)
    if test -z "$TUXEDO_LIBS"; then
      PHP_EXTENSION(tuxedo)
    fi
    AC_DEFINE(HAVE_TUXEDO,1,[ ])
    if test "$withval" = "yes"; then
      TUXEDO_INCDIR=$TUXDIR/include
      TUXEDO_LIBDIR=$TUXDIR/lib
    else
      TUXEDO_INCDIR=$withval/include
      TUXEDO_LIBDIR=$withval/lib
    fi


	    AC_DEFINE(TUX_FML32, 1, [ ])
	    AC_MSG_RESULT(yes)
		tux_config_fml32_flag="1"


	    AC_DEFINE(TUX_FML, 1, [ ])
	    AC_MSG_RESULT(yes)
		tux_config_fml_flag="1"

	
	PHP_ADD_INCLUDE($TUXEDO_INCDIR)
	PHP_ADD_LIBPATH($TUXEDO_LIBDIR)
	PHP_ADD_LIBRARY(wsc)
	PHP_ADD_LIBRARY(buft)
	PHP_ADD_LIBRARY(nwi)
	PHP_ADD_LIBRARY(nws)
	PHP_ADD_LIBRARY(gp)
	PHP_ADD_LIBRARY(fml)
	PHP_ADD_LIBRARY(fml32)
	TUXEDO_LIBS="-lwsc -lbuft -lnwi -lnws -lfml -lfml32 -lgp"
    old_LDFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS -L$TUXEDO_LIBDIR"
    LDFLAGS=$old_LDFLAGS
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
