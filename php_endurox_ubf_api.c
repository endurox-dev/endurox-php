/*
	===============================================================|
	|   PHP Endurox                                                 |
	|--------------------------------------------------------------|
	|  php_endurox_ubf_api.c                                        |
	|    Raw Endurox UBF entry points                               |
	===============================================================|
*/
/* $Id: php_endurox_ubf_api.c,v 1.12 2002/01/11 00:19:04 bfoddy Exp $ */


/*
	This file contains all the raw ubf api function calls.
	
	Most of these functions map 1 for 1 with the raw UBF function calls.
	Many of these functions can be categorized by the arguments they take.
	Functions with the same or similar arguments often look very similar and
	are grouped together to make it simplier to duplicate for the next function.
	
*/
#include <stdlib.h>
#include <string.h>
/* include the endurox headers */
#include <atmi.h>
#include <ubf.h>

/* include standard php header */

#include "php.h"
#include "php_endurox.h"
#include "ext/standard/file.h"

#if HAVE_ENDUROX
#if (NDRX_UBF32 || NDRX_UBF)
/* True globals, no need for thread safety */
extern int ndrx_rh_alloc_buffer;  /* tpalloc buffer resource type resource handle*/


/*
	This function is a private function that will check
	an argument to see if the provided value is a string
	or an integer.  If its a string, it will cal Bfldid(32) against
	the argument to convert it to a true Field ID.
*/
BFLDID _get_arg_field_id (zval ** arg_field, int arg_no)
{
	BFLDID field_id;
		
	switch (Z_TYPE_PP (arg_field)) {
		case IS_STRING:  /* need to convert it to the real Field ID */
			field_id=Bfldid ((*arg_field)->value.str.val);
			break;
			
		case IS_LONG:  /* everything is ok */
			field_id = (BFLDID) (*arg_field)->value.lval;
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

/* {{{ function ndrx_get_ferrno
	function returns value of Berror
*/
ZEND_FUNCTION (ndrx_get_ferrno)
{
	if (ZEND_NUM_ARGS () != 0)
	{
		WRONG_PARAM_COUNT;
	}
	
	RETURN_LONG (Berror);
}
/* }}} */	



/* {{{ function ndrx_bstrerror
	Function to return a string containing a endurox error description.
	Function accepts one argument, berror.
*/
ZEND_FUNCTION (ndrx_bstrerror)
{
	zval **arg_ferrno;

	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, &arg_ferrno) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_ferrno);

	RETURN_STRING (Bstrerror32 ((*arg_ferrno)->value.lval), 1);
}
/* }}} */






/*********************************************************
		Functions that take only one UBF buffer argument.
**********************************************************/

/* {{{ function ndrx_Bfprintf
	function calls Bfprintf
	Accepts 1 argument, UBF Resource#
	
	Returns int similar to fprintf.
*/
ZEND_FUNCTION (ndrx_bfprintf)
{	
	zval ** arg_buf_ref;

	ndrx_tpalloc_buf_type * ubf_buf_res;
	FILE * fp;
	long ret_val;


	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1,
			&arg_buf_ref) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_buf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

	fp = fopen ("/tmp/php_ndrx.out", "a");

	ret_val = Bfprint ((UBFH*)ubf_buf_res->buf, fp);
						  
	fclose (fp);

	RETURN_LONG (ret_val);
}
/* }}} */


/* {{{ function ndrx_bidxused
	Accepts 1 argument, 
	1.  UBF Resource#
	
	Returns int bidxused
*/
ZEND_FUNCTION (ndrx_bidxused)
{	
	zval ** arg_ubf_ref;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_ubf_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	RETURN_LONG (Bidxused ((UBFH*)ubf_buf_res->buf));
}
/* }}} */



/* {{{ function ndrx_bisubf
	Accepts 1 argument, 
	1.  UBF Resource#
	
	Returns int bisubf
*/
ZEND_FUNCTION (ndrx_bisubf)
{	
	zval ** arg_ubf_ref;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_ubf_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	RETURN_LONG (Bisubf ((UBFH*)ubf_buf_res->buf));
}
/* }}} */



/* {{{ function ndrx_bsizeof
	Accepts 1 argument, 
	1.  UBF Resource#
	
	Returns int bsizeof
*/
ZEND_FUNCTION (ndrx_bsizeof)
{	
	zval ** arg_ubf_ref;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_ubf_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	RETURN_LONG (Bsizeof ((UBFH*)ubf_buf_res->buf));
}
/* }}} */



