/*
	===============================================================|
	|   PHP Tuxedo                                                 |
	|--------------------------------------------------------------|
	|  php_tuxedo_arrayfml.c                                       |
	|    Contains functions for the array to fml functions         |
	===============================================================|
*/
/* $Id: php_tuxedo_arrayfml.c,v 1.6 2002/01/07 05:36:41 bfoddy Exp $ */


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
#if (TUX_FML32 || TUX_FML)

/* True globals, no need for thread safety */
extern int tux_rh_alloc_buffer;  /* tpalloc buffer resource type resource handle*/



/* 
	ZEND_FUNCTION declarations located in php_tuxedo.h
*/

/* {{{ function tux_falloc
	Function mimics falloc or falloc32.  Actually calls
	Fneeded and Feeded32 then calls
	_tux_alloc which does a tpalloc followed by a Finit.
	
	Returns reference to buffer.
*/
ZEND_FUNCTION (tux_falloc)
{	
	zval ** arg_buf_type;
	zval ** arg_buf_fldocc;
	zval ** arg_buf_fldlen;

	long size_needed;
	int is32;
	

	if((ZEND_NUM_ARGS() != 3) || 
		(zend_get_parameters_ex(3, 
			&arg_buf_type,
			&arg_buf_fldocc,
			&arg_buf_fldlen) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_buf_type);
	convert_to_long_ex (arg_buf_fldocc);
	convert_to_long_ex (arg_buf_fldlen);
	
/*
		Check to see if its a FML or FML32.
*/
	if ((is32 = _tux_is_fml_type ((*arg_buf_type)->value.lval)) == -1)
		RETURN_NULL ();

	
	size_needed = IS32 (is32, 
						Fneeded32 ((*arg_buf_fldocc)->value.lval,
								     (*arg_buf_fldlen)->value.lval),
						Fneeded ((*arg_buf_fldocc)->value.lval,
							  	   (*arg_buf_fldlen)->value.lval));

/*
		Now that we have the right size, call the generic alloc function
		with the type and size.
*/
	RETURN_RESOURCE (_tux_alloc ((*arg_buf_type)->value.lval, "", size_needed));
}
/* }}} */



/* {{{ function tux_arrary2fml
	Function takes 2 arguments:
		1.  The destination FML reference number 
		2.  an array argument (possibly 2 dimensional)
	and inserts values into a FML/FML32 buffer according to the
	index value.
	
	Design note:  I though some of making this function do a recursive call
	to move through N layers of a array, but the FML buffer can only take
	two dimensions so we will limit this function to 2 without recursion.
	
	Returns TRUE/FALSE.
*/
ZEND_FUNCTION (tux_array2fml)
{
	zval ** arg_buf_ref;
	zval ** arg_src_array;

	int is32;
	
	tux_tpalloc_buf_type * fml_buf_res;
	HashTable * primary_ht;
	HashTable * secondary_ht;
	zval **     primary_data;
	long		primary_key;
	zval **     secondary_data;
	long		secondary_key;
	char * 		dummy;	/* just a dummy string pointer */



	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2,
			&arg_buf_ref,
			&arg_src_array) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_array_ex (arg_src_array);
	
	/*
		Now get the buffer from the argument.
	*/

	ZEND_FETCH_RESOURCE(
						fml_buf_res, 
						tux_tpalloc_buf_type *, 
						arg_buf_ref, 
						-1, 
						"Tuxedo tpalloc buffer", 
						tux_rh_alloc_buffer);
/*
	Ok the plan of action...
	Loop through the array element by element.
		
	If the primary is a text, its assumed to be a Field name
	and needs converting to its Fieldid before use.

	This function primarily uses the following API calls, which
	aren't very well documented:
	
	zend_hash_move_forward (*ht) simply set an internal pointer to the next record
			in the array.  Return TRUE or FALSE (no more records). 

	zend_hash_get_current_key   -- get the index (string or long) of current record
			(*ht,						-- pointer to hash table
			char **str_index, 			-- if string index, pointer to the char *
			ulong *str_length,			-- if string index, length of string index
			ulong *num_index, 			-- if int index, value of index
			HashPosition *pos)			-- only for _ex call, ht position.
		returns HASH_KEY_IS_STRING or HASH_KEY_IS_LONG, or HASH_KEY_NON_EXISTANT
		
	zend_hash_get_current_data  -- get the current records data.
			(*ht,						-- pointer to hash table
			void **pData,				-- pointer to Bucket struct  (**pval)
			HashPosition *pos)			-- pointer to position  (only for _ex call).
		returns SUCCESS or FAILURE

	zend_hash_internal_pointer_reset(*ht)  -- resets array pointer to start
*/

	if (fml_buf_res->type == TUX_FML_BUF_TYPE)
	{
		is32 = FALSE;
	}
	else if (fml_buf_res->type == TUX_FML32_BUF_TYPE)
	{
		is32 = TRUE;
	}
	else
	{
		zend_error (E_WARNING, "Supplied buffer type is not FML/FML32");
		RETURN_FALSE;
	}
	

	primary_ht = HASH_OF (*arg_src_array);
	zend_hash_internal_pointer_reset (primary_ht);
	

	while (zend_hash_get_current_data (primary_ht, (void**) &primary_data) == SUCCESS) 
	{
/*
			First get the primary key value (could be int or string).
*/
		if ((primary_key = _tux_get_fmlarray_key (primary_ht, is32)) == -1)
		{
			RETURN_FALSE;
		}
		
/*		zend_printf ("Inside zend_hash_get_cur_data loop %d<br>", primary_key);		*/
		if ((*primary_data)->type == IS_ARRAY)  /* if another array, start another loop*/
		{
			secondary_ht = HASH_OF(*primary_data);
			while (zend_hash_get_current_data (secondary_ht, (void**) &secondary_data) == SUCCESS)
			{
/*
					If the secondary key is a string, set it to -1 to be appended,
					else use it.
*/
				if (zend_hash_get_current_key 
					(secondary_ht, &dummy, &secondary_key, 0) == HASH_KEY_IS_STRING)
				{
					secondary_key = -1;  /* it was a string */
				}

/*				zend_printf ("Inside zend_hash_get_cur_data array %d<br>", secondary_key);		*/

				if ( ! _tux_fml_add (fml_buf_res, secondary_data, primary_key, secondary_key))
				{
					/* need more error handling here... 
					   if we change the buffer, remember to write it back to the resource*/
					zend_error (E_WARNING, "Adding FML field call failed");
					RETURN_FALSE;
				}


				zend_hash_move_forward (secondary_ht);  /* next secondary record */
			}
		}
		else		/* its a normal value of some type, add it*/
		{
			if ( ! _tux_fml_add (fml_buf_res, primary_data, primary_key, -1))
			{
					/* need more error handling here...  
					   if we change the buffer, remember to write it back to the resource*/
				zend_error (E_WARNING, "Adding FML field call failed");
				RETURN_FALSE;
			}
		}
	
		zend_hash_move_forward (primary_ht);  /* next primary record */	
	}	

	RETURN_TRUE;
}
/* }}} */



