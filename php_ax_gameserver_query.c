#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ax_gameserver_query.h"
#include "ax_gameserver_query.h"

/* If you declare any globals in php_ax_gameserver_query.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ax_gameserver_query)
*/

static int ax_gameserver_query_resource;

static void ax_gameserver_query_resource_dtor( zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	//if( rsrc->ptr == NULL )
	//	return;
	//axgsq_disconnect( (struct axgsq_res*)rsrc->ptr );
}

zend_function_entry ax_gameserver_query_functions[] = {
	PHP_FE( axgsq_debug, NULL )
	PHP_FE( axgsq_connect, NULL )
	PHP_FE( axgsq_disconnect, NULL )
	PHP_FE( axgsq_get_serverinfo, NULL )
	{ NULL, NULL, NULL }
};

zend_module_entry ax_gameserver_query_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ax_gameserver_query",
	ax_gameserver_query_functions,
	PHP_MINIT(ax_gameserver_query),
	NULL, //PHP_MSHUTDOWN(ax_gameserver_query),
	NULL, //PHP_RINIT(ax_gameserver_query),
	NULL, //PHP_RSHUTDOWN(ax_gameserver_query),
	PHP_MINFO(ax_gameserver_query),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AX_GAMESERVER_QUERY
ZEND_GET_MODULE(ax_gameserver_query)
#endif

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ax_gameserver_query.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_ax_gameserver_query_globals, ax_gameserver_query_globals)
    STD_PHP_INI_ENTRY("ax_gameserver_query.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ax_gameserver_query_globals, ax_gameserver_query_globals)
PHP_INI_END()
*/

/* Uncomment this function if you have INI entries
static void php_ax_gameserver_query_init_globals(zend_ax_gameserver_query_globals *ax_gameserver_query_globals)
{
	ax_gameserver_query_globals->global_value = 0;
	ax_gameserver_query_globals->global_string = NULL;
}
*/

PHP_MINIT_FUNCTION(ax_gameserver_query)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	ax_gameserver_query_resource = zend_register_list_destructors_ex( ax_gameserver_query_resource_dtor, NULL, "ax_gameserver_query resource", module_number );
	//REGISTER_LONG_CONSTANT( "AXGSQ_UNKNOWN", AXGSQ_UNKNOWN, CONST_CS|CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "AXGSQ_SOURCE", AXGSQ_SOURCE, CONST_CS|CONST_PERSISTENT );
	return SUCCESS;
}

/* PHP_MSHUTDOWN_FUNCTION(ax_gameserver_query)
{
	//uncomment this line if you have INI entries
	//UNREGISTER_INI_ENTRIES();
	return SUCCESS;
} */

/* Remove if there's nothing to do at request start */
/* PHP_RINIT_FUNCTION(ax_gameserver_query)
{
	return SUCCESS;
} */

/* Remove if there's nothing to do at request end */
/* PHP_RSHUTDOWN_FUNCTION(ax_gameserver_query)
{
	return SUCCESS;
} */

PHP_MINFO_FUNCTION(ax_gameserver_query)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ax_gameserver_query support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}


PHP_FUNCTION(axgsq_debug)
{
	int iDebug;
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "l", &iDebug ) == FAILURE )
	{
		// Output error message
		RETURN_NULL();
	}
	axgsq_debug( iDebug );
}

PHP_FUNCTION(axgsq_connect)
{
	int iGameServer, iPort, iConnectionString_len;
	char* cConnectionString;
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "lsl", &iGameServer, &cConnectionString, &iConnectionString_len, &iPort ) == FAILURE )
	{
		// Output error message
		RETURN_NULL();
	}
	struct axgsq_res* pResource;
	pResource = axgsq_connect( iGameServer, cConnectionString, iPort );
	if( pResource == NULL )
	{
		RETURN_NULL();
	}
	else
	{
		ZEND_REGISTER_RESOURCE( return_value, pResource, ax_gameserver_query_resource );
	}
}

PHP_FUNCTION(axgsq_disconnect)
{
	zval* zResource;
	struct axgsq_res* pResource;
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource ) == FAILURE )
	{
		// Output error message
		RETURN_NULL();
	}
	ZEND_FETCH_RESOURCE( pResource, struct axgsq_res*, &zResource, -1, "ax_gameserver_query_resource", ax_gameserver_query_resource );
	if( axgsq_disconnect( pResource ) )
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(axgsq_get_serverinfo)
{
	zval* zResource;
	struct axgsq_res* pResource;
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource ) == FAILURE )
	{
		// Output error message
		RETURN_NULL();
	}
	ZEND_FETCH_RESOURCE( pResource, struct axgsq_res*, &zResource, -1, "ax_gameserver_query_resource", ax_gameserver_query_resource );
	struct axgsq_serverinfo* pServerInfo = axgsq_get_serverinfo( pResource );
	if( pServerInfo == NULL )
	{
		RETURN_NULL();
	}
	else
	{
		switch( pServerInfo->iGameServer )
		{
		case AXGSQ_SOURCE: ; // Yes the semi-colon is ment to be there for gcc compiler issues
			struct axgsq_serverinfo_source* pSIs = (struct axgsq_serverinfo_source*) pServerInfo->pSI;
			char temp[2];
			temp[1] = 0;
			array_init( return_value );
			add_assoc_long( return_value, "Version", pSIs->Version );
			add_assoc_string( return_value, "ServerName", pSIs->ServerName, 1 );
			add_assoc_string( return_value, "Map", pSIs->Map, 1 );
			add_assoc_string( return_value, "GameDirectory", pSIs->GameDirectory, 1 );
			add_assoc_string( return_value, "GameDescription", pSIs->GameDescription, 1 );
			add_assoc_long( return_value, "AppID", pSIs->AppID );
			add_assoc_long( return_value, "NumberOfPlayers", pSIs->NumberOfPlayers );
			add_assoc_long( return_value, "MaximumPlayers", pSIs->MaximumPlayers );
			add_assoc_long( return_value, "NumberOfBots", pSIs->NumberOfBots );
			temp[0] = pSIs->Dedicated;
			add_assoc_stringl( return_value, "Dedicated", temp, 1, 1 );
			temp[0] = pSIs->OS;
			add_assoc_stringl( return_value, "OS", temp, 1, 1 );
			add_assoc_long( return_value, "Password", pSIs->Password );
			add_assoc_long( return_value, "Secure", pSIs->Secure );
			add_assoc_string( return_value, "GameVersion", pSIs->GameVersion, 1 );
			add_assoc_long( return_value, "NumPlayers", pSIs->NumPlayers );

			zval* zPlayerArray;
			zval* zTempPArray;
			ALLOC_INIT_ZVAL( zPlayerArray );
			array_init( zPlayerArray );
			int x;
			for( x = 0; x < pSIs->NumPlayers; x++ )
			{
				ALLOC_INIT_ZVAL( zTempPArray );
				array_init( zTempPArray );
				add_assoc_long( zTempPArray, "Index", pSIs->Players[x].Index );
				add_assoc_string( zTempPArray, "PlayerName", pSIs->Players[x].PlayerName, 1 );
				add_assoc_long( zTempPArray, "Kills", pSIs->Players[x].Kills );
				add_assoc_double( zTempPArray, "TimeConnected", pSIs->Players[x].TimeConnected );
				add_next_index_zval( zPlayerArray, zTempPArray );
			}
			add_assoc_zval( return_value, "Players", zPlayerArray );

			//delete [] temp;
			//free( pSIs );
			//free( pServerInfo );
			return;
			break;
		default: ;
			//error
			free( pServerInfo );
			RETURN_FALSE;
		}
	}
}
