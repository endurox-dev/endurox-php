/*
	===============================================================|
	|   PHP Endurox                                                 |
	|--------------------------------------------------------------|
	|  php_endurox_arrayubf.c                                       |
	|    Contains functions for the array to ubf functions         |
	===============================================================|
*/
/* $Id: php_endurox_arrayubf.c,v 1.6 2002/01/07 05:36:41 bfoddy Exp $ */


#include <stdlib.h>
#include <string.h>
/* include the endurox headers */
#include <atmi.h>
#include <ubf.h>

/* include standard php header */

#include "php.h"
#include "php_endurox.h"

#if HAVE_ENDUROX
#if (NDRX_UBF32 || NDRX_UBF)

/* True globals, no need for thread safety */
extern int ndrx_rh_alloc_buffer;  /* tpalloc buffer resource type resource handle*/



/* 
	ZEND_FUNCTION declarations located in php_endurox.h
*/

/* {{{ function ndrx_arrary2ubf
	Function takes 2 arguments:
		1.  The destination UBF reference number 
		2.  an array argument (possibly 2 dimensional)
	and inserts values into a UBF/UBF32 buffer according to the
	index value.
	
	Design note:  I though some of making this function do a recursive call
	to move through N layers of a array, but the UBF buffer can only take
	two dimensions so we will limit this function to 2 without recursion.
	
	Returns TRUE/FALSE.
*/
ZEND_FUNCTION (ndrx_array2ubf)
{
	zval ** arg_buf_ref;
	zval ** arg_src_array;

	int is32;
	
	ndrx_tpalloc_buf_type * ubf_buf_res;
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
						ubf_buf_res, 
						ndrx_tpalloc_buf_type *, 
						arg_buf_ref, 
						-1, 
						"Endurox tpalloc buffer", 
						ndrx_rh_alloc_buffer);
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

	if (ubf_buf_res->type == NDRX_UBF_BUF_TYPE)
	{
		is32 = FALSE;
	}
	else if (ubf_buf_res->type == NDRX_UBF32_BUF_TYPE)
	{
		is32 = TRUE;
	}
	else
	{
		zend_error (E_WARNING, "Supplied buffer type is not UBF/UBF32");
		RETURN_FALSE;
	}
	

	primary_ht = HASH_OF (*arg_src_array);
	zend_hash_internal_pointer_reset (primary_ht);
	

	while (zend_hash_get_current_data (primary_ht, (void**) &primary_data) == SUCCESS) 
	{
/*
			First get the primary key value (could be int or string).
*/
		if ((primary_key = _ndrx_get_ubfarray_key (primary_ht, is32)) == -1)
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

				if ( ! _ndrx_ubf_add (ubf_buf_res, secondary_data, primary_key, secondary_key))
				{
					/* need more error handling here... 
					   if we change the buffer, remember to write it back to the resource*/
					zend_error (E_WARNING, "Adding UBF field call failed");
					RETURN_FALSE;
				}


				zend_hash_move_forward (secondary_ht);  /* next secondary record */
			}
		}
		else		/* its a normal value of some type, add it*/
		{
			if ( ! _ndrx_ubf_add (ubf_buf_res, primary_data, primary_key, -1))
			{
					/* need more error handling here...  
					   if we change the buffer, remember to write it back to the resource*/
				zend_error (E_WARNING, "Adding UBF field call failed");
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
long _ndrx_get_ubfarray_key (HashTable * ht, int is32)
{
	long ret_val = 0;
	char * str_index;
	ulong num_index;
	
	
	switch (zend_hash_get_current_key (ht, &str_index, &num_index, 1))
	{
		case HASH_KEY_IS_LONG:	/* if its a long value, just return it */
			ret_val = num_index;
			break;
			
		case HASH_KEY_IS_STRING:	/* a string, convert with  Bfldid*/
			ret_val = Bfldid (str_index);
			break;
	
		default:					/* How did this happen? */
			zend_error (E_WARNING, "Internal error, Invalid Hash index type");
			ret_val = -1;
	}
	efree (str_index);
	return ret_val;
}




/*
	This function takes an long key (BFLDID), a pointer to a ndrx_tpalloc_buf_type,
	and a zval ** and inserts the data into the buffer by calling Fchg(32).

	returns TRUE/FALSE.
	
	It has to vary exection depending by what type of value data points to,
	and by what type of UBF buf is.

	We call Bfldtype to determine the type of field they gave us, and cast accordingly.

	Fappend(UBFH *fbfr, BFLDID fieldid, char *value, BFLDLEN len)
	
	UBF field types  -- ubf.h  these are the same for UBF32 also.
		#define BFLD_SHORT       0        short int 
		#define BFLD_LONG        1        long int 
		#define BFLD_CHAR        2        character 
		#define BFLD_FLOAT       3        single-precision float 
		#define BFLD_DOUBLE      4        double-precision float 
		#define BFLD_STRING      5        string - null terminated 
		#define BFLD_CARRAY      6        character array 
  
*/
long _ndrx_ubf_add (ndrx_tpalloc_buf_type * buffer, zval ** data, BFLDID bfldid, BFLDOCC occ32)
{
/*
		This is a function pointer, its name F_add does not imply it points to
		Badd, actually it uses Fchg.
*/
	BFLDOCC occ = (BFLDOCC) occ32;

	int  bfldid_type;
	short short_data;
	long  long_data;
	char  char_data;
	float float_data;
	double double_data;
	
	int ret_val;
	int  is32 = ((buffer->type == NDRX_UBF32_BUF_TYPE) ? TRUE : FALSE);	

/*
		First they passed us a bfldid, find out what type it is.
*/
	bfldid_type = Bfldtype (bfldid);
/*
		Ok, we know what type the bfldid is, we need to make
		sure that the type of data they sent us is the same type.
		If not, we need to convert it.  The Fchg function
		expects a pointer to exactly the correct length variable.
*/
	switch (bfldid_type)
	{
		case BFLD_SHORT:		/* take a long and make it short */
			convert_to_long_ex (data);
			short_data = (short) (*data)->value.lval;
			ret_val = Bchg((UBFH*) buffer->buf, bfldid, occ32, (char*)&short_data, 0);
			break;
			

		case BFLD_LONG:		/* nothing to do here except ensure its long */
			convert_to_long_ex (data);
			long_data = (long) (*data)->value.lval;
			ret_val = Bchg((UBFH*) buffer->buf, bfldid, occ32, (char*)&long_data, 0);
			break;
			

		case BFLD_CHAR:		/* take a string and use only first character */
			convert_to_string_ex (data);
			char_data = (char) (*data)->value.str.val[0];  /* we only want the first char */
			ret_val = Bchg((UBFH*) buffer->buf, bfldid, occ32, (char*)&char_data, 0);
			break;
			

		case BFLD_FLOAT:		/* take a double and make it a float */
			convert_to_double_ex (data);
			float_data = (float) (*data)->value.dval;
			ret_val = Bchg((UBFH*) buffer->buf, bfldid, occ32, (char*)&float_data, 0);
			break;
			

		case BFLD_DOUBLE:	/* nothing to do here */
			convert_to_double_ex (data);
			double_data = (double) (*data)->value.dval;
			ret_val = Bchg((UBFH*) buffer->buf, bfldid, occ32, (char*)&double_data, 0);
			break;


		case BFLD_STRING:	/* nothing to do here */
			convert_to_string_ex (data);
			ret_val = Bchg((UBFH*) buffer->buf, bfldid, occ32, (char*)(*data)->value.str.val, 0);
			break;


		case BFLD_CARRAY:	/* assume a string with binary safe copy, dont convert */
			ret_val = Bchg((UBFH*)  buffer->buf, 
													bfldid, 
													occ32, 
													(char*)(*data)->value.str.val, 
													(BFLDLEN) (*data)->value.str.len);
			break;
		
		
		default:
			zend_error (E_WARNING, "Somehow you have created a"
				" Field ID type I don't know about, probably a new Ndrx version.");
			return FAILURE;
	}
	if (ret_val == -1)
	{
		zend_error (E_WARNING, "Failed to add field [%d] to UBF buffer [%s]", 
			bfldid, (char*)Bstrerror(Berror));
	}
	return ret_val;
}




/* {{{ function ndrx_ubf2array
	Function takes 2 arguments:
		1.  The source UBF reference number 
		2.  A flag to indicate whether the primary index should be int or string
			They are set in a sequence of bits anded together.
	
			0 0 0  =  Perform no index translation, multi-occurrence.
			0 0 1  =  Store keys as the Field ID, coded same as 0.
			0 1 0  =  Store keys as the Endurox Field Number value.
			1 0 0  =  Store keys as the Field Name (String) value.

	
	Returns array, possibly 2 dimensional with second-D being occurrences.

	Design:  Using Fnext(32) loop through the source UBF(32) buffer field
	by field, occurrence by occurrence.  For each hit:
	1.  Fnext points to the hit.
	2.  Fnext stores info in value.
	2a. If its too long to fit, we trap the FNOSPACE error and allocate
	    A bigger space if needed.
	3.  Use Bfldtype to tell us what type of field it is.
	4.  Allocate a new zval and add it to the hash.
	5.  Once we know what type it is, copy the data from value into a permanent spot.
	6.  On to the next value.
	7.  Return the array ht at the end.
*/
ZEND_FUNCTION (ndrx_ubf2array)
{
	zval ** arg_ubf_ref;
	zval ** arg_index_flag;

	BFLDID fieldid32;
	BFLDOCC occ32;
	BFLDLEN len32;
	BFLDID last_fieldid;      /* stores the last fieldid we had, so I can tell if it changes */
	
	int is32;			/* our flag everywhere to tell us what type of buffer we have */
	
	int multi_occ_flag = 0;
	long array_key;     /* either the fieldid or occ for a 2-d */
	char value[256];	/* a temp holding area */
	char * copy_val;	/* a pointer to value usually, but not if too short */
	ndrx_tpalloc_buf_type * ubf_buf_res;
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
			&arg_ubf_ref,
			&arg_index_flag) != SUCCESS))
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex (arg_index_flag);
	
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
	we just retrieved the UBF buffer, what type was it?
	And set the function pointers we will need later on...
