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

static int ax_gameserver_query_resource;

static void ax_gameserver_query_resource_dtor( zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
}

zend_function_entry ax_gameserver_query_functions[] = {
	PHP_FE( axgsq_version, NULL )
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
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ax_gameserver_query),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1.40",
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AX_GAMESERVER_QUERY
ZEND_GET_MODULE(ax_gameserver_query)
#endif

PHP_MINIT_FUNCTION(ax_gameserver_query)
{
	ax_gameserver_query_resource = zend_register_list_destructors_ex( ax_gameserver_query_resource_dtor, NULL, "ax_gameserver_query resource", module_number );
	REGISTER_LONG_CONSTANT( "AXGSQ_SOURCE", AXGSQ_SOURCE, CONST_CS|CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "AXGSQ_THESHIP", AXGSQ_THESHIP, CONST_CS|CONST_PERSISTENT );
	return SUCCESS;
}

PHP_MINFO_FUNCTION(ax_gameserver_query)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ax_gameserver_query support", "enabled");
	php_info_print_table_end();
}

PHP_FUNCTION(axgsq_version)
{
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "" ) == FAILURE )
	{
		RETURN_NULL();
	}
	RETURN_STRING( axgsq_version(), 1 );
}

PHP_FUNCTION(axgsq_connect)
{
	int iGameServer, iPort, iConnectionString_len;
	char *cConnectionString;
	struct axgsq_res *pResource;
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "lsl", &iGameServer, &cConnectionString, &iConnectionString_len, &iPort ) == FAILURE )
	{
		RETURN_NULL();
	}
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
	zval *zResource;
	struct axgsq_res *pResource;
	struct axgsq_serverinfo *pServerInfo;
	void *pSIs;
	char temp[2];
	zval* zServerInfo;
	zval* zPlayerArray;
	zval* zTempPArray;
	int x;
	
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource ) == FAILURE )
	{
		RETURN_NULL();
	}
	ZEND_FETCH_RESOURCE( pResource, struct axgsq_res*, &zResource, -1, "ax_gameserver_query_resource", ax_gameserver_query_resource );
	pServerInfo = axgsq_get_serverinfo( pResource );
	if( pServerInfo == NULL )
	{
		RETURN_NULL();
	}
	else
	{
		if( pServerInfo->GameServer == AXGSQ_SOURCE )
		{
			pSIs = (struct axgsq_serverinfo_source*) pServerInfo->ServerInfo;
			temp[1] = 0;
			ALLOC_INIT_ZVAL( zServerInfo );
			array_init( zServerInfo );
			array_init( return_value );
			add_assoc_long( zServerInfo, "Version", ((struct axgsq_serverinfo_source *)pSIs)->Version );
			add_assoc_string( zServerInfo, "ServerName", ((struct axgsq_serverinfo_source *)pSIs)->ServerName, 1 );
			add_assoc_string( zServerInfo, "Map", ((struct axgsq_serverinfo_source *)pSIs)->Map, 1 );
			add_assoc_string( zServerInfo, "GameDirectory", ((struct axgsq_serverinfo_source *)pSIs)->GameDirectory, 1 );
			add_assoc_string( zServerInfo, "GameDescription", ((struct axgsq_serverinfo_source *)pSIs)->GameDescription, 1 );
			add_assoc_long( zServerInfo, "AppID", ((struct axgsq_serverinfo_source *)pSIs)->AppID );
			add_assoc_long( zServerInfo, "NumberOfPlayers", ((struct axgsq_serverinfo_source *)pSIs)->NumberOfPlayers );
			add_assoc_long( zServerInfo, "MaximumPlayers", ((struct axgsq_serverinfo_source *)pSIs)->MaximumPlayers );
			add_assoc_long( zServerInfo, "NumberOfBots", ((struct axgsq_serverinfo_source *)pSIs)->NumberOfBots );
			temp[0] = ((struct axgsq_serverinfo_source *)pSIs)->Dedicated;
			add_assoc_stringl( zServerInfo, "Dedicated", temp, 1, 1 );
			temp[0] = ((struct axgsq_serverinfo_source *)pSIs)->OS;
			add_assoc_stringl( zServerInfo, "OS", temp, 1, 1 );
			add_assoc_long( zServerInfo, "Password", ((struct axgsq_serverinfo_source *)pSIs)->Password );
			add_assoc_long( zServerInfo, "Secure", ((struct axgsq_serverinfo_source *)pSIs)->Secure );
			add_assoc_string( zServerInfo, "GameVersion", ((struct axgsq_serverinfo_source *)pSIs)->GameVersion, 1 );
			add_assoc_long( zServerInfo, "NumPlayers", ((struct axgsq_serverinfo_source *)pSIs)->NumPlayers );
			ALLOC_INIT_ZVAL( zPlayerArray );
			array_init( zPlayerArray );
			for( x = 0; x < ((struct axgsq_serverinfo_source *)pSIs)->NumPlayers; x++ )
			{
				ALLOC_INIT_ZVAL( zTempPArray );
				array_init( zTempPArray );
				add_assoc_long( zTempPArray, "Index", ((struct axgsq_serverinfo_source *)pSIs)->Players[x].Index );
				add_assoc_string( zTempPArray, "PlayerName", ((struct axgsq_serverinfo_source *)pSIs)->Players[x].PlayerName, 1 );
				add_assoc_long( zTempPArray, "Kills", ((struct axgsq_serverinfo_source *)pSIs)->Players[x].Kills );
				add_assoc_double( zTempPArray, "TimeConnected", ((struct axgsq_serverinfo_source *)pSIs)->Players[x].TimeConnected );
				add_next_index_zval( zPlayerArray, zTempPArray );
			}
			add_assoc_zval( zServerInfo, "Players", zPlayerArray );
			add_assoc_long( return_value, "GameServer", pServerInfo->GameServer );
			add_assoc_zval( return_value, "ServerInfo", zServerInfo );
			axgsq_dealloc( pServerInfo );
		}
		else if( pServerInfo->GameServer == AXGSQ_THESHIP )
		{
			pSIs = (struct axgsq_serverinfo_theship*) pServerInfo->ServerInfo;
			temp[1] = 0;
			ALLOC_INIT_ZVAL( zServerInfo );
			array_init( zServerInfo );
			array_init( return_value );
			add_assoc_long( zServerInfo, "Version", ((struct axgsq_serverinfo_theship *)pSIs)->Version );
			add_assoc_string( zServerInfo, "ServerName", ((struct axgsq_serverinfo_theship *)pSIs)->ServerName, 1 );
			add_assoc_string( zServerInfo, "Map", ((struct axgsq_serverinfo_theship *)pSIs)->Map, 1 );
			add_assoc_string( zServerInfo, "GameDirectory", ((struct axgsq_serverinfo_theship *)pSIs)->GameDirectory, 1 );
			add_assoc_string( zServerInfo, "GameDescription", ((struct axgsq_serverinfo_theship *)pSIs)->GameDescription, 1 );
			add_assoc_long( zServerInfo, "AppID", ((struct axgsq_serverinfo_theship *)pSIs)->AppID );
			add_assoc_long( zServerInfo, "NumberOfPlayers", ((struct axgsq_serverinfo_theship *)pSIs)->NumberOfPlayers );
			add_assoc_long( zServerInfo, "MaximumPlayers", ((struct axgsq_serverinfo_theship *)pSIs)->MaximumPlayers );
			add_assoc_long( zServerInfo, "NumberOfBots", ((struct axgsq_serverinfo_theship *)pSIs)->NumberOfBots );
			temp[0] = ((struct axgsq_serverinfo_theship *)pSIs)->Dedicated;
			add_assoc_stringl( zServerInfo, "Dedicated", temp, 1, 1 );
			temp[0] = ((struct axgsq_serverinfo_theship *)pSIs)->OS;
			add_assoc_stringl( zServerInfo, "OS", temp, 1, 1 );
			add_assoc_long( zServerInfo, "Password", ((struct axgsq_serverinfo_theship *)pSIs)->Password );
			add_assoc_long( zServerInfo, "Secure", ((struct axgsq_serverinfo_theship *)pSIs)->Secure );
			add_assoc_long( zServerInfo, "GameMode", ((struct axgsq_serverinfo_theship *)pSIs)->GameMode );
			add_assoc_long( zServerInfo, "WitnessCount", ((struct axgsq_serverinfo_theship *)pSIs)->WitnessCount );
			add_assoc_long( zServerInfo, "WitnessTime", ((struct axgsq_serverinfo_theship *)pSIs)->WitnessTime );
			add_assoc_string( zServerInfo, "GameVersion", ((struct axgsq_serverinfo_theship *)pSIs)->GameVersion, 1 );
			add_assoc_long( zServerInfo, "NumPlayers", ((struct axgsq_serverinfo_theship *)pSIs)->NumPlayers );
			ALLOC_INIT_ZVAL( zPlayerArray );
			array_init( zPlayerArray );
			for( x = 0; x < ((struct axgsq_serverinfo_theship *)pSIs)->NumPlayers; x++ )
			{
				ALLOC_INIT_ZVAL( zTempPArray );
				array_init( zTempPArray );
				add_assoc_long( zTempPArray, "Index", ((struct axgsq_serverinfo_theship *)pSIs)->Players[x].Index );
				add_assoc_string( zTempPArray, "PlayerName", ((struct axgsq_serverinfo_theship *)pSIs)->Players[x].PlayerName, 1 );
				add_assoc_long( zTempPArray, "Kills", ((struct axgsq_serverinfo_theship *)pSIs)->Players[x].Kills );
				add_assoc_double( zTempPArray, "TimeConnected", ((struct axgsq_serverinfo_theship *)pSIs)->Players[x].TimeConnected );
				add_next_index_zval( zPlayerArray, zTempPArray );
			}
			add_assoc_zval( zServerInfo, "Players", zPlayerArray );
			add_assoc_long( return_value, "GameServer", pServerInfo->GameServer );
			add_assoc_zval( return_value, "ServerInfo", zServerInfo );
			axgsq_dealloc( pServerInfo );
		}
		else
		{
			axgsq_dealloc( pServerInfo );
			RETURN_NULL();
		}
	}
}