/* {{{ function ndrx_bunindex
	Accepts 1 argument, 
	1.  UBF Resource#
	
	Returns int bunindex
*/
ZEND_FUNCTION (ndrx_bunindex)
{	
	zval ** arg_ubf_ref;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_ubf_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	RETURN_LONG (Bunindex ((UBFH*)ubf_buf_res->buf));
}
/* }}} */



/* {{{ function ndrx_bunused
	Accepts 1 argument, 
	1.  UBF Resource#
	
	Returns int bunused
*/
ZEND_FUNCTION (ndrx_bunused)
{	
	zval ** arg_ubf_ref;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
		
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_ubf_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

	RETURN_LONG (Bunused((UBFH*)ubf_buf_res->buf));
}
/* }}} */




/* {{{ function ndrx_bused
	Accepts 1 argument, 
	1.  UBF Resource#
	
	Returns int bused
*/
ZEND_FUNCTION (ndrx_bused)
{	
	zval ** arg_ubf_ref;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, 
			&arg_ubf_ref)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

	RETURN_LONG (Bused ((UBFH*)ubf_buf_res->buf));
}
/* }}} */


/***********************************************************
	Functions that take only 2 UBF arguments
************************************************************/

/* {{{ function ndrx_bconcat
	function takes two argument:
	1.  UBF buffer resource destination
	2.  UBF buffer resource source
	
	and returns the result from Bconcat(32).
*/
ZEND_FUNCTION (ndrx_bconcat)
{
	zval ** arg_ubf_ref_d;
	zval ** arg_ubf_ref_s;
	

	ndrx_tpalloc_buf_type * ubf_buf_res_d;
	ndrx_tpalloc_buf_type * ubf_buf_res_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref_d,
			&arg_ubf_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						ubf_buf_res_d, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_d, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						ubf_buf_res_s, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_s, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

	
	RETURN_LONG (Bconcat ((UBFH*)ubf_buf_res_d->buf, (UBFH*)ubf_buf_res_s->buf));
}
/* }}} */




/* {{{ function ndrx_bcpy
	function takes two argument:
	1.  UBF buffer resource destination
	2.  UBF buffer resource source
	
	and returns the result from Bcpy(32).
*/
ZEND_FUNCTION (ndrx_bcpy)
{
	zval ** arg_ubf_ref_d;
	zval ** arg_ubf_ref_s;
	

	ndrx_tpalloc_buf_type * ubf_buf_res_d;
	ndrx_tpalloc_buf_type * ubf_buf_res_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref_d,
			&arg_ubf_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						ubf_buf_res_d, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_d, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						ubf_buf_res_s, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_s, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	
	RETURN_LONG (Bcpy((UBFH*)ubf_buf_res_d->buf, (UBFH*)ubf_buf_res_s->buf));
}
/* }}} */



/* {{{ function ndrx_bjoin
	function takes two argument:
	1.  UBF buffer resource destination
	2.  UBF buffer resource source
	
	and returns the result from Bjoin(32).
*/
ZEND_FUNCTION (ndrx_bjoin)
{
	zval ** arg_ubf_ref_d;
	zval ** arg_ubf_ref_s;
	

	ndrx_tpalloc_buf_type * ubf_buf_res_d;
	ndrx_tpalloc_buf_type * ubf_buf_res_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref_d,
			&arg_ubf_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						ubf_buf_res_d, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_d, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						ubf_buf_res_s, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_s, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

	
	RETURN_LONG (Bjoin((UBFH*)ubf_buf_res_d->buf, (UBFH*)ubf_buf_res_s->buf));
}
/* }}} */


/* {{{ function ndrx_bupdate
	function takes two argument:
	1.  UBF buffer resource destination
	2.  UBF buffer resource source
	
	and returns the result from Bupdate(32).
*/
ZEND_FUNCTION (ndrx_bupdate)
{
	zval ** arg_ubf_ref_d;
	zval ** arg_ubf_ref_s;
	

	ndrx_tpalloc_buf_type * ubf_buf_res_d;
	ndrx_tpalloc_buf_type * ubf_buf_res_s;
	int is32_d, is32_s;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref_d,
			&arg_ubf_ref_s)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						ubf_buf_res_d, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_d, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						ubf_buf_res_s, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref_s, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

	
	RETURN_LONG (Bupdate((UBFH*)ubf_buf_res_d->buf, (UBFH*)ubf_buf_res_s->buf));
}
/* }}} */




/*********************************************************
	Functions that take 2 args, an UBF buffer and 
	one other (usually long) arg.
**********************************************************/