/*
	This function will take a hash pointer and a buffer type, and
	return the type of the current key array.
*/
long _tux_get_fmlarray_key (HashTable * ht, int is32)
{
	long ret_val = 0;
	char * str_index;
	ulong num_index;
	
	
	switch (zend_hash_get_current_key (ht, &str_index, &num_index, 1))
	{
		case HASH_KEY_IS_LONG:	/* if its a long value, just return it */
			ret_val = num_index;
			break;
			
		case HASH_KEY_IS_STRING:	/* a string, convert with  Fldid*/
			ret_val = IS32 (is32, Fldid32 (str_index), Fldid (str_index));
			break;
	
		default:					/* How did this happen? */
			zend_error (E_WARNING, "Internal error, Invalid Hash index type");
			ret_val = -1;
	}
	efree (str_index);
	return ret_val;
}




/*
	This function takes an long key (FLDID), a pointer to a tux_tpalloc_buf_type,
	and a zval ** and inserts the data into the buffer by calling Fchg(32).

	returns TRUE/FALSE.
	
	It has to vary exection depending by what type of value data points to,
	and by what type of FML buf is.

	We call Fldtype to determine the type of field they gave us, and cast accordingly.

	Fappend(FBFR *fbfr, FLDID fieldid, char *value, FLDLEN len)
	
	FML field types  -- fml.h  these are the same for FML32 also.
		#define FLD_SHORT       0        short int 
		#define FLD_LONG        1        long int 
		#define FLD_CHAR        2        character 
		#define FLD_FLOAT       3        single-precision float 
		#define FLD_DOUBLE      4        double-precision float 
		#define FLD_STRING      5        string - null terminated 
		#define FLD_CARRAY      6        character array 
  
*/
long _tux_fml_add (tux_tpalloc_buf_type * buffer, zval ** data, FLDID32 fldid32, FLDOCC32 occ32)
{
/*
		This is a function pointer, its name F_add does not imply it points to
		Fadd, actually it uses Fchg.
*/
	FLDID fldid = (FLDID) fldid32;
	FLDOCC occ = (FLDOCC) occ32;

	int  fldid_type;
	short short_data;
	long  long_data;
	char  char_data;
	float float_data;
	double double_data;
	
	int ret_val;
	int  is32 = ((buffer->type == TUX_FML32_BUF_TYPE) ? TRUE : FALSE);	

/*
		First they passed us a fldid, find out what type it is.
*/
	fldid_type = IS32(is32, Fldtype32 (fldid32), Fldtype (fldid));
/*
		Ok, we know what type the fldid is, we need to make
		sure that the type of data they sent us is the same type.
		If not, we need to convert it.  The Fchg function
		expects a pointer to exactly the correct length variable.
*/
	switch (fldid_type)
	{
		case FLD_SHORT:		/* take a long and make it short */
			convert_to_long_ex (data);
			short_data = (short) (*data)->value.lval;
			ret_val = IS32 (is32, Fchg32((FBFR32*) buffer->buf, fldid32, occ32, (char*)&short_data, 0) ,
								  Fchg ((FBFR*) buffer->buf, fldid, occ, (char*)&short_data, 0));
			break;
			

		case FLD_LONG:		/* nothing to do here except ensure its long */
			convert_to_long_ex (data);
			long_data = (long) (*data)->value.lval;
			ret_val = IS32 (is32, Fchg32((FBFR32*) buffer->buf, fldid32, occ32, (char*)&long_data, 0) ,
								  Fchg ((FBFR*) buffer->buf, fldid, occ, (char*)&long_data, 0));
			break;
			

		case FLD_CHAR:		/* take a string and use only first character */
			convert_to_string_ex (data);
			char_data = (char) (*data)->value.str.val[0];  /* we only want the first char */
			ret_val = IS32 (is32, Fchg32((FBFR32*) buffer->buf, fldid32, occ32, (char*)&char_data, 0) ,
								  Fchg ((FBFR*) buffer->buf, fldid, occ, (char*)&char_data, 0));
			break;
			

		case FLD_FLOAT:		/* take a double and make it a float */
			convert_to_double_ex (data);
			float_data = (float) (*data)->value.dval;
			ret_val = IS32 (is32, Fchg32((FBFR32*) buffer->buf, fldid32, occ32, (char*)&float_data, 0) ,
								  Fchg ((FBFR*) buffer->buf, fldid, occ, (char*)&float_data, 0));
			break;
			

		case FLD_DOUBLE:	/* nothing to do here */
			convert_to_double_ex (data);
			double_data = (double) (*data)->value.dval;
			ret_val = IS32 (is32, Fchg32((FBFR32*) buffer->buf, fldid32, occ32, (char*)&double_data, 0) ,
								  Fchg ((FBFR*) buffer->buf, fldid, occ, (char*)&double_data, 0));
			break;


		case FLD_STRING:	/* nothing to do here */
			convert_to_string_ex (data);
			ret_val = IS32 (is32, Fchg32((FBFR32*) buffer->buf, fldid32, occ32, (char*)(*data)->value.str.val, 0) ,
								  Fchg ((FBFR*) buffer->buf, fldid, occ, (char*)(*data)->value.str.val, 0)); 
			break;


		case FLD_CARRAY:	/* assume a string with binary safe copy, dont convert */
			ret_val = IS32 (is32, Fchg32((FBFR32*)  buffer->buf, 
													fldid32, 
													occ32, 
													(char*)(*data)->value.str.val, 
													(FLDLEN32) (*data)->value.str.len) ,
								  Fchg ((FBFR*)     buffer->buf, 
								  					fldid, 
													occ, 
													(char*)(*data)->value.str.val, 
								  					(FLDLEN) (*data)->value.str.len));
			break;
		
		
		default:
			zend_error (E_WARNING, "Somehow you have created a"
				" Field ID type I don't know about, probably a new Tux version.");
			return FAILURE;
	}
	if (ret_val == -1)
	{
		zend_error (E_WARNING, "Failed to add field [%d] to FML buffer [%s]", 
			fldid32, IS32(is32, (char*)Fstrerror32(Ferror32), (char*)Fstrerror(Ferror)));
	}
	return ret_val;
}




