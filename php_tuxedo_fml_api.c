/*
	===============================================================|
	|   PHP Tuxedo                                                 |
	|--------------------------------------------------------------|
	|  php_tuxedo_fml_api.c                                        |
	|    Raw Tuxedo FML entry points                               |
	===============================================================|
*/
/* $Id: php_tuxedo_fml_api.c,v 1.12 2002/01/11 00:19:04 bfoddy Exp $ */


/*
	This file contains all the raw fml api function calls.
	
	Most of these functions map 1 for 1 with the raw FML function calls.
	Many of these functions can be categorized by the arguments they take.
	Functions with the same or similar arguments often look very similar and
	are grouped together to make it simplier to duplicate for the next function.
	
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
#include "ext/standard/file.h"

#if HAVE_TUXEDO
#if (TUX_FML32 || TUX_FML)
/* True globals, no need for thread safety */
extern int tux_rh_alloc_buffer;  /* tpalloc buffer resource type resource handle*/


/*
	This function is a private function that will check
	an argument to see if the provided value is a string
	or an integer.  If its a string, it will cal Fldid(32) against
	the argument to convert it to a true Field ID.
*/
FLDID32 _get_arg_field_id (zval ** arg_field, int is32, int arg_no)
{
	FLDID32 field_id;
		
	switch (Z_TYPE_PP (arg_field)) {
		case IS_STRING:  /* need to convert it to the real Field ID */

			field_id = IS32 (is32,
						Fldid32 ((*arg_field)->value.str.val),
						Fldid ((*arg_field)->value.str.val));
			break;
			
		case IS_LONG:  /* everything is ok */
			field_id = (FLDID32) (*arg_field)->value.lval;
			break;
			
		default:
			php_error (E_WARNING, "%s(): argument %d must be of type integer or string",
					get_active_function_name(TSRMLS_C), arg_no);
			return (-1);
	}
	return (field_id);
}

/**********************************************
	Functions that take misc arguments
	and don't offer much copy opportunity.
***********************************************/

/* {{{ function tux_get_ferrno
	function returns value of Ferror
*/
ZEND_FUNCTION (tux_get_ferrno)
{
	if (ZEND_NUM_ARGS () != 0)
	{
		WRONG_PARAM_COUNT;
	}
	
	RETURN_LONG (Ferror);
}
/* }}} */	



/* {{{ function tux_fstrerror
	Function to return a string containing a tuxedo error description.
	Function accepts one argument, ferror.
*/
ZEND_FUNCTION (tux_fstrerror)
{
	zval **arg_ferrno;

	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, &arg_ferrno) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_ferrno);

	RETURN_STRING (Fstrerror32 ((*arg_ferrno)->value.lval), 1);
}
/* }}} */






/*********************************************************
		Functions that take only one FML buffer argument.
**********************************************************/

/* {{{ function tux_Ffprintf
	function calls Ffprintf
	Accepts 1 argument, FML Resource#
	
	Returns int similar to fprintf.
*/
ZEND_FUNCTION (tux_ffprintf)
{	
	zval ** arg_buf_ref;

	tux_tpalloc_buf_type * fml_buf_res;
	FILE * fp;
	long ret_val;
	int is32;


	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1,
			&arg_buf_ref) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_buf_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);

	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);
		
	fp = fopen ("/tmp/php_tux.out", "a");

	ret_val = IS32 (is32, Ffprint32 ((FBFR32*)fml_buf_res->buf, fp),
						  Ffprint ((FBFR*)fml_buf_res->buf, fp));
						  
	fclose (fp);

	RETURN_LONG (ret_val);
}
/* }}} */



/* {{{ function tux_fchksum
	function takes one argument:
	1.  FML buffer resource
	
	and returns the result from Fchksum(32).
*/
ZEND_FUNCTION (tux_fchksum)
{
	zval ** arg_fml_ref;

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

	
	RETURN_LONG (IS32(is32, Fchksum32 ((FBFR32*)fml_buf_res->buf), 
							Fchksum((FBFR*)fml_buf_res->buf)));
}
/* }}} */



