/*
 * Copyright (c) 2009 Matthew Jeffries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * $Id$
 */

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
	REGISTER_LONG_CONSTANT( "AXGSQ_THESHIP", AXGSQ_THESHIP, CONST_CS|CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "AXGSQ_HALO", AXGSQ_HALO, CONST_CS|CONST_PERSISTENT );
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
		free( pResource );
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
		free( pServerInfo );
		RETURN_NULL();
	}
	else
	{
		if( pServerInfo->GameServer == AXGSQ_SOURCE )
		{
			struct axgsq_serverinfo_source* pSIs = (struct axgsq_serverinfo_source*) pServerInfo->ServerInfo;
			char temp[2];
			temp[1] = 0;
			zval* zServerInfo;
			ALLOC_INIT_ZVAL( zServerInfo );
			array_init( zServerInfo );
			array_init( return_value );
			add_assoc_long( zServerInfo, "Version", pSIs->Version );
			add_assoc_string( zServerInfo, "ServerName", pSIs->ServerName, 1 );
			add_assoc_string( zServerInfo, "Map", pSIs->Map, 1 );
			add_assoc_string( zServerInfo, "GameDirectory", pSIs->GameDirectory, 1 );
			add_assoc_string( zServerInfo, "GameDescription", pSIs->GameDescription, 1 );
			add_assoc_long( zServerInfo, "AppID", pSIs->AppID );
			add_assoc_long( zServerInfo, "NumberOfPlayers", pSIs->NumberOfPlayers );
			add_assoc_long( zServerInfo, "MaximumPlayers", pSIs->MaximumPlayers );
			add_assoc_long( zServerInfo, "NumberOfBots", pSIs->NumberOfBots );
			temp[0] = pSIs->Dedicated;
			add_assoc_stringl( zServerInfo, "Dedicated", temp, 1, 1 );
			temp[0] = pSIs->OS;
			add_assoc_stringl( zServerInfo, "OS", temp, 1, 1 );
			add_assoc_long( zServerInfo, "Password", pSIs->Password );
			add_assoc_long( zServerInfo, "Secure", pSIs->Secure );
			add_assoc_string( zServerInfo, "GameVersion", pSIs->GameVersion, 1 );
			add_assoc_long( zServerInfo, "NumPlayers", pSIs->NumPlayers );
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
				free( pSIs->Players[x].PlayerName );
			}
			add_assoc_zval( zServerInfo, "Players", zPlayerArray );
			add_assoc_long( return_value, "GameServer", pServerInfo->GameServer );
			add_assoc_zval( return_value, "ServerInfo", zServerInfo );
			free( pSIs->ServerName );
			free( pSIs->Map );
			free( pSIs->GameDirectory );
			free( pSIs->GameDescription );
			free( pSIs->GameVersion );
		}
		else if( pServerInfo->GameServer == AXGSQ_THESHIP )
		{
			struct axgsq_serverinfo_theship* pSIs = (struct axgsq_serverinfo_theship*) pServerInfo->ServerInfo;
			char temp[2];
			temp[1] = 0;
			zval* zServerInfo;
			ALLOC_INIT_ZVAL( zServerInfo );
			array_init( zServerInfo );
			array_init( return_value );
			add_assoc_long( zServerInfo, "Version", pSIs->Version );
			add_assoc_string( zServerInfo, "ServerName", pSIs->ServerName, 1 );
			add_assoc_string( zServerInfo, "Map", pSIs->Map, 1 );
			add_assoc_string( zServerInfo, "GameDirectory", pSIs->GameDirectory, 1 );
			add_assoc_string( zServerInfo, "GameDescription", pSIs->GameDescription, 1 );
			add_assoc_long( zServerInfo, "AppID", pSIs->AppID );
			add_assoc_long( zServerInfo, "NumberOfPlayers", pSIs->NumberOfPlayers );
			add_assoc_long( zServerInfo, "MaximumPlayers", pSIs->MaximumPlayers );
			add_assoc_long( zServerInfo, "NumberOfBots", pSIs->NumberOfBots );
			temp[0] = pSIs->Dedicated;
			add_assoc_stringl( zServerInfo, "Dedicated", temp, 1, 1 );
			temp[0] = pSIs->OS;
			add_assoc_stringl( zServerInfo, "OS", temp, 1, 1 );
			add_assoc_long( zServerInfo, "Password", pSIs->Password );
			add_assoc_long( zServerInfo, "Secure", pSIs->Secure );
			add_assoc_long( zServerInfo, "GameMode", pSIs->GameMode );
			add_assoc_long( zServerInfo, "WitnessCount", pSIs->WitnessCount );
			add_assoc_long( zServerInfo, "WitnessTime", pSIs->WitnessTime );
			add_assoc_string( zServerInfo, "GameVersion", pSIs->GameVersion, 1 );
			add_assoc_long( zServerInfo, "NumPlayers", pSIs->NumPlayers );
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
				free( pSIs->Players[x].PlayerName );
			}
			add_assoc_zval( zServerInfo, "Players", zPlayerArray );
			add_assoc_long( return_value, "GameServer", pServerInfo->GameServer );
			add_assoc_zval( return_value, "ServerInfo", zServerInfo );
			free( pSIs->ServerName );
			free( pSIs->Map );
			free( pSIs->GameDirectory );
			free( pSIs->GameDescription );
			free( pSIs->GameVersion );
		}
		else if( pServerInfo->GameServer == AXGSQ_HALO )
		{
			struct axgsq_serverinfo_halo* pSIs = (struct axgsq_serverinfo_halo*) pServerInfo->ServerInfo;
			zval* zServerInfo;
			ALLOC_INIT_ZVAL( zServerInfo );
			array_init( zServerInfo );
			array_init( return_value );

			add_assoc_string( zServerInfo, "Hostname", pSIs->Hostname, 1 );
			add_assoc_string( zServerInfo, "GameVer", pSIs->GameVer, 1 );
			add_assoc_string( zServerInfo, "HostPort", pSIs->HostPort, 1 );
			add_assoc_string( zServerInfo, "MaxPlayers", pSIs->MaxPlayers, 1 );
			add_assoc_string( zServerInfo, "Password", pSIs->Password, 1 );
			add_assoc_string( zServerInfo, "MapName", pSIs->MapName, 1 );
			add_assoc_string( zServerInfo, "Dedicated", pSIs->Dedicated, 1 );
			add_assoc_string( zServerInfo, "GameMode", pSIs->GameMode, 1 );
			add_assoc_string( zServerInfo, "Game_Classic", pSIs->Game_Classic, 1 );
			add_assoc_string( zServerInfo, "NumPlayers", pSIs->NumPlayers, 1 );
			add_assoc_string( zServerInfo, "GameType", pSIs->GameType, 1 );
			add_assoc_string( zServerInfo, "TeamPlay", pSIs->TeamPlay, 1 );
			add_assoc_string( zServerInfo, "GameVariant", pSIs->GameVariant, 1 );
			add_assoc_string( zServerInfo, "FragLimit", pSIs->FragLimit, 1 );

			add_assoc_long( return_value, "GameServer", pServerInfo->GameServer );
			add_assoc_zval( return_value, "ServerInfo", zServerInfo );
			free( pSIs->Hostname );
			free( pSIs->GameVer );
			free( pSIs->HostPort );
			free( pSIs->MaxPlayers );
			free( pSIs->Password );
			free( pSIs->MapName );
			free( pSIs->Dedicated );
			free( pSIs->GameMode );
			free( pSIs->Game_Classic );
			free( pSIs->NumPlayers );
			free( pSIs->GameType );
			free( pSIs->TeamPlay );
			free( pSIs->GameVariant );
			free( pSIs->FragLimit );
		}
		else
		{
			free( pServerInfo->ServerInfo );
			free( pServerInfo );
			RETURN_FALSE;
		}
	}
	free( pServerInfo->ServerInfo );
	free( pServerInfo );
}
