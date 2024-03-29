/*
  +----------------------------------------------------------------------+
  | PHP Version 5, 7, 8                                                  |
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

#ifndef PHP_ANT_H
#define PHP_ANT_H

extern zend_module_entry ant_module_entry;
#define phpext_ant_ptr &ant_module_entry

#define PHP_ANT_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_ANT_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ANT_API __attribute__ ((visibility("default")))
#else
#	define PHP_ANT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ant);
PHP_MSHUTDOWN_FUNCTION(ant);
PHP_RINIT_FUNCTION(ant);
PHP_RSHUTDOWN_FUNCTION(ant);
PHP_MINFO_FUNCTION(ant);

PHP_FUNCTION(confirm_ant_compiled);
PHP_FUNCTION(antsystem); /* php system */

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(ant)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(ant)
*/
#if PHP_MAJOR_VERSION == 7 || PHP_MAJOR_VERSION == 8
/* Always refer to the globals in your function as ANT_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define ANT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(ant, v)

#if defined(ZTS) && defined(COMPILE_DL_ANT)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#elif PHP_MAJOR_VERSION == 5

#ifdef ZTS
#define ANT_G(v) TSRMG(ant_globals_id, zend_ant_globals *, v)
#else
#define ANT_G(v) (ant_globals.v)
#endif

#else /* PHP_MAJOR_VERSION */

#endif /* PHP_MAJOR_VERSION */

#endif	/* PHP_ANT_H */

/* 兼容php5-8 */
#if PHP_MAJOR_VERSION == 8
#define TSRMLS_CC
#endif

#if PHP_MAJOR_VERSION == 5
#define ant_num_type int
#else
#define ant_num_type size_t
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
