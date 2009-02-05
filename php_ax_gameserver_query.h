#ifndef PHP_AX_GAMESERVER_QUERY_H
#define PHP_AX_GAMESERVER_QUERY_H

extern zend_module_entry ax_gameserver_query_module_entry;
#define phpext_ax_gameserver_query_ptr &ax_gameserver_query_module_entry

#ifdef PHP_WIN32
#define PHP_AX_GAMESERVER_QUERY_API __declspec(dllexport)
#else
#define PHP_AX_GAMESERVER_QUERY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ax_gameserver_query);
PHP_MSHUTDOWN_FUNCTION(ax_gameserver_query);
PHP_RINIT_FUNCTION(ax_gameserver_query);
PHP_RSHUTDOWN_FUNCTION(ax_gameserver_query);
PHP_MINFO_FUNCTION(ax_gameserver_query);

PHP_FUNCTION(axgsq_debug);
PHP_FUNCTION(axgsq_connect);
PHP_FUNCTION(axgsq_disconnect);
PHP_FUNCTION(axgsq_get_serverinfo);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(ax_gameserver_query)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(ax_gameserver_query)
*/

#ifdef ZTS
#define AX_GAMESERVER_QUERY_G(v) TSRMG(ax_gameserver_query_globals_id, zend_ax_gameserver_query_globals *, v)
#else
#define AX_GAMESERVER_QUERY_G(v) (ax_gameserver_query_globals.v)
#endif

#endif	/* PHP_AX_GAMESERVER_QUERY_H */
