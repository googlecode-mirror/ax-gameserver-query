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
	char* cConnectionString;
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "lsl", &iGameServer, &cConnectionString, &iConnectionString_len, &iPort ) == FAILURE )
	{
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
			}
			add_assoc_zval( zServerInfo, "Players", zPlayerArray );
			add_assoc_long( return_value, "GameServer", pServerInfo->GameServer );
			add_assoc_zval( return_value, "ServerInfo", zServerInfo );
			axgsq_dealloc( pServerInfo );
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