/* {{{ function tux_fidxused
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int fidxused
*/
ZEND_FUNCTION (tux_fidxused)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fidxused32 ((FBFR32*)fml_buf_res->buf),
							 Fidxused ((FBFR*) fml_buf_res->buf)));
}
/* }}} */



/* {{{ function tux_fielded
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int fielded
*/
ZEND_FUNCTION (tux_fielded)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fielded32 ((FBFR32*)fml_buf_res->buf),
							 Fielded ((FBFR*) fml_buf_res->buf)));
}
/* }}} */



/* {{{ function tux_fnum
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int fnum
*/
ZEND_FUNCTION (tux_fnum)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fnum32 ((FBFR32*)fml_buf_res->buf),
							 Fnum ((FBFR*) fml_buf_res->buf)));
}
/* }}} */


/* {{{ function tux_fsizeof
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int fsizeof
*/
ZEND_FUNCTION (tux_fsizeof)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fsizeof32 ((FBFR32*)fml_buf_res->buf),
							 Fsizeof ((FBFR*) fml_buf_res->buf)));
}
/* }}} */



/* {{{ function tux_funindex
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int funindex
*/
ZEND_FUNCTION (tux_funindex)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Funindex32 ((FBFR32*)fml_buf_res->buf),
							 Funindex ((FBFR*) fml_buf_res->buf)));
}
/* }}} */



/* {{{ function tux_funused
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int funused
*/
ZEND_FUNCTION (tux_funused)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Funused32 ((FBFR32*)fml_buf_res->buf),
							 Funused ((FBFR*) fml_buf_res->buf)));
}
/* }}} */




/* {{{ function tux_fused
	Accepts 1 argument, 
	1.  FML Resource#
	
	Returns int fused
*/
ZEND_FUNCTION (tux_fused)
{	
	zval ** arg_fml_ref;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_fml_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fused32 ((FBFR32*)fml_buf_res->buf),
							 Fused ((FBFR*) fml_buf_res->buf)));
}
/* }}} */


/***********************************************************
	Functions that take only 2 FML arguments
************************************************************/

/* {{{ function tux_fcmp
	function takes two argument:
	1.  FML buffer resource
	2.  FML buffer resource
	
	and returns the result from Fcmp(32).
*/
ZEND_FUNCTION (tux_fcmp)
{
	zval ** arg_fml_ref1;
	zval ** arg_fml_ref2;
	

	tux_tpalloc_buf_type * fml_buf_res1;
	tux_tpalloc_buf_type * fml_buf_res2;
	int is32_1, is32_2;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref1,
			&arg_fml_ref2)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res1, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref1, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res2, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref2, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32_1 = _tux_is_fml_type (fml_buf_res1->type)) == -1)
		RETURN_LONG (-1);
	
	if ((is32_2 = _tux_is_fml_type (fml_buf_res2->type)) == -1)
		RETURN_LONG (-1);

	if (is32_1 != is32_2)
	{
		zend_error (E_WARNING, "Attemping to compare different FML buffer types");
		RETURN_LONG (-1);
	}
	
	RETURN_LONG (IS32(is32_1, Fcmp32 ((FBFR32*)fml_buf_res1->buf, (FBFR32*)fml_buf_res2->buf), 
							Fcmp((FBFR*)fml_buf_res1->buf, (FBFR*)fml_buf_res2->buf)));
}
/* }}} */


