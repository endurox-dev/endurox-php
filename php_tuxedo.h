/*
	===============================================================|
	|   PHP Tuxedo                                                 |
	|--------------------------------------------------------------|
	|  php_tuxedo.h                                                |
	|    Function prototypes and defines                           |
	===============================================================|
*/
/* $Id: php_tuxedo.h,v 1.13 2002/01/07 05:37:37 bfoddy Exp $ */

#include <atmi.h>
#include <fml.h>
#include <fml32.h>
#include <tx.h>

#include "php.h"

static char _php_tux_ver[] = "0.4.0";
/*
	Standard Tuxedo buffer types:
	CARRAY - binary data
	STRING - Null terminated character array
	FML    - FML buffer
	FMLTYPE - The better waya to define FML bufs
	VIEW   - C structure
*/

#define TRUE 1
#define FALSE 0

#define TUX_TPALLOC_TYPE_LEN   20
#define TUX_NUM_BUF_TYPES		5

#define TUX_STRING_BUF_TYPE		0
#define TUX_CARRAY_BUF_TYPE		1
#define TUX_FML_BUF_TYPE		2
#define TUX_FML32_BUF_TYPE		3
#define TUX_VIEW_BUF_TYPE		4
 
									  
#if (TUX_FML32 && TUX_FML)
#define IS32(flag,a,b) ((flag) ? (a) : (b))
#elif TUX_FML32
#define IS32(flag,a,b) (a)
#elif TUX_FML
#define IS32(flag,a,b) (b)
#endif

extern zend_module_entry tuxedo_module_entry;

/*
	These are our two types of Tuxedo resources.
*/

typedef struct
{
	char * buf;		/* the pointer to the raw tpalloc buffer */
	long type;		/* what type of buffer is it. */
	char subtype [TUX_TPALLOC_TYPE_LEN];
	long size;		/* the length of the buffer */
} tux_tpalloc_buf_type;


/* 
	Declaration of functions to be exported 
	This is nothing more than a forward declaration.
	
	Functions in php_tuxedo.c
*/
ZEND_FUNCTION (tux_tpstrerror);
ZEND_FUNCTION (tux_get_tperrno);
ZEND_FUNCTION (tux_tpfree);
ZEND_FUNCTION (tux_tpalloc);
ZEND_FUNCTION (tux_put_buf);
ZEND_FUNCTION (tux_get_buf);
ZEND_FUNCTION (tux_tpcall);
ZEND_FUNCTION (tux_tpacall);
ZEND_FUNCTION (tux_tpgetrply);
ZEND_FUNCTION (tux_tpclose);
ZEND_FUNCTION (tux_tpterm);
ZEND_FUNCTION (tux_tpinit);
ZEND_FUNCTION (tux_tpabort);
ZEND_FUNCTION (tux_tpcommit);
ZEND_FUNCTION (tux_tpbegin);
ZEND_FUNCTION (tux_tuxreadenv);
ZEND_FUNCTION (tux_tuxgetenv);
ZEND_FUNCTION (tux_tuxputenv);
/*
	Declarations for FML functions in
	php_tuxedo_arrayfml.c
*/
#if (TUX_FML32 || TUX_FML)
ZEND_FUNCTION (tux_falloc);
ZEND_FUNCTION (tux_array2fml);
ZEND_FUNCTION (tux_fml2array);

/*
	Declarations for the raw FML functions in
	php_tuxedo_fml_api.c
*/
ZEND_FUNCTION (tux_fadd);
ZEND_FUNCTION (tux_fcmp);
ZEND_FUNCTION (tux_fconcat);
ZEND_FUNCTION (tux_fcpy);
ZEND_FUNCTION (tux_fdel);
ZEND_FUNCTION (tux_fdelall);
ZEND_FUNCTION (tux_ffprintf);
ZEND_FUNCTION (tux_get_ferrno);
ZEND_FUNCTION (tux_fstrerror);
ZEND_FUNCTION (tux_fldid);
ZEND_FUNCTION (tux_fldno);
ZEND_FUNCTION (tux_fldtype);
ZEND_FUNCTION (tux_fchksum);
ZEND_FUNCTION (tux_fidxused);
ZEND_FUNCTION (tux_fielded);
ZEND_FUNCTION (tux_findex);
ZEND_FUNCTION (tux_fjoin);
ZEND_FUNCTION (tux_flen);
ZEND_FUNCTION (tux_fmkfldid);
ZEND_FUNCTION (tux_fmove);
ZEND_FUNCTION (tux_fname);
ZEND_FUNCTION (tux_fneeded);
ZEND_FUNCTION (tux_fnum);
ZEND_FUNCTION (tux_foccur);
ZEND_FUNCTION (tux_fojoin);
ZEND_FUNCTION (tux_fpres);
ZEND_FUNCTION (tux_frstrindex);
ZEND_FUNCTION (tux_fsizeof);
ZEND_FUNCTION (tux_ftype);
ZEND_FUNCTION (tux_funindex);
ZEND_FUNCTION (tux_funused);
ZEND_FUNCTION (tux_fupdate);
ZEND_FUNCTION (tux_fused);
ZEND_FUNCTION (tux_fget);
ZEND_FUNCTION (tux_ffprint);
#endif

static void free_tux_tpalloc_buf(tux_tpalloc_buf_type *);
long _tux_alloc (long type, char * subtype, long size);
char * _tux_type2string (long type);
long _tux_get_fmlarray_key (HashTable * ht, int);
long _tux_fml_add (tux_tpalloc_buf_type *, zval **, FLDID32 fldid32, FLDOCC32 occ32);
long _tux_update_fml_zend_hash (HashTable *ht, FLDID32 fieldid32, zval ** data, int fmltype, int flag);
int  _tux_is_fml_type (int);
FLDID32 _get_arg_field_id (zval ** arg_field, int is32, int arg_no);

#define phpext_tuxedo_ptr &tuxedo_module_entry
