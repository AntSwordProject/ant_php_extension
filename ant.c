/*
  +----------------------------------------------------------------------+
  | PHP Version 5, 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: github.com/antswordproject                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/exec.h"
#include "php_ant.h"

/* If you declare any globals in php_ant.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ant)
*/

/* True global resources - no need for thread safety here */
static int le_ant;

/* {{{ ext/standard/basic_functions.c for exec.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_system, 0, 0, 1)
    ZEND_ARG_INFO(0, command)
    ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ ant_functions[]
 *
 * Every user visible function must have an entry in ant_functions[].
 */
const zend_function_entry ant_functions[] = {
    PHP_FE(confirm_ant_compiled,    NULL)       /* For testing, remove later. */
    PHP_FE(antsystem,               arginfo_system)
    PHP_FE_END  /* Must be the last line in ant_functions[] */
};
/* }}} */

/* {{{ ant_module_entry
 */
zend_module_entry ant_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "ant",
    ant_functions,
    PHP_MINIT(ant),
    PHP_MSHUTDOWN(ant),
    PHP_RINIT(ant),     /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(ant), /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(ant),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ANT_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ant.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ant_globals, ant_globals)
    STD_PHP_INI_ENTRY("ant.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ant_globals, ant_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_ant_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_ant_compiled)
{
#if PHP_MAJOR_VERSION == 7
    char *arg = NULL;
    size_t arg_len, len;
    zend_string *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "ant", arg);

    RETURN_STR(strg);

#elif PHP_MAJOR_VERSION == 5
    char *arg = NULL;
    int arg_len, len;
    char *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "ant", arg);
    RETURN_STRINGL(strg, len, 0);
#else
#endif
};
/* }}} */


static void ant_php_exec_ex(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
    char *cmd;
    zval *ret_code=NULL, *ret_array=NULL;
    int ret;
#if PHP_MAJOR_VERSION == 5
    int cmd_len;

    if (mode) {
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z/", &cmd, &cmd_len, &ret_code) == FAILURE) {
            RETURN_FALSE;
        }
    } else {
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z/z/", &cmd, &cmd_len, &ret_array, &ret_code) == FAILURE) {
            RETURN_FALSE;
        }
    }
    if (!cmd_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command");
        RETURN_FALSE;
    }
    if (!ret_array) {
        ret = php_exec(mode, cmd, NULL, return_value TSRMLS_CC);
    } else {
        if (Z_TYPE_P(ret_array) != IS_ARRAY) {
            zval_dtor(ret_array);
            array_init(ret_array);
        }
        ret = php_exec(2, cmd, ret_array, return_value TSRMLS_CC);
    }
#elif PHP_MAJOR_VERSION == 7

    size_t cmd_len;

    ZEND_PARSE_PARAMETERS_START(1, (mode ? 2 : 3))
        Z_PARAM_STRING(cmd, cmd_len)
        Z_PARAM_OPTIONAL
        if (!mode) {
            Z_PARAM_ZVAL_DEREF(ret_array)
        }
        Z_PARAM_ZVAL_DEREF(ret_code)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (!cmd_len) {
        php_error_docref(NULL, E_WARNING, "Cannot execute a blank command");
        RETURN_FALSE;
    }
    if (strlen(cmd) != cmd_len) {
        php_error_docref(NULL, E_WARNING, "NULL byte detected. Possible attack");
        RETURN_FALSE;
    }
    if (!ret_array) {
        ret = php_exec(mode, cmd, NULL, return_value);
    } else {
        if (Z_TYPE_P(ret_array) != IS_ARRAY) {
            zval_ptr_dtor(ret_array);
            array_init(ret_array);
        } else if (Z_REFCOUNT_P(ret_array) > 1) {
            zval_ptr_dtor(ret_array);
            ZVAL_ARR(ret_array, zend_array_dup(Z_ARR_P(ret_array)));
        }
        ret = php_exec(2, cmd, ret_array, return_value);
    }
#endif
    if (ret_code) {
        zval_ptr_dtor(ret_code);
        ZVAL_LONG(ret_code, ret);
    }
};
/* }}} */

/* {{{ proto int antsystem(string command [, int &return_value])
   Execute an external program and display output */
PHP_FUNCTION(antsystem)
{
    ant_php_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
};
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_ant_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ant_init_globals(zend_ant_globals *ant_globals)
{
	ant_globals->global_value = 0;
	ant_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ant)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ant)
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
PHP_RINIT_FUNCTION(ant)
{
#if PHP_MAJOR_VERSION == 7
#if defined(COMPILE_DL_ANT) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
#endif
	return SUCCESS;
};
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ant)
{
	return SUCCESS;
};
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ant)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ant support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
};
/* }}} */

#ifdef COMPILE_DL_ANT
#if PHP_MAJOR_VERSION == 7

#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif

#endif /* PHP_MAJOR_VERSION */
ZEND_GET_MODULE(ant)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