*/
	if ((is32 = _ndrx_is_ubf_type (ubf_buf_res->type)) == -1)
		RETURN_NULL ();

	
	if (array_init (return_value) != SUCCESS)	/* initialize the array */
	{
		zend_error (E_WARNING, "Could not initialize return array");
		RETURN_FALSE;
	}

/*
		A couple variable inits before the loop.
*/

	fieldid32 = last_fieldid = BFIRSTFLDID;	/* start at the beginning */
	array_ht = HASH_OF(return_value);  /* point us to the main array ht */

/*
		Start our loop.
		We need to re-init a few values at the start of every loop and before
		the Fnext(32) calls, so stick them ANDED in the while loop
		
		Main call... Fnext
			A return of 1 is fine.
			A return of 0 from Fnext is the end of the UBF.
			A return of -1 is an error.
*/

	while ((len32 = sizeof(value)) && 
		   memset (value, 0, len32) &&		/* zero out for new data */
		   (fnext_ret = Bnext ((UBFH*)ubf_buf_res->buf, &fieldid32, &occ32, value, &len32)))
	{
		copy_val = value;		/* re-init the copy_val pointer */
		dupe_string_flag = 1;	/* normally we want to duplicate any strings */

		if (fnext_ret == -1)
		{

			if (Berror == BNOSPACE)  /* value too small */
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

				Bnext ((UBFH*)ubf_buf_res->buf, &fieldid32, &occ32, NULL, &len32);

				tmp_ptr = Bfind ((UBFH*)ubf_buf_res->buf, fieldid32, occ32, &len32);
							
				if (tmp_ptr == NULL)
				{
					zend_error (E_WARNING, "Unable to retrieve fieldid [%d, %s]",
						fieldid32, Bstrerror (Berror));
					RETURN_FALSE;
				}
				
				
				tmp_len = len32;
				if ((copy_val = emalloc (tmp_len)) == NULL)
				{
					zend_error (E_WARNING, "Unable to allocate %ld bytes of memory", tmp_len);
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
				zend_error (E_WARNING, "Failure getting next UBF record [%s]",
						(char*)Bstrerror(Berror)); 
				RETURN_FALSE;
			}
		}
		

		if ((fieldid32 != last_fieldid))  /* did we change to a new fieldid */
		{
			last_fieldid = fieldid32;
/*
				get the number of occurrences to expect.
*/
			if ((cur_fieldid_occ = Boccur ((UBFH*) ubf_buf_res->buf, fieldid32))  <= 0)
			{
				zend_error (E_WARNING, 
					"Error determining number of occurrences for BFLDID %d [%s]", 
					fieldid32,
					(char*)Bstrerror(Berror));
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


				/*Update the return hash table with the UBF info */
				if (_ndrx_update_ubf_zend_hash (
						HASH_OF (return_value), 
						fieldid32,
						&d2_array, 
						(*arg_index_flag)->value.lval) == -1)
				{
					RETURN_FALSE;
				}
			}
			else
			{
				array_key = fieldid32;	/* normally for single occurrences we want this */
				multi_occ_flag = 0;
				array_ht = HASH_OF(return_value);  /* point us to the main array ht */
			}
		}
		else	/* we are on the multi-occurrence of a 2-d */
		{
			array_key = occ32;	/* this time we want to use the occurrence number */
		}


		MAKE_STD_ZVAL (new_data);			/* allocate and init zval element */

		switch ( Bfldtype (fieldid32))
		{
			case BFLD_SHORT:		/* take a short and store as long */
				memcpy (&short_data, copy_val, sizeof (short));
				ZVAL_LONG (new_data, (long)short_data);
				break;
			

			case BFLD_LONG:		/* nothing to do here */
				memcpy (&long_data, copy_val, sizeof (long));
				ZVAL_LONG (new_data, long_data);
				break;
						

			case BFLD_FLOAT:		/* take the float and store as double */
				memcpy (&float_data, copy_val, sizeof (float));
				ZVAL_DOUBLE (new_data, (double)float_data);
				break;
			

			case BFLD_DOUBLE:	/* nothing to do here */
				memcpy (&double_data, copy_val, sizeof (double));
				ZVAL_DOUBLE (new_data, double_data);
				break;


			case BFLD_CHAR:		/* take the char and store as string */
			case BFLD_STRING:	/* nothing to do here */
				ZVAL_STRING (new_data, copy_val, dupe_string_flag);
				break;
		

			case BFLD_CARRAY:	/* assume a string with binary safe copy, dont convert */
				ZVAL_STRINGL (new_data, copy_val, len32, dupe_string_flag);  /* binary safe copy */
				break;
				
				
			default:
				zend_error (E_WARNING, "Somehow you have created a"
					" Field ID type I don't know about, probably a new Ndrx version.");
				RETURN_FALSE;
		}
		
			/*
			  Update the hash table with the UBF info 
			  The conditional assignment overrides the use Bfname flag if
			  we are in the multiple occurrence loop
			*/
		if (_ndrx_update_ubf_zend_hash (
				array_ht, 
				array_key, 
				&new_data, 
				multi_occ_flag ? 0 : (*arg_index_flag)->value.lval) 
			== -1)
		{
			RETURN_FALSE;
		}
	}
	return;  /* return_value should be built up. */
}


