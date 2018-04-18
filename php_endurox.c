/*
	===============================================================|
	|   PHP Endurox                                                 |
	|--------------------------------------------------------------|
	|  php_endurox.c                                                |
	|    Main entry point to external module.                      |
	===============================================================|
*/
/* $Id: php_endurox.c,v 1.11 2001/12/26 03:50:41 bfoddy Exp $ */



#include <stdlib.h>
#include <string.h>
/* include the endurox headers */
#include <atmi.h>
#include <ubf.h>

/* include standard php header */

#include "php.h"
#include "php_endurox.h"

#if HAVE_ENDUROX

/* True globals, no need for thread safety */
int ndrxph_rh_alloc_buffer = 0;  /* tpalloc buffer resource type resource handle*/

/* 
	ZEND_FUNCTION declarations located in php_endurox.h
*/



/*
	Reference:  http://www.zend.com/apidoc/x517.php
	compiled function list so Zend knows what's in this module.
	
	This array must include all the functions including the
	UBF functions in php_endurox_ubf.c
*/
zend_function_entry endurox_functions[] =
{
	ZEND_FE (ndrxph_tpstrerror, NULL)
	ZEND_FE (ndrxph_get_tperrno, NULL)
	ZEND_FE (ndrxph_tpfree, NULL)
	ZEND_FE (ndrxph_tpalloc, NULL)
	ZEND_FE (ndrxph_put_buf, NULL)
	ZEND_FE (ndrxph_get_buf, NULL)
	ZEND_FE (ndrxph_tpcall, NULL)
	ZEND_FE (ndrxph_tpacall, NULL)
	ZEND_FE (ndrxph_tpgetrply, NULL)
	ZEND_FE (ndrxph_tpclose, NULL)
	ZEND_FE (ndrxph_tpterm, NULL)
	ZEND_FE (ndrxph_tpinit, NULL)
	ZEND_FE (ndrxph_tpabort, NULL)
	ZEND_FE (ndrxph_tpcommit, NULL)
	ZEND_FE (ndrxph_tpbegin, NULL)
	ZEND_FE (ndrxph_array2ubf, NULL)
	ZEND_FE (ndrxph_ubf2array, NULL)
/*		Functions in php_endurox_ubf_api.c */
	ZEND_FE (ndrxph_bfprintf, NULL)
	ZEND_FE (ndrxph_get_ferrno, NULL)
	ZEND_FE (ndrxph_bstrerror, NULL)
	ZEND_FE (ndrxph_bfldid, NULL)
	ZEND_FE (ndrxph_bfldno, NULL)
	ZEND_FE (ndrxph_bfldtype, NULL)
	ZEND_FE (ndrxph_bfname, NULL)
	ZEND_FE (ndrxph_badd, NULL)
	ZEND_FALIAS (ndrxph_fchg, ndrxph_badd, NULL)  /* alias for badd */
	ZEND_FE (ndrxph_bconcat, NULL)
	ZEND_FE (ndrxph_bcpy, NULL)
	ZEND_FE (ndrxph_bdelall, NULL)
	ZEND_FE (ndrxph_bdel, NULL)
	ZEND_FE (ndrxph_bidxused, NULL)
	ZEND_FE (ndrxph_bisubf, NULL)
	ZEND_FE (ndrxph_blen, NULL)
	ZEND_FE (ndrxph_bmkfldid, NULL)
	ZEND_FE (ndrxph_boccur, NULL)
	ZEND_FE (ndrxph_bpres, NULL)
	ZEND_FE (ndrxph_btype, NULL)
	ZEND_FE (ndrxph_bunindex, NULL)
	ZEND_FE (ndrxph_bunused, NULL)
	ZEND_FE (ndrxph_bupdate, NULL)
	ZEND_FE (ndrxph_bused, NULL)
	ZEND_FE (ndrxph_brstrindex, NULL)
	ZEND_FE (ndrxph_bsizeof, NULL)
	ZEND_FE (ndrxph_bget, NULL)
	ZEND_FE (ndrxph_bfprint, NULL)
	{NULL, NULL, NULL}	/* end of structure marker */
};