/* {{{ function tux_fconcat
	function takes two argument:
	1.  FML buffer resource destination
	2.  FML buffer resource source
	
	and returns the result from Fconcat(32).
*/
ZEND_FUNCTION (tux_fconcat)
{
	zval ** arg_fml_ref_d;
	zval ** arg_fml_ref_s;
	

	tux_tpalloc_buf_type * fml_buf_res_d;
	tux_tpalloc_buf_type * fml_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref_d,
			&arg_fml_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res_d, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_d, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res_s, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_s, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32_d = _tux_is_fml_type (fml_buf_res_d->type)) == -1)
		RETURN_LONG (-1);
	
	if ((is32_s = _tux_is_fml_type (fml_buf_res_s->type)) == -1)
		RETURN_LONG (-1);

	if (is32_d != is32_s)
	{
		zend_error (E_WARNING, "Attemping to copy different FML buffer types");
		RETURN_LONG (-1);
	}
	
	RETURN_LONG (IS32(is32_d, Fconcat32 ((FBFR32*)fml_buf_res_d->buf, (FBFR32*)fml_buf_res_s->buf), 
							Fconcat((FBFR*)fml_buf_res_d->buf, (FBFR*)fml_buf_res_s->buf)));
}
/* }}} */




/* {{{ function tux_fcpy
	function takes two argument:
	1.  FML buffer resource destination
	2.  FML buffer resource source
	
	and returns the result from Fcpy(32).
*/
ZEND_FUNCTION (tux_fcpy)
{
	zval ** arg_fml_ref_d;
	zval ** arg_fml_ref_s;
	

	tux_tpalloc_buf_type * fml_buf_res_d;
	tux_tpalloc_buf_type * fml_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref_d,
			&arg_fml_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res_d, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_d, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res_s, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_s, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32_d = _tux_is_fml_type (fml_buf_res_d->type)) == -1)
		RETURN_LONG (-1);
	
	if ((is32_s = _tux_is_fml_type (fml_buf_res_s->type)) == -1)
		RETURN_LONG (-1);

	if (is32_d != is32_s)
	{
		zend_error (E_WARNING, "Attemping to copy different FML buffer types");
		RETURN_LONG (-1);
	}
	
	RETURN_LONG (IS32(is32_d, Fcpy32 ((FBFR32*)fml_buf_res_d->buf, (FBFR32*)fml_buf_res_s->buf), 
							Fcpy((FBFR*)fml_buf_res_d->buf, (FBFR*)fml_buf_res_s->buf)));
}
/* }}} */



/* {{{ function tux_fjoin
	function takes two argument:
	1.  FML buffer resource destination
	2.  FML buffer resource source
	
	and returns the result from Fjoin(32).
*/
ZEND_FUNCTION (tux_fjoin)
{
	zval ** arg_fml_ref_d;
	zval ** arg_fml_ref_s;
	

	tux_tpalloc_buf_type * fml_buf_res_d;
	tux_tpalloc_buf_type * fml_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref_d,
			&arg_fml_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res_d, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_d, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res_s, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_s, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32_d = _tux_is_fml_type (fml_buf_res_d->type)) == -1)
		RETURN_LONG (-1);
	
	if ((is32_s = _tux_is_fml_type (fml_buf_res_s->type)) == -1)
		RETURN_LONG (-1);

	if (is32_d != is32_s)
	{
		zend_error (E_WARNING, "Attemping to join different FML buffer types");
		RETURN_LONG (-1);
	}
	
	RETURN_LONG (IS32(is32_d, Fjoin32 ((FBFR32*)fml_buf_res_d->buf, (FBFR32*)fml_buf_res_s->buf), 
							Fjoin((FBFR*)fml_buf_res_d->buf, (FBFR*)fml_buf_res_s->buf)));
}
/* }}} */



/* {{{ function tux_fmove
	function takes two argument:
	1.  FML buffer resource destination
	2.  FML buffer resource source
	
	and returns the result from Fmove(32).
	
	CAREFULL::  This function has a slight modification to the norm
	for these args because the destination is not required to be a FML Buf.
*/
ZEND_FUNCTION (tux_fmove)
{
	zval ** arg_fml_ref_d;
	zval ** arg_fml_ref_s;
	

	tux_tpalloc_buf_type * fml_buf_res_d;
	tux_tpalloc_buf_type * fml_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref_d,
			&arg_fml_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res_d, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_d, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res_s, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_s, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	
	if ((is32_s = _tux_is_fml_type (fml_buf_res_s->type)) == -1)
		RETURN_LONG (-1);

	
	RETURN_LONG (IS32(is32_d, Fmove32 (fml_buf_res_d->buf, (FBFR32*)fml_buf_res_s->buf), 
							Fmove(fml_buf_res_d->buf, (FBFR*)fml_buf_res_s->buf)));
}
/* }}} */





