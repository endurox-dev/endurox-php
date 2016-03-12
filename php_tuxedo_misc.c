/*
	===============================================================|
	|   PHP Tuxedo                                                 |
	|--------------------------------------------------------------|
	|  php_tuxedo_misc.c                                           |
	|    Misc functions used whereever                             |
	===============================================================|
*/
/* $Id: php_tuxedo_misc.c,v 1.4 2001/05/28 18:16:19 bfoddy Exp $ */


/*
		This file contains misc conversion functions.
*/
#include <stdlib.h>
#include <string.h>
/* include the tuxedo headers */
#include <atmi.h>
#include <fml.h>
#include <fml32.h>
#include <tx.h>

/* include standard php header */

#include "php.h"
#include "php_tuxedo.h"

#if HAVE_TUXEDO

/* True globals, no need for thread safety */
extern int tux_rh_alloc_buffer;  /* tpalloc buffer resource type resource handle*/


char * _tux_type2string (long type)
{
	static char * buf_types [TUX_NUM_BUF_TYPES] =
		{"STRING", "CARRAY", "FML", "FML32", "VIEW" };
		
	return buf_types[type];
}

/*
	We got to using this construct so often, I made it a function.
*/
int _tux_is_fml_type (int type)
{
	int ret_val;
	
	switch (type)
	{
		case TUX_FML_BUF_TYPE:
			ret_val = FALSE;
			break;
		
		case TUX_FML32_BUF_TYPE:
			ret_val = TRUE;
			break;
			
		default:
			zend_error (E_WARNING, "Buffer type not FML / FML32");
			ret_val = -1;
	}
	
	return ret_val;
}


/*	end #if HAVE_TUXEDO */
#endif