/*
	Reference:  http://www.zend.com/apidoc/x629.php
	Declare the Zend Module block
	zend_module_entry mysql_module_entry = {
	"mysql", mysql_functions, PHP_MINIT(mysql), PHP_MSHUTDOWN(mysql), PHP_RINIT(mysql), NULL, 
			 PHP_MINFO(mysql), STANDARD_MODULE_PROPERTIES
};

	function called on module initialization.
*/
ZEND_MINIT_FUNCTION (endurox)
{
#define CONSTANT_FLAG (CONST_CS | CONST_PERSISTENT)	
	
	ndrxph_rh_alloc_buffer = register_list_destructors(free_ndrxph_tpalloc_buf, NULL);

/*
		Lets register some endurox constants.
*/
	REGISTER_LONG_CONSTANT ("NDRXPH_STRING", NDRXPH_STRING_BUF_TYPE, CONSTANT_FLAG );
	REGISTER_LONG_CONSTANT ("NDRXPH_CARRAY", NDRXPH_CARRAY_BUF_TYPE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT ("NDRXPH_UBF", NDRXPH_UBF_BUF_TYPE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT ("NDRXPH_UBF32", NDRXPH_UBF32_BUF_TYPE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT ("NDRXPH_VIEW", NDRXPH_VIEW_BUF_TYPE, CONSTANT_FLAG);
	
	REGISTER_LONG_CONSTANT("NDRXPH_TPNOTRAN", TPNOTRAN, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPNOCHANGE", TPNOCHANGE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPNOBLOCK", TPNOBLOCK, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPNOTIME", TPNOTIME, CONSTANT_FLAG); 
	REGISTER_LONG_CONSTANT("NDRXPH_TPSIGRSTRT", TPSIGRSTRT, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPGETANY", TPGETANY, CONSTANT_FLAG);
/*
		Error return codes.  In case someone wants to check them.
*/
	REGISTER_LONG_CONSTANT("NDRXPH_TPMINVAL", TPMINVAL, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEABORT", TPEABORT, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEBADDESC", TPEBADDESC, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEBLOCK", TPEBLOCK, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEINVAL", TPEINVAL, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPELIMIT", TPELIMIT, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPENOENT", TPENOENT, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEOS", TPEOS, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEPERM", TPEPERM, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEPROTO", TPEPROTO, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPESVCERR", TPESVCERR, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPESVCFAIL", TPESVCFAIL, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPESYSTEM", TPESYSTEM, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPETIME", TPETIME, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPETRAN", TPETRAN, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPGOTSIG", TPGOTSIG, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPERMERR", TPERMERR, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEITYPE", TPEITYPE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEOTYPE", TPEOTYPE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPERELEASE", TPERELEASE, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEHAZARD", TPEHAZARD, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEHEURISTIC", TPEHEURISTIC, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEEVENT", TPEEVENT, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEMATCH", TPEMATCH, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEDIAGNOSTIC", TPEDIAGNOSTIC, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPEMIB", TPEMIB, CONSTANT_FLAG);
	REGISTER_LONG_CONSTANT("NDRXPH_TPMAXVAL", TPMAXVAL, CONSTANT_FLAG);

	
	return SUCCESS;
}




/*
	function called on module shutdown
*/
ZEND_MSHUTDOWN_FUNCTION (endurox)
{
	tpterm ();

	return SUCCESS;
}


/*
	function called on request startup
*/
ZEND_RINIT_FUNCTION (endurox)
{
/*
		In case the previous ended in an error, zero them out.
*/
	tperrno = 0;
	Berror = 0;
	return SUCCESS;
}



/*
	function called on request shutdown
*/
ZEND_RSHUTDOWN_FUNCTION (endurox)
{
	tpterm ();
	return SUCCESS;
}


/*
	function called on phpinfo ()
*/
ZEND_MINFO_FUNCTION (endurox)
{
	char env [1000];
	
	php_info_print_table_start();
	php_info_print_table_header(2, "Endurox Variable", "Value");
	
	php_info_print_table_row (2, "PHP-Endurox Version", _php_ndrxph_ver);

	if (getenv ("NDRXDIR") == NULL)
		strcpy (env, "");
	else
		strcpy (env, getenv ("NDRXDIR"));
		
	php_info_print_table_row(2, "NDRXDIR", env);
	
	if (getenv ("WSNADDR") == NULL)
		strcpy (env, "");
	else
		strcpy (env, getenv ("WSNADDR"));
		
	php_info_print_table_row (2, "WSNADDR", env);


	if (getenv("FLDTBLDIR") == NULL)
		strcpy (env, "");
	else
		strcpy (env, getenv ("FLDTBLDIR"));

	php_info_print_table_row (2, "FLDTBLDIR", env);

		
	if (getenv("FLDTBLDIR32") == NULL)
		strcpy (env, "");
	else
		strcpy (env, getenv ("FLDTBLDIR32"));

	php_info_print_table_row (2, "FLDTBLDIR32", env);



	if (getenv("FIELDTBLS") == NULL)
		strcpy (env, "");
	else
		strcpy (env, getenv ("FIELDTBLS"));

	php_info_print_table_row (2, "FIELDTBLS", env);

		
	if (getenv("FIELDTBLS32") == NULL)
		strcpy (env, "");
	else
		strcpy (env, getenv ("FIELDTBLS32"));

	php_info_print_table_row (2, "FIELDTBLS32", env);

#if NDRXPH_UBF
	strcpy (env, "Enabled");
#else
	strcpy (env, "Disabled");
#endif
	php_info_print_table_row (2, "UBF Functions", env);
	
#if NDRXPH_UBF32
	strcpy (env, "Enabled");
#else
	strcpy (env, "Disabled");
#endif
	php_info_print_table_row (2, "UBF32 Functions", env);
	
	php_info_print_table_end();


}



zend_module_entry endurox_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ATR Baltic Endurox Module",				/* the name of these modules */
	endurox_functions,					/* points to zend_function_entry */
	ZEND_MINIT (endurox),				/* module startup function */
	ZEND_MSHUTDOWN (endurox),			/* module shutdown function */
	ZEND_RINIT (endurox),				/* request startup function */
	ZEND_RSHUTDOWN (endurox),			/* request shutdown function */
	ZEND_MINFO (endurox),				/* phpinfo callback function */
#if ZEND_MODULE_API_NO >= 20010901
	NO_VERSION_YET,
#endif
	STANDARD_MODULE_PROPERTIES  /* sets defaults for global startup/shutdown */
};

/*
	implement standard "stub" routine to introduce ourselves to Zend 
	the argument must be the xxxx of xxxx_module_entry line above.
*/

#if COMPILE_DL_ENDUROX_MODULE
ZEND_GET_MODULE(endurox)
#endif


/*
	Function to free the tpalloc resource buffer.
*/
static void free_ndrxph_tpalloc_buf(ndrxph_tpalloc_buf_type * res)
{	
	tpfree (res->buf);		/* free the tpalloc buffer */
	efree (res);			/* now free the whole structure */
}


/*
	Now lets start with the good stuff.

	The standard function definition would look like:
	
	ZEND_FUNCTION(first_module)
	{
		zval **param;

		if((ZEND_NUM_ARGS() != 1) || 
			(zend_get_parameters_ex(1, &param) != SUCCESS))
		{
			WRONG_PARAM_COUNT;
		}

		convert_to_long_ex(param);

		RETURN_LONG((*param)->value.lval);
	}
*/

/* {{{ function ndrxph_tpstrerror
	Function to return a string containing a endurox error description.
	Function accepts one argument, tperrno.

*/
ZEND_FUNCTION (ndrxph_tpstrerror)
{
	zval **arg_tperrno;

	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, &arg_tperrno) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_tperrno);

	RETURN_STRING (tpstrerror ((*arg_tperrno)->value.lval), 1);
}
/* }}} */


/* {{{ function tp_get_tperrno
	Function to return the numeric value of tperrno.
	This value will most likely be used to call ndrxph_tpstrerror.
	Function accepts no arguments.
*/
ZEND_FUNCTION (ndrxph_get_tperrno)
{
	if (ZEND_NUM_ARGS () != 0)
	{
		WRONG_PARAM_COUNT;
	}
	
	RETURN_LONG (tperrno);
}
/* }}} */


/* {{{ function ndrxph_tpfree
	Function to free a tpalloc'ed buffer.
	Function accepts one argument, the buf resource of the buffer to free.
	Function returns nothing.
*/
ZEND_FUNCTION (ndrxph_tpfree)
{
	zval ** arg_tpalloc_res;

	ndrxph_tpalloc_buf_type * tp_alloc_res;
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, &arg_tpalloc_res) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	if (((*arg_tpalloc_res)->type != IS_RESOURCE) || ((*arg_tpalloc_res)->value.lval==0))
	{
		zend_error (E_WARNING, "ndrxph_tpfree argument not a tpalloc resource");
		RETURN_FALSE;
	}

	zend_list_delete((*arg_tpalloc_res)->value.lval);

	RETURN_NULL ();
}
/* }}} */


/* {{{ function ndrxph_tpalloc
	Function to tpalloc a typed buffer.
	Function requires 3 arguments,
		TYPE = type type of buffer they are requesting.
		SUBTYPE = aux info related to TYPE.
		SIZE = the initial size of the request.
	Function reutrns a buffer reference number.
*/
ZEND_FUNCTION (ndrxph_tpalloc)
{
	zval ** arg_buf_type;
	zval ** arg_buf_subtype;
	zval ** arg_buf_size;

	

	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_buf_type,
			&arg_buf_subtype,
			&arg_buf_size) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_buf_type);
	convert_to_string_ex (arg_buf_subtype);
	convert_to_long_ex (arg_buf_size);
			
	RETURN_RESOURCE (_ndrxph_alloc ((*arg_buf_type)->value.lval,
								 (*arg_buf_subtype)->value.str.val,
								 (*arg_buf_size)->value.lval));
}
/* }}} */