/* {{{ function tux_fojoin
	function takes two argument:
	1.  FML buffer resource destination
	2.  FML buffer resource source
	
	and returns the result from Fojoin(32).
*/
ZEND_FUNCTION (tux_fojoin)
{
	zval ** arg_fml_ref_d;
	zval ** arg_fml_ref_s;
	

	tux_tpalloc_buf_type * fml_buf_res_d;
	tux_tpalloc_buf_type * fml_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref_d,
			&arg_fml_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res_d, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_d, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res_s, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_s, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32_d = _tux_is_fml_type (fml_buf_res_d->type)) == -1)
		RETURN_LONG (-1);
	
	if ((is32_s = _tux_is_fml_type (fml_buf_res_s->type)) == -1)
		RETURN_LONG (-1);

	if (is32_d != is32_s)
	{
		zend_error (E_WARNING, "Attemping to outer join different FML buffer types");
		RETURN_LONG (-1);
	}
	
	RETURN_LONG (IS32(is32_d, Fojoin32 ((FBFR32*)fml_buf_res_d->buf, (FBFR32*)fml_buf_res_s->buf), 
							Fojoin((FBFR*)fml_buf_res_d->buf, (FBFR*)fml_buf_res_s->buf)));
}
/* }}} */




/* {{{ function tux_fupdate
	function takes two argument:
	1.  FML buffer resource destination
	2.  FML buffer resource source
	
	and returns the result from Fupdate(32).
*/
ZEND_FUNCTION (tux_fupdate)
{
	zval ** arg_fml_ref_d;
	zval ** arg_fml_ref_s;
	

	tux_tpalloc_buf_type * fml_buf_res_d;
	tux_tpalloc_buf_type * fml_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref_d,
			&arg_fml_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res_d, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_d, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						fml_buf_res_s, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref_s, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32_d = _tux_is_fml_type (fml_buf_res_d->type)) == -1)
		RETURN_LONG (-1);
	
	if ((is32_s = _tux_is_fml_type (fml_buf_res_s->type)) == -1)
		RETURN_LONG (-1);

	if (is32_d != is32_s)
	{
		zend_error (E_WARNING, "Attemping to outer join different FML buffer types");
		RETURN_LONG (-1);
	}
	
	RETURN_LONG (IS32(is32_d, Fupdate32 ((FBFR32*)fml_buf_res_d->buf, (FBFR32*)fml_buf_res_s->buf), 
							Fupdate((FBFR*)fml_buf_res_d->buf, (FBFR*)fml_buf_res_s->buf)));
}
/* }}} */




/*********************************************************
	Functions that take 2 args, an FML buffer and 
	one other (usually long) arg.
**********************************************************/

/* {{{ function tux_fdelall
	Accepts 2 argumens, 
	1.  FML Resource#
	2.  Fieldid
	
	Returns int fdelall.
*/
ZEND_FUNCTION (tux_fdelall)
{	
	zval ** arg_fml_ref;
	zval ** arg_field;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
	FLDID32 field_id;
			
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref,
			&arg_field)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fdelall32 ((FBFR32*)fml_buf_res->buf, (FLDID32) field_id),
							 Fdelall ((FBFR*) fml_buf_res->buf, (FLDID) field_id)));
}
/* }}} */