/* {{{ function tux_fml2array
	Function takes 2 arguments:
		1.  The source FML reference number 
		2.  A flag to indicate whether the primary index should be int or string
			They are set in a sequence of bits anded together.
	
			0 0 0  =  Perform no index translation, multi-occurrence.
			0 0 1  =  Store keys as the Field ID, coded same as 0.
			0 1 0  =  Store keys as the Tuxedo Field Number value.
			1 0 0  =  Store keys as the Field Name (String) value.

	
	Returns array, possibly 2 dimensional with second-D being occurrences.

	Design:  Using Fnext(32) loop through the source FML(32) buffer field
	by field, occurrence by occurrence.  For each hit:
	1.  Fnext points to the hit.
	2.  Fnext stores info in value.
	2a. If its too long to fit, we trap the FNOSPACE error and allocate
	    A bigger space if needed.
	3.  Use Fldtype to tell us what type of field it is.
	4.  Allocate a new zval and add it to the hash.
	5.  Once we know what type it is, copy the data from value into a permanent spot.
	6.  On to the next value.
	7.  Return the array ht at the end.
*/
ZEND_FUNCTION (tux_fml2array)
{
	zval ** arg_fml_ref;
	zval ** arg_index_flag;

	FLDID fieldid;
	FLDOCC occ;
	FLDLEN len;
	
	FLDID32 fieldid32;
	FLDOCC32 occ32;
	FLDLEN32 len32;
	FLDID32 last_fieldid;      /* stores the last fieldid we had, so I can tell if it changes */
	
	int is32;			/* our flag everywhere to tell us what type of buffer we have */
	
	int multi_occ_flag = 0;
	long array_key;     /* either the fieldid or occ for a 2-d */
	char value[256];	/* a temp holding area */
	char * copy_val;	/* a pointer to value usually, but not if too short */
	tux_tpalloc_buf_type * fml_buf_res;
	long fnext_ret;
	zval *new_data;	/* a pointer to each new data element as allocated */
	zval *d2_array;	/* pointer to an array, needed it its a multiple occurrence */
	int  dupe_string_flag;	/* simple flag to see if I want to dupe the string value */
	long cur_fieldid_occ;  /* stores the number of occurrences to expect for this fieldid */
	HashTable * array_ht;
	
	short short_data;	/* more temp holding variables */
	long  long_data;
	char  char_data;
	float float_data;
	double double_data;
	
	
	if((ZEND_NUM_ARGS() != 2) || 
		(zend_get_parameters_ex(2,
			&arg_fml_ref,
			&arg_index_flag) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_index_flag);
	
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

/*
	we just retrieved the FML buffer, what type was it?
	And set the function pointers we will need later on...
*/
	if ((is32 = _tux_is_fml_type (fml_buf_res->type)) == -1)
		RETURN_NULL ();

	
	if (array_init (return_value) != SUCCESS)	/* initialize the array */
	{
		zend_error (E_WARNING, "Could not initialize return array");
		RETURN_FALSE;
	}

/*
		A couple variable inits before the loop.
*/

	fieldid = fieldid32 = last_fieldid = FIRSTFLDID;	/* start at the beginning */
	array_ht = HASH_OF(return_value);  /* point us to the main array ht */

/*
		Start our loop.
		We need to re-init a few values at the start of every loop and before
		the Fnext(32) calls, so stick them ANDED in the while loop
		
		Main call... Fnext
			A return of 1 is fine.
			A return of 0 from Fnext is the end of the FML.
			A return of -1 is an error.
*/

	while ((len = len32 = sizeof(value)) && 
		   memset (value, 0, len) &&		/* zero out for new data */
		   (fnext_ret = IS32 (is32, Fnext32 ((FBFR32*)fml_buf_res->buf, &fieldid32, &occ32, value, &len32),
									Fnext ((FBFR*)fml_buf_res->buf, &fieldid, &occ, value, &len))))
	{
		copy_val = value;		/* re-init the copy_val pointer */
		dupe_string_flag = 1;	/* normally we want to duplicate any strings */

		if (fnext_ret == -1)
		{

			if (IS32(is32, Ferror32, Ferror) == FNOSPACE)  /* value too small */
			{
				char * tmp_ptr;
				long   tmp_len;
/*
				We need to find just how long the value (that was too long for our
				standard buffer) is to be.  The only way I've been able to do that
				is to use the Ffind function, which will return us a pointer to the
				value, and its length.  I'll then emalloc a permanent buffer and
				copy it.

				By calling Fnext one more time with NULL as a value, we tell it not
				to retrieve the value, but to only get the fieldid and occ.  Which
				we can then use in Ffind.
*/

				IS32 (is32, Fnext32 ((FBFR32*)fml_buf_res->buf, &fieldid32, &occ32, NULL, &len32),
									Fnext ((FBFR*)fml_buf_res->buf, &fieldid, &occ, NULL, &len));

				tmp_ptr = IS32(is32, Ffind32 ((FBFR32*)fml_buf_res->buf, fieldid32, occ32, &len32),
							Ffind ((FBFR*) fml_buf_res->buf, fieldid, occ, &len));
							
				if (tmp_ptr == NULL)
				{
					zend_error (E_WARNING, "Unable to retrieve fieldid [%d, %s]",
						IS32 (is32, fieldid32, fieldid), 
						IS32 (is32, Fstrerror32 (Ferror32), Fstrerror (Ferror)));
					RETURN_FALSE;
				}
				
				
				tmp_len = IS32(is32, len32, len);
				if ((copy_val = emalloc (tmp_len)) == NULL)
				{
					zend_error (E_WARNING, "Unable to allocate %d bytes of memory", tmp_len);
					RETURN_FALSE;
				}

/*
					Now we should have the space.  Copy the value from tmp_ptr to permanent storage.
*/
				memcpy (copy_val, tmp_ptr, tmp_len);
				dupe_string_flag = 0;	/* since we've emalloc'ed the buf, don't dupe it */
			}
			else
			{
				zend_error (E_WARNING, "Failure getting next FML record [%s]",
						IS32(is32, (char*)Fstrerror32(Ferror32), (char*)Fstrerror(Ferror))); 
				RETURN_FALSE;
			}
		}
		

		if (((is32) && (fieldid32 != last_fieldid))  ||  /* did we change to a new fieldid */
			 (!is32) && (fieldid != last_fieldid))
		{
			last_fieldid = IS32(is32, fieldid32, fieldid);	
/*
				get the number of occurrences to expect.
*/
			if ((cur_fieldid_occ = IS32(is32, Foccur32 ((FBFR32*)fml_buf_res->buf, fieldid32),
											 Foccur ((FBFR*) fml_buf_res->buf, fieldid)))  <= 0)
			{
				zend_error (E_WARNING, 
					"Error determining number of occurrences for FLDID %d [%s]", 
					IS32(is32,fieldid32,fieldid), 
					IS32(is32, (char*)Fstrerror32(Ferror32), (char*)Fstrerror(Ferror)));
				RETURN_FALSE;
			}
/*
				If multi-occur, do some one-time initialization of the new array.
*/			
			if (cur_fieldid_occ > 1)	/* we have a 2-d array to build. */
			{
				multi_occ_flag = 1;
				array_key = 0;				/* start out the 2-d array at zero */
				MAKE_STD_ZVAL (d2_array);	/* initialize a new array for multi-occurrence */
				if (array_init (d2_array) != SUCCESS)	/* initialize the array */
				{
					zend_error (E_WARNING, "Could not initialize multi-occurrence array");
					RETURN_FALSE;
				}
				array_ht = HASH_OF(d2_array);	/* point us to the 2d array ht */


				/*Update the return hash table with the FML info */
				if (_tux_update_fml_zend_hash (
						HASH_OF (return_value), 
						IS32(is32, fieldid32, fieldid), 
						&d2_array, 
						is32,
						(*arg_index_flag)->value.lval) == -1)
				{
					RETURN_FALSE;
				}
			}
			else
			{
				array_key = IS32 (is32, fieldid32, fieldid);	/* normally for single occurrences we want this */
				multi_occ_flag = 0;
				array_ht = HASH_OF(return_value);  /* point us to the main array ht */
			}
		}
		else	/* we are on the multi-occurrence of a 2-d */
		{
			array_key = IS32(is32, occ32, occ);	/* this time we want to use the occurrence number */
		}


		MAKE_STD_ZVAL (new_data);			/* allocate and init zval element */

		switch ( IS32 (is32, Fldtype32 (fieldid32), Fldtype (fieldid)))
		{
			case FLD_SHORT:		/* take a short and store as long */
				memcpy (&short_data, copy_val, sizeof (short));
				ZVAL_LONG (new_data, (long)short_data);
				break;
			

			case FLD_LONG:		/* nothing to do here */
				memcpy (&long_data, copy_val, sizeof (long));
				ZVAL_LONG (new_data, long_data);
				break;
						

			case FLD_FLOAT:		/* take the float and store as double */
				memcpy (&float_data, copy_val, sizeof (float));
				ZVAL_DOUBLE (new_data, (double)float_data);
				break;
			

			case FLD_DOUBLE:	/* nothing to do here */
				memcpy (&double_data, copy_val, sizeof (double));
				ZVAL_DOUBLE (new_data, double_data);
				break;


			case FLD_CHAR:		/* take the char and store as string */
			case FLD_STRING:	/* nothing to do here */
				ZVAL_STRING (new_data, copy_val, dupe_string_flag);
				break;
		

			case FLD_CARRAY:	/* assume a string with binary safe copy, dont convert */
				ZVAL_STRINGL (new_data, copy_val, len, dupe_string_flag);  /* binary safe copy */
				break;
				
				
			default:
				zend_error (E_WARNING, "Somehow you have created a"
					" Field ID type I don't know about, probably a new Tux version.");
				RETURN_FALSE;
		}
		
			/*
			  Update the hash table with the FML info 
			  The conditional assignment overrides the use Fname flag if
			  we are in the multiple occurrence loop
			*/
		if (_tux_update_fml_zend_hash (
				array_ht, 
				array_key, 
				&new_data, 
				is32, 
				multi_occ_flag ? 0 : (*arg_index_flag)->value.lval) 
			== -1)
		{
			RETURN_FALSE;
		}
	}
	return;  /* return_value should be built up. */
}


long _tux_update_fml_zend_hash (HashTable *ht, FLDID32 fieldid32, zval ** data, int is32, int flag)
{
	char * (*f_name) (long);	/*define a function pointer */
	char * name;
/*
	Now we need to store the zval into the permanent ht array.
	
	We have three different conditions the flag can tell us to perform.
	They are set in a sequence of bits anded together.
	
	0 0 0  =  Perform no index translation, multi-occurrence.
	0 0 1  =  Store keys as the Field ID, coded same as 0.
	0 1 0  =  Store keys as the Tuxedo Field Number value.
	1 0 0  =  Store keys as the Field Name (String) value.
*/				
	if (flag & 2)		/* they want the Field Number */
	{
		if (zend_hash_index_update (ht, IS32(is32, Fldno32(fieldid32), Fldno((FLDID)fieldid32)), 
				(void*) data, sizeof (zval*), NULL) == FAILURE)
		{
			zend_error (E_WARNING, "Unable to convert to Field Number, returning as FieldID");
			flag &= ~2;  /* unset the flag */
		}	
	}
	
	if (flag & 4)		/* they want the Field Name */
	{
		if ((name = IS32 (is32, Fname32 (fieldid32), Fname ((FLDID) fieldid32))) == NULL)
		{
			zend_error (E_WARNING, "Error converting FIELDID to Fname, returning as FieldID");
			flag &= ~4;  /* unset the bit for below */
		}	
		else
		{
			if (zend_hash_update 
				(ht, name, strlen(name)+1, (void*) data, sizeof (zval*), NULL) == FAILURE)
			{
				zend_error (E_WARNING, "Unable to add data value to return array");
				return FALSE;
			}
		}
	}
	
	if ((flag & 1) || (flag == 0))  /* either they wanted Field ID, multi-occurrence, or error return */
	{
		if (zend_hash_index_update (ht, fieldid32, 
			(void*) data, sizeof (zval*), NULL) == FAILURE)
		{
			zend_error (E_WARNING, "Unable to add data value to return array");
			return FALSE;
		}	
	}
/*		free (name);	*//* the string was malloc'd so we should free it now */

	return TRUE;
}
#endif
/*	end #if HAVE_TUXEDO */
#endif