/*
	This function actually does the tpalloc for both the
	ndrxph_tpalloc call and Falloc for UBFs.

	Returns a resource (value  currently a LONG).
*/
long _ndrxph_alloc (long type, char * subtype, long size)
{
	char *	buf_ptr;
	int is32 = 0;
	ndrxph_tpalloc_buf_type * tp_alloc_res;

/*
		Has someone given us an invalid type 
*/

	if ((type < 0) || (type >= NDRXPH_NUM_BUF_TYPES))
	{
		zend_error (E_WARNING, "Invalid buffer type");
		return 0;
	}

#if (! NDRXPH_UBF)
	if (type == NDRXPH_UBF_BUF_TYPE)
	{
		zend_error (E_ERROR, "UBF buffer support not configured.  reconfigure with --enable-endurox-ubf");
		return 0;
	}
#endif

#if (! NDRXPH_UBF32)
	if (type == NDRXPH_UBF32_BUF_TYPE)
	{
		zend_error (E_ERROR, "UBF32 buffer support not configured.  reconfigure with  --enable-endurox-ubf32");
		return 0;
	}
#endif


/*
		Allocate the buffer.
*/	
	if ((buf_ptr = (char*) tpalloc (_ndrxph_type2string(type), subtype, size)) == NULL)
	{
		zend_error (E_WARNING, "Endurox tpalloc failed [%s]", tpstrerror (tperrno));
		return 0;
	}


	if (type == NDRXPH_UBF_BUF_TYPE)
		is32 = FALSE;
	else if (type == NDRXPH_UBF32_BUF_TYPE)
		is32 = TRUE;
		
/*
		Now allocate the resource buffer 
*/
	tp_alloc_res = (ndrxph_tpalloc_buf_type *) emalloc (sizeof (ndrxph_tpalloc_buf_type));
	memset (tp_alloc_res, 0, sizeof (ndrxph_tpalloc_buf_type));

/*
		Move the resource values into the res buf.
*/
	tp_alloc_res->buf = buf_ptr;
	tp_alloc_res->size = size;
	
	tp_alloc_res->type = type; 	/* set the buffer type */
	
	strncpy (tp_alloc_res->subtype, subtype, 
		MIN (strlen(subtype), NDRXPH_TPALLOC_TYPE_LEN));

/*
		I cant use the RETURN_ macro because I haven't define the return_value
		structure.
*/
	return zend_list_insert(tp_alloc_res, ndrxph_rh_alloc_buffer);
}