long _ndrx_update_ubf_zend_hash (HashTable *ht, BFLDID fieldid32, zval ** data, int flag)
{
	char * (*f_name) (long);	/*define a function pointer */
	char * name;
/*
	Now we need to store the zval into the permanent ht array.
	
	We have three different conditions the flag can tell us to perform.
	They are set in a sequence of bits anded together.
	
	0 0 0  =  Perform no index translation, multi-occurrence.
	0 0 1  =  Store keys as the Field ID, coded same as 0.
	0 1 0  =  Store keys as the Endurox Field Number value.
	1 0 0  =  Store keys as the Field Name (String) value.
*/				
	if (flag & 2)		/* they want the Field Number */
	{
		if (zend_hash_index_update (ht, Bfldno(fieldid32), 
				(void*) data, sizeof (zval*), NULL) == FAILURE)
		{
			zend_error (E_WARNING, "Unable to convert to Field Number, returning as FieldID");
			flag &= ~2;  /* unset the flag */
		}	
	}
	
	if (flag & 4)		/* they want the Field Name */
	{
		if ((name = Bfname (fieldid32)) == NULL)
		{
			zend_error (E_WARNING, "Error converting FIELDID to Bfname, returning as FieldID");
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
/*	end #if HAVE_ENDUROX */
#endif
