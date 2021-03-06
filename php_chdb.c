/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_chdb.h"

#include "chdb.h"

#define T_fromObj(T, ce, obj) (T *)Z_LVAL_P(zend_read_property(ce, obj, "handle", 6, 0))
#define T_getThis(T, ce) (T *)Z_LVAL_P(zend_read_property(ce, getThis(), "handle", 6, 0))
#define T_setThis(p, ce) zend_update_property_long(ce, getThis(), "handle", 6, (long)p);

zend_class_entry *chdb_ce;

#define chdb_getThis() T_getThis(CHDB, chdb_ce)
#define chdb_setThis(p) T_setThis(p, chdb_ce)

/* If you declare any globals in php_chdb.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(chdb)
*/

/* True global resources - no need for thread safety here */
//static int le_chdb;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("chdb.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_chdb_globals, chdb_globals)
    STD_PHP_INI_ENTRY("chdb.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_chdb_globals, chdb_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_chdb_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_chdb_init_globals(zend_chdb_globals *chdb_globals)
{
    chdb_globals->global_value = 0;
    chdb_globals->global_string = NULL;
}
*/
/* }}} */

PHP_METHOD(chdb, get)
{
    char *key = NULL;
    int key_len = 0;

    if (zend_parse_parameters_ex(0, ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
        RETURN_NULL();
    }

    CHDB *chdb = chdb_getThis();
    char *value = NULL;
    size_t value_len = 0;
    if(-1 == chdb_get(chdb, key, key_len, &value, &value_len)) {
        //not found
        RETURN_BOOL(0);
    }
    else {
        //found
        RETURN_STRINGL(value, value_len, 1);
    }
}

function_entry chdb_methods[] = {
    PHP_ME(chdb,  get,           NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(chdb)
{
    /* If you have INI entries, uncomment these lines
    REGISTER_INI_ENTRIES();
    */

    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "chdb", chdb_methods);
    chdb_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_long(chdb_ce, "handle", 6, 0, ZEND_ACC_PRIVATE);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(chdb)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(chdb)
{
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(chdb)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(chdb)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "chdb support", "enabled");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_chdb_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(chdb_open)
{
    char *pathname = NULL;
    int pathname_len = 0;

    if (zend_parse_parameters_ex(0, ZEND_NUM_ARGS() TSRMLS_CC, "s", &pathname, &pathname_len) == FAILURE) {
        RETURN_NULL();
    }

    CHDB *chdb = chdb_open(pathname);
    object_init_ex(return_value, chdb_ce);
    zend_update_property_long(chdb_ce, return_value, "handle", 6, (long)chdb);
}


/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/* {{{ chdb_functions[]
 *
 * Every user visible function must have an entry in chdb_functions[].
 */
zend_function_entry chdb_functions[] = {
    PHP_FE(chdb_open,	NULL)		/* For testing, remove later. */
    {NULL, NULL, NULL}	/* Must be the last line in chdb_functions[] */
};
/* }}} */

/* {{{ chdb_module_entry
 */
zend_module_entry chdb_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "chdb",
    chdb_functions,
    PHP_MINIT(chdb),
    PHP_MSHUTDOWN(chdb),
    PHP_RINIT(chdb),		/* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(chdb),	/* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(chdb),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CHDB
ZEND_GET_MODULE(chdb)
#endif



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