/* {{{ function tx_put_buf
	Function to copy information into a buffer.
	Function requires 2 arguments:
		Buffer Reference = the destination reference buffer 
		Input = The provided text.
	Function returns BOOL true or false on error.
*/
ZEND_FUNCTION (ndrxph_put_buf)
{
	zval ** arg_tpalloc_res;
	zval ** arg_buf;

	long arg_length;
	ndrxph_tpalloc_buf_type * tp_alloc_res;
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2, &arg_tpalloc_res,
								&arg_buf)) != SUCCESS)
	{
		WRONG_PARAM_COUNT;
	}

	
	
	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						tp_alloc_res, 
						ndrxph_tpalloc_buf_type *, 
						arg_tpalloc_res, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrxph_rh_alloc_buffer);

/*
		We use a memcpy here for the future hopes of being able to
		handle a CARRAY or VIEW type seemlessly.  May not work.
*/
	
	arg_length = (*arg_buf)->value.str.len;
	
/*
		Is the value too long for the provided buffer?
		If so realloc it.
*/
	if (arg_length > tp_alloc_res->size)
	{
		char * new_buf;
		
		zend_error (E_WARNING, "Expanding buffer");
		new_buf = tprealloc (tp_alloc_res->buf, arg_length+1);
		if (new_buf == NULL)
		{
			zend_error (E_ERROR, "Reallocation of buffer failed");
			RETURN_FALSE;
		}
		
		tp_alloc_res->buf = new_buf;
		tp_alloc_res->size = arg_length+1;
	}	
	memcpy (tp_alloc_res->buf, (*arg_buf)->value.str.val, arg_length);
	tp_alloc_res->buf[arg_length+1] = 0;	/* null term a possible string */
		
	RETURN_TRUE ;
}
/* }}} */