/* {{{ function tux_findex
	Accepts 2 argumens, 
	1.  FML Resource#
	2.  Index interval
	
	Returns int findex.
*/
ZEND_FUNCTION (tux_findex)
{	
	zval ** arg_fml_ref;
	zval ** arg_index_intvl;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref,
			&arg_index_intvl)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_index_intvl);

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Findex32 ((FBFR32*)fml_buf_res->buf, (FLDOCC32)(*arg_index_intvl)->value.lval),
							 Findex ((FBFR*) fml_buf_res->buf, (FLDOCC)(*arg_index_intvl)->value.lval)));
}
/* }}} */




/* {{{ function tux_frstrindex
	Accepts 2 argumens, 
	1.  FML Resource#
	2.  Index interval
	
	Returns int frstrindex.
*/
ZEND_FUNCTION (tux_frstrindex)
{	
	zval ** arg_fml_ref;
	zval ** arg_index_intvl;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref,
			&arg_index_intvl)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_index_intvl);

	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Frstrindex32 ((FBFR32*)fml_buf_res->buf, (FLDOCC32)(*arg_index_intvl)->value.lval),
							 Frstrindex ((FBFR*) fml_buf_res->buf, (FLDOCC)(*arg_index_intvl)->value.lval)));
}
/* }}} */



/* {{{ function tux_foccur
	Accepts 2 argumens, 
	1.  FML Resource#
	2.  Field ID
	
	Returns int findex.
*/
ZEND_FUNCTION (tux_foccur)
{	
	zval ** arg_fml_ref;
	zval ** arg_field;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
	FLDID32 field_id;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_fml_ref,
			&arg_field)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);


	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Foccur32 ((FBFR32*)fml_buf_res->buf, (FLDID32) field_id),
							 Foccur ((FBFR*) fml_buf_res->buf, (FLDID) field_id)));
}
/* }}} */
/***********************************************************
	Functions that take a FieldID and a special passed buf type.
***************************************************************/

/* {{{ function tux_fldid
	function takes two arguments:
	1.  the field name 
	2.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the corresponding Field ID.
*/
ZEND_FUNCTION (tux_fldid)
{
	zval ** arg_field_name;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_field_name,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg_field_name);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Fldid32 ((*arg_field_name)->value.str.val),
			Fldid ((*arg_field_name)->value.str.val)));
}
/* }}} */



/* {{{ function tux_fldno
	function takes two arguments:
	1.  the field id 
	2.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the corresponding Field Number.
*/
ZEND_FUNCTION (tux_fldno)
{
	zval ** arg_field_id;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_field_id,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_field_id);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Fldno32 ((FLDID32) (*arg_field_id)->value.lval),
			Fldno ((FLDID) (*arg_field_id)->value.lval)));
}
/* }}} */



/* {{{ function tux_fldtype
	function takes two arguments:
	1.  the field id 
	2.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the result from Fldtype(32).
*/
ZEND_FUNCTION (tux_fldtype)
{
	zval ** arg_field_id;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_field_id,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_field_id);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Fldtype32 ((FLDID32) (*arg_field_id)->value.lval),
			Fldtype ((FLDID) (*arg_field_id)->value.lval)));
}
/* }}} */



/* {{{ function tux_fmkfldid
	function takes three arguments:
	1.  the data type from fml.h
	2.  The Field Number 
	3.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the result from Fmkfldid(32).
*/
ZEND_FUNCTION (tux_fmkfldid)
{
	zval ** arg_data_type;
	zval ** arg_field_num;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_data_type,
			&arg_field_num,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_data_type);
	convert_to_long_ex (arg_field_num);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Fmkfldid32 ((int) (*arg_data_type)->value.lval, (FLDID32) (*arg_field_num)->value.lval),
			Fmkfldid ((int) (*arg_data_type)->value.lval, (FLDID) (*arg_field_num)->value.lval)));
}
/* }}} */





/* {{{ function tux_fname
	function takes two arguments:
	1.  the field id 
	2.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the result from Fname(32).
*/
ZEND_FUNCTION (tux_fname)
{
	zval ** arg_field_id;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_field_id,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_field_id);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_NULL ();
	}

	RETURN_STRING (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Fname32 ((FLDID32) (*arg_field_id)->value.lval),
			Fname ((FLDID) (*arg_field_id)->value.lval)), 1);
}
/* }}} */