/* {{{ function ndrx_bdelall
	Accepts 2 argumens, 
	1.  UBF Resource#
	2.  Fieldid
	
	Returns int bdelall.
*/
ZEND_FUNCTION (ndrx_bdelall)
{	
	zval ** arg_ubf_ref;
	zval ** arg_field;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	BFLDID field_id;
			
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref,
			&arg_field)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (Bdelall((UBFH*)ubf_buf_res->buf, (BFLDID) field_id)));
}
/* }}} */


/* {{{ function ndrx_brstrindex
	Accepts 2 argumens, 
	1.  UBF Resource#
	2.  Index interval
	
	Returns int brstrindex.
*/
ZEND_FUNCTION (ndrx_brstrindex)
{	
	zval ** arg_ubf_ref;
	zval ** arg_index_intvl;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	int is32;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref,
			&arg_index_intvl)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_index_intvl);

	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);



	RETURN_LONG (Brstrindex((UBFH*)ubf_buf_res->buf, (BFLDOCC32)(*arg_index_intvl)->value.lval));
}
/* }}} */



/* {{{ function ndrx_boccur
	Accepts 2 argumens, 
	1.  UBF Resource#
	2.  Field ID
	
	Returns int findex.
*/
ZEND_FUNCTION (ndrx_boccur)
{	
	zval ** arg_ubf_ref;
	zval ** arg_field;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	BFLDID field_id;
		
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, 
			&arg_ubf_ref,
			&arg_field)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (Boccur ((UBFH*)ubf_buf_res->buf, (BFLDID) field_id));
}
/* }}} */
/***********************************************************
	Functions that take a FieldID and a special passed buf type.
***************************************************************/

/* {{{ function ndrx_bfldid
	function takes two arguments:
	1.  the field name 
	2.  The UBF / UBF32 type (NDRX_UBF_BUF_TYPE, NDRX_UBF32_BUF_TYPE)
	
	and returns the corresponding Field ID.
*/
ZEND_FUNCTION (ndrx_bfldid)
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
	
	if (((*arg_buf_type)->value.lval != NDRX_UBF_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != NDRX_UBF32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a UBF/UBF32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (Bfldid ((*arg_field_name)->value.str.val));
}
/* }}} */



/* {{{ function ndrx_bfldno
	function takes two arguments:
	1.  the field id 
	2.  The UBF / UBF32 type (NDRX_UBF_BUF_TYPE, NDRX_UBF32_BUF_TYPE)
	
	and returns the corresponding Field Number.
*/
ZEND_FUNCTION (ndrx_bfldno)
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
	
	if (((*arg_buf_type)->value.lval != NDRX_UBF_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != NDRX_UBF32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a UBF/UBF32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (Bfldno ((BFLDID) (*arg_field_id)->value.lval));
}
/* }}} */



/* {{{ function ndrx_bfldtype
	function takes two arguments:
	1.  the field id 
	2.  The UBF / UBF32 type (NDRX_UBF_BUF_TYPE, NDRX_UBF32_BUF_TYPE)
	
	and returns the result from Bfldtype(32).
*/
ZEND_FUNCTION (ndrx_bfldtype)
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
	
	if (((*arg_buf_type)->value.lval != NDRX_UBF_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != NDRX_UBF32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a UBF/UBF32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (Bfldtype ((BFLDID) (*arg_field_id)->value.lval));
}
/* }}} */



/* {{{ function ndrx_fmkbfldid
	function takes three arguments:
	1.  the data type from ubf.h
	2.  The Field Number 
	3.  The UBF / UBF32 type (NDRX_UBF_BUF_TYPE, NDRX_UBF32_BUF_TYPE)
	
	and returns the result from Fmkbfldid(32).
*/
ZEND_FUNCTION (ndrx_fmkbfldid)
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
	
	if (((*arg_buf_type)->value.lval != NDRX_UBF_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != NDRX_UBF32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a UBF/UBF32");
		RETURN_LONG (-1);
	}

	RETURN_LONG (Fmkbfldid ((int) (*arg_data_type)->value.lval, (BFLDID) (*arg_field_num)->value.lval));
}
/* }}} */





/* {{{ function ndrx_bfname
	function takes two arguments:
	1.  the field id 
	2.  The UBF / UBF32 type (NDRX_UBF_BUF_TYPE, NDRX_UBF32_BUF_TYPE)
	
	and returns the result from Bfname(32).
*/
ZEND_FUNCTION (ndrx_bfname)
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
	
	if (((*arg_buf_type)->value.lval != NDRX_UBF_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != NDRX_UBF32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a UBF/UBF32");
		RETURN_NULL ();
	}

	RETURN_STRING (Bfname ((BFLDID) (*arg_field_id)->value.lval), 1);
}
/* }}} */

/* {{{ function ndrx_btype
	function takes two arguments:
	1.  the field id 
	2.  The UBF / UBF32 type (NDRX_UBF_BUF_TYPE, NDRX_UBF32_BUF_TYPE)
	
	and returns the result from Btype(32).
*/
ZEND_FUNCTION (ndrx_btype)
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
	
	if (((*arg_buf_type)->value.lval != NDRX_UBF_BUF_TYPE) &&
		((*arg_buf_type)->value.lval != NDRX_UBF32_BUF_TYPE))
	{
		zend_error (E_WARNING, "Buffer type not a UBF/UBF32");
		RETURN_NULL ();
	}

	RETURN_STRING (Btype ((BFLDID) (*arg_field_id)->value.lval), 1);
}
/* }}} */



/************************************************************
	Functions taking 3+ arguments, a UBF resource and two plus
	other arguments.
*************************************************************/

/* {{{ function ndrx_bdel
	Accepts 3 argumens, 
	1.  UBF Resource#
	2.  Fieldid
	3.  Occurance.
	
	Returns int bdel.
*/
ZEND_FUNCTION (ndrx_bdel)
{	
	zval ** arg_ubf_ref;
	zval ** arg_field;
	zval ** arg_occ;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	BFLDID field_id;
			
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_ubf_ref,
			&arg_field,
			&arg_occ)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_occ);
	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (Bdel   ((UBFH*) ubf_buf_res->buf, (BFLDID) field_id,
							(BFLDOCC) (*arg_occ)->value.lval));
}
/* }}} */