/* {{{ function ndrxph_get_buf
	Function to return a string equivalent to the 
	reference buffer provided.
	Function requires 1 argument, the buffer reference number.
*/
ZEND_FUNCTION (ndrxph_get_buf)
{
	zval ** arg_tpalloc_res;

	ndrxph_tpalloc_buf_type * tp_alloc_res;
	
	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, &arg_tpalloc_res) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

/*
	if (((*arg_tpalloc_res)->type != IS_RESOURCE) || ((*arg_tpalloc_res)->value.lval==0))
	{
		zend_error (E_WARNING, "ndrxph_get_buf argument not a tpalloc resource");
		RETURN_EMPTY_STRING ();
	}
*/
	
	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						tp_alloc_res, 
						ndrxph_tpalloc_buf_type *, 
						arg_tpalloc_res, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrxph_rh_alloc_buffer);

	
	RETURN_STRINGL (tp_alloc_res->buf, tp_alloc_res->size,  1);  /* binary safe copy */	
}
/* }}} */



/* {{{ function ndrxph_tpcall
	Function to submit via tpcall the endurox request.
	Requires 4 arguments:
		String containing the service to call.
		Buf ref # for the submit.
		Buf ref # for the reply.
		Flags.
	Function returns a 0 or an error condition in tperrno.
*/
		
ZEND_FUNCTION (ndrxph_tpcall)
{
	zval ** arg_service;
	zval ** arg_tpalloc_res_in;
	zval ** arg_tpalloc_res_out;
	zval ** arg_flags;
	
	int tpcall_return;

	ndrxph_tpalloc_buf_type * tp_alloc_res_in;
	ndrxph_tpalloc_buf_type * tp_alloc_res_out;
	ndrxph_tpalloc_buf_type temp_alloc_buf;		/* not a pointer */
	
	
	
	if((ZEND_NUM_ARGS() != 4) || 
		(zend_get_parameters_ex(4,
			&arg_service,
			&arg_tpalloc_res_in,
			&arg_tpalloc_res_out,
			&arg_flags) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

/*
	if (((*arg_tpalloc_res_in)->type != IS_RESOURCE) || ((*arg_tpalloc_res_in)->value.lval==0))
	{
		zend_error (E_WARNING, "ndrxph_tpcall argument not a tpalloc resource");
		RETURN_FALSE;
	}
*/

	convert_to_string_ex (arg_service);
	convert_to_long_ex (arg_flags);

	/*
		Now get the buffers from the arguments.
	*/

	ZEND_FETCH_RESOURCE(
						tp_alloc_res_in, 
						ndrxph_tpalloc_buf_type *, 
						arg_tpalloc_res_in, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrxph_rh_alloc_buffer);


	ZEND_FETCH_RESOURCE(
						tp_alloc_res_out, 
						ndrxph_tpalloc_buf_type *, 
						arg_tpalloc_res_out, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrxph_rh_alloc_buffer);

/*
	We need to copy the output buf to see if the call to tpcall made any changes.
*/
	temp_alloc_buf.buf = tp_alloc_res_out->buf;
	temp_alloc_buf.size = tp_alloc_res_out->size;
		
/*
	int tpcall(char *svc, char *idata, long ilen, char **odata, long *olen, long flags)
*/
	tpcall_return =
		tpcall ((*arg_service)->value.str.val,
				tp_alloc_res_in->buf,
				tp_alloc_res_in->size,
				&temp_alloc_buf.buf,
				&temp_alloc_buf.size,
				(*arg_flags)->value.lval);
				
	if ((temp_alloc_buf.buf != tp_alloc_res_out->buf) ||
		(temp_alloc_buf.size != tp_alloc_res_out->size))
	{
		tp_alloc_res_out->buf = temp_alloc_buf.buf;
		tp_alloc_res_out->size = temp_alloc_buf.size;
	}

	RETURN_LONG (tpcall_return);
}
/* }}} */


/* {{{  function ndrxph_tpacall
	Function to submit via tpacall the endurox request.
	Requires 3 arguments:
		String containing the service to call.
		Buf ref # for the submit.
		Flags.
	Function returns cd reference # or an error condition in tperrno.
*/
ZEND_FUNCTION (ndrxph_tpacall)
{
	zval ** arg_service;
	zval ** arg_tpalloc_res;
	zval ** arg_flags;

	ndrxph_tpalloc_buf_type * tp_alloc_res;
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3,
			&arg_service, 
			&arg_tpalloc_res,
			&arg_flags) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

/*
	if (((*arg_tpalloc_res)->type != IS_RESOURCE) || ((*arg_tpalloc_res)->value.lval==0))
	{
		zend_error (E_WARNING, "ndrxph_tpacall argument not a tpalloc resource");
		RETURN_FALSE;
	}
*/

	convert_to_string_ex (arg_service);
	convert_to_long_ex (arg_flags);

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						tp_alloc_res, 
						ndrxph_tpalloc_buf_type *, 
						arg_tpalloc_res, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrxph_rh_alloc_buffer);

	RETURN_LONG (tpacall ((*arg_service)->value.str.val,
				  tp_alloc_res->buf,
				  tp_alloc_res->size,
				  (*arg_flags)->value.lval));
}
/* }}} */