/* {{{ function tux_fneeded
	function takes three arguments:
	1.  the number of fields
	2.  Amount of value space 
	3.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the result from Fneeded(32).
*/
ZEND_FUNCTION (tux_fneeded)
{
	zval ** arg_fields;
	zval ** arg_value_space;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_fields,
			&arg_value_space,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_fields);
	convert_to_long_ex (arg_value_space);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Fneeded32 ((FLDOCC32) (*arg_fields)->value.lval, (FLDLEN) (*arg_value_space)->value.lval),
			Fneeded ((FLDOCC) (*arg_fields)->value.lval, (FLDLEN) (*arg_value_space)->value.lval)));
}
/* }}} */



/* {{{ function tux_ftype
	function takes two arguments:
	1.  the field id 
	2.  The FML / FML32 type (TUX_FML_BUF_TYPE, TUX_FML32_BUF_TYPE)
	
	and returns the result from Ftype(32).
*/
ZEND_FUNCTION (tux_ftype)
{
	zval ** arg_field_id;
	zval ** arg_buf_type;
	

	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_field_id,
			&arg_buf_type)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_field_id);
	convert_to_long_ex (arg_buf_type);
	
	if (((*arg_buf_type)->value.lval != TUX_FML_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != TUX_FML32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a FML/FML32");
		RETURN_NULL ();
	}

	RETURN_STRING (
		IS32 (((*arg_buf_type)->value.lval == TUX_FML32_BUF_TYPE),
			Ftype32 ((FLDID32) (*arg_field_id)->value.lval),
			Ftype ((FLDID) (*arg_field_id)->value.lval)), 1);
}
/* }}} */



/************************************************************
	Functions taking 3+ arguments, a FML resource and two plus
	other arguments.
*************************************************************/

/* {{{ function tux_fdel
	Accepts 3 argumens, 
	1.  FML Resource#
	2.  Fieldid
	3.  Occurance.
	
	Returns int fdel.
*/
ZEND_FUNCTION (tux_fdel)
{	
	zval ** arg_fml_ref;
	zval ** arg_field;
	zval ** arg_occ;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
	FLDID32 field_id;
			
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_fml_ref,
			&arg_field,
			&arg_occ)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_occ);
	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fdel32 ((FBFR32*)fml_buf_res->buf, 
									 (FLDID32) field_id,
									 (FLDOCC32) (*arg_occ)->value.lval),
							 Fdel   ((FBFR*) fml_buf_res->buf, 
							 		 (FLDID) field_id,
									 (FLDOCC) (*arg_occ)->value.lval)));
}
/* }}} */



/* {{{ function tux_flen
	Accepts 3 argumens, 
	1.  FML Resource#
	2.  Fieldid
	3.  Occurance.
	
	Returns int flen
*/
ZEND_FUNCTION (tux_flen)
{	
	zval ** arg_fml_ref;
	zval ** arg_field;
	zval ** arg_occ;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
	FLDID32 field_id;
		
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_fml_ref,
			&arg_field,
			&arg_occ)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_occ);
	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Flen32 ((FBFR32*)fml_buf_res->buf, 
									 (FLDID32) field_id,
									 (FLDOCC32) (*arg_occ)->value.lval),
							 Flen   ((FBFR*) fml_buf_res->buf, 
							 		 (FLDID) field_id,
									 (FLDOCC) (*arg_occ)->value.lval)));
}
/* }}} */



/* {{{ function tux_fpres
	Accepts 3 argumens, 
	1.  FML Resource#
	2.  Fieldid
	3.  Occurance.
	
	Returns int fpres
*/
ZEND_FUNCTION (tux_fpres)
{	
	zval ** arg_fml_ref;
	zval ** arg_field;
	zval ** arg_occ;
	

	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
	FLDID32 field_id;
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_fml_ref,
			&arg_field,
			&arg_occ)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_occ);
	
	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (is32, Fpres32 ((FBFR32*)fml_buf_res->buf, 
									 (FLDID32) field_id,
									 (FLDOCC32) (*arg_occ)->value.lval),
							 Fpres   ((FBFR*) fml_buf_res->buf, 
							 		 (FLDID) field_id,
									 (FLDOCC) (*arg_occ)->value.lval)));
}
/* }}} */


