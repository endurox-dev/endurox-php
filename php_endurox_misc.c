/*
	===============================================================|
	|   PHP Endurox                                                 |
	|--------------------------------------------------------------|
	|  php_endurox_misc.c                                           |
	|    Misc functions used whereever                             |
	===============================================================|
*/
/* $Id: php_endurox_misc.c,v 1.4 2001/05/28 18:16:19 bfoddy Exp $ */


/*
		This file contains misc conversion functions.
*/
#include <stdlib.h>
#include <string.h>
/* include the endurox headers */
#include <atmi.h>
#include <ubf.h>
#include <ubf32.h>

/* include standard php header */

#include "php.h"
#include "php_endurox.h"

#if HAVE_ENDUROX

/* True globals, no need for thread safety */
extern int ndrx_rh_alloc_buffer;  /* tpalloc buffer resource type resource handle*/


char * _ndrx_type2string (long type)
{
	static char * buf_types [NDRX_NUM_BUF_TYPES] =
		{"STRING", "CARRAY", "UBF", "UBF32", "VIEW" };
		
	return buf_types[type];
}

/*
	We got to using this construct so often, I made it a function.
*/
int _ndrx_is_ubf_type (int type)
{
	int ret_val;
	
	switch (type)
	{
		case NDRX_UBF_BUF_TYPE:
			ret_val = FALSE;
			break;
		
		case NDRX_UBF32_BUF_TYPE:
			ret_val = TRUE;
			break;
			
		default:
			zend_error (E_WARNING, "Buffer type not UBF / UBF32");
			ret_val = -1;
	}
	
	return ret_val;
}


/*	end #if HAVE_ENDUROX */
#endif