/* {{{  function ndrxph_tpgetreply
	Function to retrieve the results from a previous tpacall
	Requires 3 arguments:
		cd reference number returned by tpacall
		Buf ref # for the reply.
		Flags.
	Function returns service return value (tpreturn) or an error code
*/
ZEND_FUNCTION (ndrxph_tpgetrply)
{
	zval ** arg_tpacall_cd_ref;
	zval ** arg_tpalloc_res;
	zval ** arg_flags;

	ndrxph_tpalloc_buf_type * tp_alloc_res;
	long ret_val;
	int cd;
	
	
	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3,
			&arg_tpacall_cd_ref, 
			&arg_tpalloc_res,
			&arg_flags) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}


/* check for arg_tpacall_cd_ref being passed by reference */
#if ZEND_MODULE_API_NO < 20010901
	if(!ParameterPassedByReference(ht, 1))
	{
		zend_error(E_ERROR, "Argument 1 needs to be passed as a reference");
		RETURN_NULL();
	}
#endif

	convert_to_long_ex (arg_flags);

	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						tp_alloc_res, 
						ndrxph_tpalloc_buf_type *, 
						arg_tpalloc_res, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrxph_rh_alloc_buffer);

	cd = (*arg_tpacall_cd_ref)->value.lval;
	ret_val = tpgetrply (&cd, &tp_alloc_res->buf, &tp_alloc_res->size, (*arg_flags)->value.lval);
	
	(*arg_tpacall_cd_ref)->type = IS_LONG;	/* should already be a long */
	(*arg_tpacall_cd_ref)->value.lval = cd;  /* put in the new value */
	
	RETURN_LONG (ret_val);
}
/* }}} */



/* {{{  function ndrxph_tx_close
	Function closes current endurox connection.
	No arguments.
	No return value.
*/
ZEND_FUNCTION (ndrxph_tpclose)
{
	if (ZEND_NUM_ARGS () != 0)
	{
		WRONG_PARAM_COUNT;
	}
	
	tpclose ();
	RETURN_NULL ();
}
/* }}} */



/* {{{  function ndrxph_tpterm
	Function closes current endurox connection.
	No arguments.
	No return value.
*/
ZEND_FUNCTION (ndrxph_tpterm)
{
	if (ZEND_NUM_ARGS () != 0)
	{
		WRONG_PARAM_COUNT;
	}
	
	RETURN_LONG (tpterm ());
}
/* }}} */