/* {{{ function tux_fadd (ALIAS:  tux_fchg)
	function takes three (or four) arguments:
	1.  FML buffer resource
	2.  Field ID
	3.  data value
	4.  (optional) the occurance value
	
	and returns the result from Fadd(32).
*/
ZEND_FUNCTION (tux_fadd)
{
	zval ** arg_fml_ref;
	zval ** arg_field;
	zval ** arg_data_val;
	zval ** arg_occ_val;
	
	tux_tpalloc_buf_type * fml_buf_res;
	int is32;
	FLDOCC32 occ = -1;
	FLDID32 field_id;
	

	switch (ZEND_NUM_ARGS ())
	{
		case 3:
			if (zend_get_parameters_ex (3, 
						&arg_fml_ref, 
						&arg_field, 
						&arg_data_val) !=  SUCCESS)
				WRONG_PARAM_COUNT;
			break;
			
		case 4:
			if (zend_get_parameters_ex (4, 
						&arg_fml_ref, 
						&arg_field, 
						&arg_data_val,
						&arg_occ_val) !=  SUCCESS)
				{WRONG_PARAM_COUNT;}
			else
			{
				convert_to_long_ex (arg_occ_val);
				occ = (*arg_occ_val)->value.lval;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_fml_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);



	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_LONG (-1);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);
	
/*
		We won't actually call Fadd, but rather a Fchg with an
		occurence of -1.  It should be the same.  If we find
		its not the same its pretty easy to change it.
*/
	
	RETURN_LONG (_tux_fml_add (fml_buf_res, arg_data_val, field_id, occ));
}
/* }}} */


/* {{{ function tux_fget
	function takes 2 or 3 arguments:
	1.  FML buffer resource
	2.  Field ID
	3.  (optional) the occurrence value

	returns the field value(LONG,DOUBLE,STRING, or FALSE)
*/
ZEND_FUNCTION (tux_fget)
{
	zval ** arg_fml_ref;
	zval ** arg_field;
	zval ** arg_occ_val;
	int argc;
	FLDID32 fldid;
	int occ = 0;	/* first occurrence is default */
   
	tux_tpalloc_buf_type *fml_buf_res;
	int is32;
	char *buf;
	int buflen;
	int rc;

	short short_data;
	long long_data;
	float float_data;
	double double_data;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 ||
		zend_get_parameters_ex(argc,
			&arg_fml_ref, &arg_field, &arg_occ_val) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

    /*
        Now get the buffer from the argument.
    */
    ZEND_FETCH_RESOURCE(
                        fml_buf_res,
                        tux_tpalloc_buf_type *,
                        arg_fml_ref,
                        -1,
                        "Tuxedo tpalloc buffer",
                        tux_rh_alloc_buffer);

	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1) {
		zend_error(E_WARNING, "resource is not fml type");
		RETURN_FALSE;
	}

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((fldid = _get_arg_field_id (arg_field, is32, 2)) == -1)
		RETURN_LONG (-1);