/* {{{ function ndrx_blen
	Accepts 3 argumens, 
	1.  UBF Resource#
	2.  Fieldid
	3.  Occurance.
	
	Returns int blen
*/
ZEND_FUNCTION (ndrx_blen)
{	
	zval ** arg_ubf_ref;
	zval ** arg_field;
	zval ** arg_occ;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	BFLDID field_id;
		
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_ubf_ref,
			&arg_field,
			&arg_occ)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_occ);
	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (IS32 (Blen   ((UBFH*) ubf_buf_res->buf, (BFLDID) field_id,
					(BFLDOCC) (*arg_occ)->value.lval)));
}
/* }}} */



/* {{{ function ndrx_bpres
	Accepts 3 argumens, 
	1.  UBF Resource#
	2.  Fieldid
	3.  Occurance.
	
	Returns int bpres
*/
ZEND_FUNCTION (ndrx_bpres)
{	
	zval ** arg_ubf_ref;
	zval ** arg_field;
	zval ** arg_occ;
	

	ndrx_tpalloc_buf_type * ubf_buf_res;
	BFLDID field_id;
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_ubf_ref,
			&arg_field,
			&arg_occ)              != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_occ);
	
	ZEND_FETCH_RESOURCE(
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);


/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);


	RETURN_LONG (Bpres   ((UBFH*) ubf_buf_res->buf, 
				 (BFLDID) field_id,
				 (BFLDOCC) (*arg_occ)->value.lval));
}
/* }}} */