/* {{{  function ndrxph_tpinit
	Function opens a ndrx connection and populates connection fields
	4 or 5 arguments:
		username
		client name
		password
		flags	
		(optional) data -  user password for Endurox Security level(USER_AUTH,ACL)
	Return TRUE FALSE.
*/
ZEND_FUNCTION (ndrxph_tpinit)
{
	zval ** arg_username;
	zval ** arg_clientname;
	zval ** arg_password;
	zval ** arg_flags;
	zval ** arg_data;

	TPINIT * tpinit_buf;  /* holder for the tpalloc buffer, only for this function*/
	long return_val;
	int argc;
	long bublen;
	long datalen;

	argc = ZEND_NUM_ARGS();
	
	if(argc < 4 || argc > 5 || 
		(zend_get_parameters_ex(argc,
			&arg_username, 
			&arg_clientname,
			&arg_password,
			&arg_flags,
			&arg_data) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex (arg_username);
	convert_to_string_ex (arg_clientname);
	convert_to_string_ex (arg_password);
	convert_to_long_ex (arg_flags);
	if (argc == 5) convert_to_string_ex (arg_data);

/*
		Allocate a temp buffer
*/
	bublen = sizeof(TPINIT);
	tpinit_buf = (TPINIT*) tpalloc ("TPINIT", NULL, bublen);
	if (tpinit_buf == (TPINIT *)NULL)
	{
		zend_error (E_ERROR, "tpalloc failed for tpinit");
		RETURN_FALSE;
	}

/*
		Copy in the parameter values.
*/
	memset (tpinit_buf->usrname, 0, MAXTIDENT+2);
	memset (tpinit_buf->grpname, 0, MAXTIDENT+2);
	memset (tpinit_buf->cltname, 0, MAXTIDENT+2);
	memset (tpinit_buf->passwd, 0, MAXTIDENT+2);

	strncpy (tpinit_buf->usrname, (*arg_username)->value.str.val, 
			MIN((*arg_username)->value.str.len, MAXTIDENT));

	strncpy (tpinit_buf->cltname, (*arg_clientname)->value.str.val,
			MIN((*arg_clientname)->value.str.len, MAXTIDENT));
			
	strncpy (tpinit_buf->passwd, (*arg_password)->value.str.val,
			MIN((*arg_password)->value.str.len, MAXTIDENT));
			
	tpinit_buf->flags = (*arg_flags)->value.lval;

	if (argc == 5) {
		memcpy((char *)&tpinit_buf->data, Z_STRVAL_PP(arg_data), ((*arg_data)->value.str.len));
		tpinit_buf->datalen = (*arg_data)->value.str.len;
	} else {
		tpinit_buf->datalen = 0;
		tpinit_buf->data = 0;
	}
	
	
	return_val = tpinit (tpinit_buf);
	if (return_val == -1)
	{
		zend_error (E_WARNING, "TPINIT failed %s", tpstrerror (tperrno));
	}	
	tpfree ((char*)tpinit_buf);
	
	RETURN_LONG (return_val);
}
/* }}} */



/* {{{ function ndrxph_tpabort
	function to call the tpabort ndrx function
	
	function takes 0 args
	function returns long.
*/
ZEND_FUNCTION (ndrxph_tpabort)
{
	if(ZEND_NUM_ARGS() != 0) 
	{
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG (tpabort (0l));
}
/* }}} */



/* {{{ function ndrxph_tpcommit
	function to call the tpcommit ndrx function
	
	function takes 0 args
	function returns long.
*/
ZEND_FUNCTION (ndrxph_tpcommit)
{
	if(ZEND_NUM_ARGS() != 0) 
	{
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG (tpcommit (0l));
}
/* }}} */



/* {{{ function ndrxph_tpbegin
	function to call the tpbegin ndrx function

	function takes 1 args, the timeout, must not be 0.
	function returns long.
*/
ZEND_FUNCTION (ndrxph_tpbegin)
{
	zval ** arg_timeout;

	if((ZEND_NUM_ARGS() != 1) || 
		(zend_get_parameters_ex(1, &arg_timeout) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg_timeout);
	
	if ((*arg_timeout)->value.lval == 0)
	{
		zend_error (E_WARNING, 
			"ndrxph_commit:  timeout argument cannot be zero,  using 30.");
		(*arg_timeout)->value.lval = 30;
	}
	
	RETURN_LONG (tpbegin ((*arg_timeout)->value.lval, 0l));
}
/* }}} */


/*	end #if HAVE_ENDUROX */
#endif