/*
	Check for a third arg.
*/
	if (argc == 3) {
		convert_to_long_ex(arg_occ_val);
		occ = Z_LVAL_PP(arg_occ_val);
	}

	/* get length of the field value  for memory allocation */
	buflen = IS32 (is32, Flen32((FBFR32 *)fml_buf_res->buf,
								(FLDID32)fldid, (FLDOCC32)occ),
						 Flen  ((FBFR *)fml_buf_res->buf,
								(FLDID)fldid, (FLDOCC)occ));

	if (buflen == -1)	/* not found the field occurrence */
		RETURN_FALSE;

	if ((buf = emalloc(buflen)) == NULL) {
		zend_error(E_WARNING, "unable to allocate %d bytes of memory", buflen);
		RETURN_FALSE;
	}

	rc = IS32 (is32, Fget32((FBFR32 *)fml_buf_res->buf,
							(FLDID32)fldid, (FLDOCC32)occ, buf, NULL),
					 Fget  ((FBFR *)fml_buf_res->buf,
							(FLDID)fldid, (FLDOCC)occ, buf, NULL));

	if (rc == -1)
		RETURN_FALSE;

	switch (IS32(is32, Fldtype32((FLDID32) fldid), Fldtype((FLDID) fldid))) {
	case FLD_SHORT:
		memcpy(&short_data, buf, sizeof(short));
		RETVAL_LONG(short_data);
		break;
	case FLD_LONG:
		memcpy(&long_data, buf, sizeof(long));
		RETVAL_LONG(long_data);
		break;
	case FLD_FLOAT:
		memcpy(&float_data, buf, sizeof(float));
		RETVAL_DOUBLE((double)float_data);
		break;
	case FLD_DOUBLE:
		memcpy(&double_data, buf, sizeof(double));
		RETVAL_DOUBLE(double_data);
		break;
	case FLD_CHAR:
	case FLD_STRING:
		RETVAL_STRING(buf, 1);
		break;
	case FLD_CARRAY:
		RETVAL_STRINGL(buf, buflen, 1);
		break;
	default:
		RETVAL_FALSE;
		break;
	}
	efree(buf);
}
/* }}} */


/* {{{ function tux_ffprint
	function calls Ffprint
	Acceps 2 or 3 arguments,
		FML Resource#
		output file path
		(optional) file open mode ("a" is default)
	Returns int similar to Ffprint.

ZEND_FETCH_RESOURCE(fp, FILE *, zvalue, -1, "File-Handle", php_file_le_fopen());.
*/
ZEND_FUNCTION (tux_ffprint)
{
	zval **arg_fml_ref;
	zval **arg_file_ref;

	int argc;
	tux_tpalloc_buf_type *fml_buf_res;
	int is32;
	FILE *fp = NULL;
	int rc, file_type;

	argc = ZEND_NUM_ARGS();
	if (argc != 2 || 
		(zend_get_parameters_ex(argc,
		&arg_fml_ref, &arg_file_ref) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(			/* fetch the Tuxedo resource */
						fml_buf_res,
						tux_tpalloc_buf_type *,
						arg_fml_ref,
						-1,
						"Tuxedo tpalloc buffer",
						tux_rh_alloc_buffer);


/*
		We need to use the raw function call because its
		much easier to add resources as they are needed.
		
		Currently, only flat files and pipes seem to work.
		I tried a fsockopen call and it core dumps inside
		Ffprint32 so we will leave streams and sockets out for now.
*/
	fp = (FILE *) zend_fetch_resource(	/* fetch the file handle resource */
						arg_file_ref, 
						-1, 
						"File-Handle", 
						&file_type,
						2,				/* number of resource types to take */
						php_file_le_fopen(),
		  				php_file_le_popen());
	ZEND_VERIFY_RESOURCE(fp);


	if ((is32 = _tux_is_fml_type(fml_buf_res->type)) == -1) {
		zend_error(E_WARNING, "argument 1 type is wrong.");
		RETURN_LONG (-1);
	}

	if (!fp) {
		zend_error (E_WARNING, "argument 2 is not a file handle.");
		RETURN_LONG (-1);
	}
#ifdef HAVE_FLUSHIO
		if (file_type == php_file_le_fopen()) {
			fseek((FILE*)fp, 0, SEEK_CUR);
		}
#endif

	rc = IS32 (is32, Ffprint32((FBFR32 *)fml_buf_res->buf, fp),
					 Ffprint((FBFR *)fml_buf_res->buf, fp));

	RETURN_LONG (rc);
}
/* }}} */


#endif
/*	end #if HAVE_TUXEDO */
#endif
