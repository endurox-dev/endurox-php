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

static char _php_ndrx_ver[] = "0.4.0";
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

#define NDRX_TPALLOC_TYPE_LEN   20
#define NDRX_NUM_BUF_TYPES		5

#define NDRX_STRING_BUF_TYPE		0
#define NDRX_CARRAY_BUF_TYPE		1
#define NDRX_UBF_BUF_TYPE		2
#define NDRX_UBF32_BUF_TYPE		3
#define NDRX_VIEW_BUF_TYPE		4
 
									  
extern zend_module_entry endurox_module_entry;

/*
	These are our two types of Endurox resources.
*/

typedef struct
{
	char * buf;		/* the pointer to the raw tpalloc buffer */
	long type;		/* what type of buffer is it. */
	char subtype [NDRX_TPALLOC_TYPE_LEN];
	long size;		/* the length of the buffer */
} ndrx_tpalloc_buf_type;


/* 
	Declaration of functions to be exported 
	This is nothing more than a forward declaration.
	
	Functions in php_endurox.c
*/
ZEND_FUNCTION (ndrx_tpstrerror);
ZEND_FUNCTION (ndrx_get_tperrno);
ZEND_FUNCTION (ndrx_tpfree);
ZEND_FUNCTION (ndrx_tpalloc);
ZEND_FUNCTION (ndrx_put_buf);
ZEND_FUNCTION (ndrx_get_buf);
ZEND_FUNCTION (ndrx_tpcall);
ZEND_FUNCTION (ndrx_tpacall);
ZEND_FUNCTION (ndrx_tpgetrply);
ZEND_FUNCTION (ndrx_tpclose);
ZEND_FUNCTION (ndrx_tpterm);
ZEND_FUNCTION (ndrx_tpinit);
ZEND_FUNCTION (ndrx_tpabort);
ZEND_FUNCTION (ndrx_tpcommit);
ZEND_FUNCTION (ndrx_tpbegin);
/*
	Declarations for UBF functions in
	php_endurox_arrayubf.c
*/
#if (NDRX_UBF32 || NDRX_UBF)
ZEND_FUNCTION (ndrx_array2ubf);
ZEND_FUNCTION (ndrx_ubf2array);

/*
	Declarations for the raw UBF functions in
	php_endurox_ubf_api.c
*/
ZEND_FUNCTION (ndrx_badd);
ZEND_FUNCTION (ndrx_bconcat);
ZEND_FUNCTION (ndrx_bcpy);
ZEND_FUNCTION (ndrx_bdel);
ZEND_FUNCTION (ndrx_bdelall);
ZEND_FUNCTION (ndrx_bfprintf);
ZEND_FUNCTION (ndrx_get_ferrno);
ZEND_FUNCTION (ndrx_bstrerror);
ZEND_FUNCTION (ndrx_bfldid);
ZEND_FUNCTION (ndrx_bfldno);
ZEND_FUNCTION (ndrx_bfldtype);
ZEND_FUNCTION (ndrx_bidxused);
ZEND_FUNCTION (ndrx_bisubf);
ZEND_FUNCTION (ndrx_bjoin);
ZEND_FUNCTION (ndrx_blen);
ZEND_FUNCTION (ndrx_fmkbfldid);
ZEND_FUNCTION (ndrx_bfname);
ZEND_FUNCTION (ndrx_fnum);
ZEND_FUNCTION (ndrx_boccur);
ZEND_FUNCTION (ndrx_bpres);
ZEND_FUNCTION (ndrx_brstrindex);
ZEND_FUNCTION (ndrx_bsizeof);
ZEND_FUNCTION (ndrx_btype);
ZEND_FUNCTION (ndrx_bunindex);
ZEND_FUNCTION (ndrx_bunused);
ZEND_FUNCTION (ndrx_bupdate);
ZEND_FUNCTION (ndrx_bused);
ZEND_FUNCTION (ndrx_bget);
ZEND_FUNCTION (ndrx_bfprint);
#endif

static void free_ndrx_tpalloc_buf(ndrx_tpalloc_buf_type *);
long _ndrx_alloc (long type, char * subtype, long size);
char * _ndrx_type2string (long type);
long _ndrx_get_ubfarray_key (HashTable * ht, int);
long _ndrx_ubf_add (ndrx_tpalloc_buf_type *, zval **, BFLDID bfldid, BFLDOCC occ32);
long _ndrx_update_ubf_zend_hash (HashTable *ht, BFLDID fieldid32, zval ** data, int flag);
int  _ndrx_is_ubf_type (int);
BFLDID _get_arg_field_id (zval ** arg_field, int is32, int arg_no);

#define phpext_endurox_ptr &endurox_module_entry
