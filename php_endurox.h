/*
	===============================================================|
	|   PHP Endurox                                                 |
	|--------------------------------------------------------------|
	|  php_endurox.h                                                |
	|    Function prototypes and defines                           |
	===============================================================|
*/
/* $Id: php_endurox.h,v 1.13 2002/01/07 05:37:37 bfoddy Exp $ */

#include <atmi.h>
#include <ubf.h>

#include "php.h"

static char _php_ndrxph_ver[] = "0.4.0";
/*
	Standard Endurox buffer types:
	CARRAY - binary data
	STRING - Null terminated character array
	UBF    - UBF buffer
	UBTYPE - The better waya to define UBF bufs
	VIEW   - C structure
*/

#define TRUE 1
#define FALSE 0

#define NDRXPH_TPALLOC_TYPE_LEN   20
#define NDRXPH_NUM_BUF_TYPES		5

#define NDRXPH_STRING_BUF_TYPE		0
#define NDRXPH_CARRAY_BUF_TYPE		1
#define NDRXPH_UBF_BUF_TYPE		2
#define NDRXPH_UBF32_BUF_TYPE		3
#define NDRXPH_VIEW_BUF_TYPE		4
 
									  
extern zend_module_entry endurox_module_entry;

/*
	These are our two types of Endurox resources.
*/

typedef struct
{
	char * buf;		/* the pointer to the raw tpalloc buffer */
	long type;		/* what type of buffer is it. */
	char subtype [NDRXPH_TPALLOC_TYPE_LEN];
	long size;		/* the length of the buffer */
} ndrxph_tpalloc_buf_type;


/* 
	Declaration of functions to be exported 
	This is nothing more than a forward declaration.
	
	Functions in php_endurox.c
*/
ZEND_FUNCTION (ndrxph_tpstrerror);
ZEND_FUNCTION (ndrxph_get_tperrno);
ZEND_FUNCTION (ndrxph_tpfree);
ZEND_FUNCTION (ndrxph_tpalloc);
ZEND_FUNCTION (ndrxph_put_buf);
ZEND_FUNCTION (ndrxph_get_buf);
ZEND_FUNCTION (ndrxph_tpcall);
ZEND_FUNCTION (ndrxph_tpacall);
ZEND_FUNCTION (ndrxph_tpgetrply);
ZEND_FUNCTION (ndrxph_tpclose);
ZEND_FUNCTION (ndrxph_tpterm);
ZEND_FUNCTION (ndrxph_tpinit);
ZEND_FUNCTION (ndrxph_tpabort);
ZEND_FUNCTION (ndrxph_tpcommit);
ZEND_FUNCTION (ndrxph_tpbegin);
/*
	Declarations for UBF functions in
	php_endurox_arrayubf.c
*/
#if (NDRXPH_UBF32 || NDRXPH_UBF)
ZEND_FUNCTION (ndrxph_array2ubf);
ZEND_FUNCTION (ndrxph_ubf2array);

/*
	Declarations for the raw UBF functions in
	php_endurox_ubf_api.c
*/
ZEND_FUNCTION (ndrxph_badd);
ZEND_FUNCTION (ndrxph_bconcat);
ZEND_FUNCTION (ndrxph_bcpy);
ZEND_FUNCTION (ndrxph_bdel);
ZEND_FUNCTION (ndrxph_bdelall);
ZEND_FUNCTION (ndrxph_bfprintf);
ZEND_FUNCTION (ndrxph_get_ferrno);
ZEND_FUNCTION (ndrxph_bstrerror);
ZEND_FUNCTION (ndrxph_bfldid);
ZEND_FUNCTION (ndrxph_bfldno);
ZEND_FUNCTION (ndrxph_bfldtype);
ZEND_FUNCTION (ndrxph_bidxused);
ZEND_FUNCTION (ndrxph_bisubf);
ZEND_FUNCTION (ndrxph_blen);
ZEND_FUNCTION (ndrxph_bmkfldid);
ZEND_FUNCTION (ndrxph_bfname);
ZEND_FUNCTION (ndrxph_fnum);
ZEND_FUNCTION (ndrxph_boccur);
ZEND_FUNCTION (ndrxph_bpres);
ZEND_FUNCTION (ndrxph_brstrindex);
ZEND_FUNCTION (ndrxph_bsizeof);
ZEND_FUNCTION (ndrxph_btype);
ZEND_FUNCTION (ndrxph_bunindex);
ZEND_FUNCTION (ndrxph_bunused);
ZEND_FUNCTION (ndrxph_bupdate);
ZEND_FUNCTION (ndrxph_bused);
ZEND_FUNCTION (ndrxph_bget);
ZEND_FUNCTION (ndrxph_bfprint);
#endif

static void free_ndrxph_tpalloc_buf(ndrxph_tpalloc_buf_type *);
long _ndrxph_alloc (long type, char * subtype, long size);
char * _ndrxph_type2string (long type);
long _ndrxph_get_ubfarray_key (HashTable * ht, int);
long _ndrxph_ubf_add (ndrxph_tpalloc_buf_type *, zval **, BFLDID bfldid, BFLDOCC occ32);
long _ndrxph_update_ubf_zend_hash (HashTable *ht, BFLDID fieldid32, zval ** data, int flag);
int  _ndrxph_is_ubf_type (int);
BFLDID _get_arg_field_id (zval ** arg_field, int arg_no);

#define phpext_endurox_ptr &endurox_module_entry