/* {{{ function ndrx_badd (ALIAS:  ndrx_fchg)
	function takes three (or four) arguments:
	1.  UBF buffer resource
	2.  Field ID
	3.  data value
	4.  (optional) the occurance value
	
	and returns the result from Badd(32).
*/
ZEND_FUNCTION (ndrx_badd)
{
	zval ** arg_ubf_ref;
	zval ** arg_field;
	zval ** arg_data_val;
	zval ** arg_occ_val;
	
	ndrx_tpalloc_buf_type * ubf_buf_res;
	BFLDOCC32 occ = -1;
	BFLDID field_id;
	

	switch (ZEND_NUM_ARGS ())
	{
		case 3:
			if (zend_get_parameters_ex (3, 
						&arg_ubf_ref, 
						&arg_field, 
						&arg_data_val) !=  SUCCESS)
				WRONG_PARAM_COUNT;
			break;
			
		case 4:
			if (zend_get_parameters_ex (4, 
						&arg_ubf_ref, 
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
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_ubf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((field_id = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);
	
/*
		We won't actually call Badd, but rather a Fchg with an
		occurence of -1.  It should be the same.  If we find
		its not the same its pretty easy to change it.
*/
	
	RETURN_LONG (_ndrx_ubf_add (ubf_buf_res, arg_data_val, field_id, occ));
}
/* }}} */


/* {{{ function ndrx_bget
	function takes 2 or 3 arguments:
	1.  UBF buffer resource
	2.  Field ID
	3.  (optional) the occurrence value

	returns the field value(LONG,DOUBLE,STRING, or FALSE)
*/
ZEND_FUNCTION (ndrx_bget)
{
	zval ** arg_ubf_ref;
	zval ** arg_field;
	zval ** arg_occ_val;
	int argc;
	BFLDID bfldid;
	int occ = 0;	/* first occurrence is default */
   
	ndrx_tpalloc_buf_type *ubf_buf_res;
	char *buf;
	int bublen;
	int rc;

	short short_data;
	long long_data;
	float float_data;
	double double_data;

	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 ||
		zend_get_parameters_ex(argc,
			&arg_ubf_ref, &arg_field, &arg_occ_val) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

    /*
        Now get the buffer from the argument.
    */
    ZEND_FETCH_RESOURCE(
                        ubf_buf_res,
                        ndrx_tpalloc_buf_type *,
                        arg_ubf_ref,
                        -1,
                        "Endurox tpalloc buffer",
                        ndrx_rh_alloc_buffer);

	if ((is32 = _ndrx_is_ubf_type (ubf_buf_res->type)) == -1) {
		zend_error(E_WARNING, "resource is not ubf type");
		RETURN_FALSE;
	}

/*
	what type of second argument did they give us?
	Convert if needed.
*/
	if ((bfldid = _get_arg_field_id (arg_field, 2)) == -1)
		RETURN_LONG (-1);

/*
	Check for a third arg.
*/
	if (argc == 3) {
		convert_to_long_ex(arg_occ_val);
		occ = Z_LVAL_PP(arg_occ_val);
	}

	/* get length of the field value  for memory allocation */
	bublen = Blen  ((UBFH *)ubf_buf_res->buf, (BFLDID)bfldid, (BFLDOCC)occ);

	if (bublen == -1)	/* not found the field occurrence */
		RETURN_FALSE;

	if ((buf = emalloc(bublen)) == NULL) {
		zend_error(E_WARNING, "unable to allocate %d bytes of memory", bublen);
		RETURN_FALSE;
	}

	rc = Bget  ((UBFH *)ubf_buf_res->buf, (BFLDID)bfldid, (BFLDOCC)occ, buf, NULL);

	if (rc == -1)
		RETURN_FALSE;

	switch (Bfldtype((BFLDID) bfldid)) {
	case BFLD_SHORT:
		memcpy(&short_data, buf, sizeof(short));
		RETVAL_LONG(short_data);
		break;
	case BFLD_LONG:
		memcpy(&long_data, buf, sizeof(long));
		RETVAL_LONG(long_data);
		break;
	case BFLD_FLOAT:
		memcpy(&float_data, buf, sizeof(float));
		RETVAL_DOUBLE((double)float_data);
		break;
	case BFLD_DOUBLE:
		memcpy(&double_data, buf, sizeof(double));
		RETVAL_DOUBLE(double_data);
		break;
	case BFLD_CHAR:
	case BFLD_STRING:
		RETVAL_STRING(buf, 1);
		break;
	case BFLD_CARRAY:
		RETVAL_STRINGL(buf, bublen, 1);
		break;
	default:
		RETVAL_FALSE;
		break;
	}
	efree(buf);
}
/* }}} */


/* {{{ function ndrx_bfprint
	function calls Bfprint
	Acceps 2 or 3 arguments,
		UBF Resource#
		output file path
		(optional) file open mode ("a" is default)
	Returns int similar to Bfprint.

ZEND_FETCH_RESOURCE(fp, FILE *, zvalue, -1, "File-Handle", php_file_le_fopen());.
*/
ZEND_FUNCTION (ndrx_bfprint)
{
	zval **arg_ubf_ref;
	zval **arg_file_ref;

	int argc;
	ndrx_tpalloc_buf_type *ubf_buf_res;
	FILE *fp = NULL;
	int rc, file_type;

	argc = ZEND_NUM_ARGS();
	if (argc != 2 || 
		(zend_get_parameters_ex(argc,
		&arg_ubf_ref, &arg_file_ref) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(			/* fetch the Endurox resource */
						ubf_buf_res,
						ndrx_tpalloc_buf_type *,
						arg_ubf_ref,
						-1,
						"Endurox tpalloc buffer",
						ndrx_rh_alloc_buffer);


/*
		We need to use the raw function call because its
		much easier to add resources as they are needed.
		
		Currently, only flat files and pipes seem to work.
		I tried a fsockopen call and it core dumps inside
		Bfprint32 so we will leave streams and sockets out for now.
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


	if (_ndrx_is_ubf_type(ubf_buf_res->type) == -1) {
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

	rc = Bfprint((UBFH *)ubf_buf_res->buf, fp);

	RETURN_LONG (rc);
}
/* }}} */


#endif
/*	end #if HAVE_ENDUROX */
#endif
