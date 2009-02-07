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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ax_gameserver_query.h"

int iIsDebug = 0;

int axgsq_isIpAddress( const char* cConnectionString );
#ifdef _WIN32
int axgsq_send( SOCKET s, const unsigned char *buf, int len, int flags );
int axgsq_recv( SOCKET s, unsigned char *buf, int len, int flags );
#else /*_WIN32*/
int axgsq_send( int s, const unsigned char *buf, int len, int flags );
int axgsq_recv( int s, unsigned char *buf, int len, int flags );
#endif /*_WIN32*/
int axgsq_error( const char *fmt, ... );
unsigned char axgsq_get_byte( unsigned char* cInput, int* iPos );
short axgsq_get_short( unsigned char* cInput, int* iPos );
long axgsq_get_long( unsigned char* cInput, int* iPos );
float axgsq_get_float( unsigned char* cInput, int* iPos );
unsigned char* axgsq_get_string( unsigned char* cInput, int* iPos );

void axgsq_debug( int iDebug )
{
	if( iDebug == 1 )
		iIsDebug = 1;
	else if( iDebug == 0 )
		iIsDebug = 0;
}

struct axgsq_res* axgsq_connect( int iGameServer, const char* cConnectionString, int iPort )
{
	struct axgsq_res* pResource = (struct axgsq_res*) malloc( sizeof( struct axgsq_res ) );
	if( pResource == NULL )
	{
		free( pResource );
		axgsq_error( "Memory allocation error in axgsq_connect();\n" );
		return NULL;
	}
	memset( pResource, 0, sizeof( struct axgsq_res ) );

	/*
		Code for creating sockets for the different games
	*/
	int iSocketType, iSocketProtocol;
	pResource->iGameServer = iGameServer;
	switch( iGameServer )
	{
	case AXGSQ_SOURCE:
	case AXGSQ_THESHIP:
	case AXGSQ_HALO:
		iSocketType = SOCK_DGRAM;
		iSocketProtocol = IPPROTO_UDP;
		break;
	default:
		free( pResource );
		//free( &iSocketType );
		//free( &iSocketProtocol );
		axgsq_error( "Unknown gameserver type given to axgsq_connect();\n" );
		return NULL;
	}
	pResource->iSocketType = iSocketType;
	pResource->iSocketProtocol = iSocketProtocol;


#ifdef _WIN32
	WSADATA* pWSAData = malloc( sizeof( WSADATA ) );
	if( pWSAData == NULL )
	{
		free( pResource );
		//free( &iSocketType );
		//free( &iSocketProtocol );
		free( pWSAData );
	}
	memset( pWSAData, 0, sizeof( WSADATA ) );
	if( WSAStartup( MAKEWORD( 2, 0 ), &pWSAData ) != 0 )
	{
		// Winsock startup error
		free( pResource );
		//free( &iSocketType );
		//free( &iSocketProtocol );
		free( pWSAData );
		axgsq_error( "Winsock starup error in axgsq_connect();\n" );
		return NULL;
	}
	free( pWSAData );
#endif /* _WIN32 */
	pResource->pSocket = socket( AF_INET, iSocketType, iSocketProtocol );
	//free( &iSocketType );
	//free( &iSocketProtocol );
#ifdef _WIN32
	if( pResource->pSocket == INVALID_SOCKET )
	{
		free( pResource );
		WSACleanup();
		axgsq_error( "Socket creation error in axgsq_connect();\n" );
		return NULL;
	}
#else /* _WIN32 */
	if( pResource->pSocket < 0 )
	{
		free( pResource );
		axgsq_error( "Socket creation error in axgsq_connect();\n" );
		return NULL;
	}
#endif /* _WIN32 */
	pResource->pSockAddr.sin_family = AF_INET;
	if( axgsq_isIpAddress( cConnectionString ) )
		pResource->pSockAddr.sin_addr.s_addr = inet_addr( cConnectionString );
	else
	{
		struct hostent* pHost;
		if( ( pHost = gethostbyname( cConnectionString ) ) == NULL )
		{
			free( pResource );
			free( pHost );
			axgsq_error( "Hostname resolution failed in axgsq_connect();\n" );
			return NULL;
		}
		pResource->pSockAddr.sin_addr.s_addr = *((unsigned long*)pHost->h_addr_list[0]);
		free( pHost );
	}
	pResource->pSockAddr.sin_port = htons( (u_short)iPort );
#ifdef _WIN32
	if( connect( pResource->pSocket, (struct sockaddr*)&pResource->pSockAddr, sizeof( pResource->pSockAddr ) ) == -1 )
	{
		free( pResource );
		WSACleanup();
		axgsq_error( "Socket connection error in axgsq_connect();\n" );
		return NULL;
	}
#else /* _WIN32 */
	if( connect( pResource->pSocket, (struct sockaddr*)&pResource->pSockAddr, sizeof( pResource->pSockAddr ) ) < 0 )
	{
		free( pResource );
		axgsq_error( "Socket connection error in axgsq_connect();\n" );
		return NULL;
	}
#endif /* _WIN32 */
	return pResource;
}

int axgsq_disconnect( struct axgsq_res* pResource )
{
	if( pResource != NULL )
	{
#ifdef _WIN32
		closesocket( pResource->pSocket );
#else /* _WIN32 */
		close( pResource->pSocket );
#endif /* _WIN32 */
		free( pResource );
		return 1;
	}
	return 0;
}

struct axgsq_serverinfo* axgsq_get_serverinfo( struct axgsq_res* pResource )
{
	if( pResource != NULL )
	{
		struct axgsq_serverinfo* pServerInfo = (struct axgsq_serverinfo*) malloc( sizeof(struct axgsq_serverinfo) );
		if( pServerInfo == NULL )
		{
			free( pServerInfo );
			axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
			return NULL;
		}
		memset( pServerInfo, 0, sizeof(struct axgsq_serverinfo) );

		unsigned char* cInput = NULL;
		int iPos = 0;
		// Creating a switch() using if/else if/else syntax due to incompatibility with Dev-C++ (gcc)
		if( pResource->iGameServer == AXGSQ_SOURCE )
		{
			pServerInfo->GameServer = pResource->iGameServer;
			struct axgsq_serverinfo_source* pSI = (struct axgsq_serverinfo_source*) malloc( sizeof(struct axgsq_serverinfo_source) );
			if( pSI == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( pSI, 0, sizeof( pSI ) );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x54Source Engine Query\x00", 25, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			cInput = (unsigned char*) malloc( 2049 );
			if( cInput == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			iPos = 5;
			pSI->Version = axgsq_get_byte( cInput, &iPos );
			pSI->ServerName = axgsq_get_string( cInput, &iPos );
			pSI->Map = axgsq_get_string( cInput, &iPos );
			pSI->GameDirectory = axgsq_get_string( cInput, &iPos );
			pSI->GameDescription = axgsq_get_string( cInput, &iPos );
			pSI->AppID = axgsq_get_short( cInput, &iPos );
			pSI->NumberOfPlayers = axgsq_get_byte( cInput, &iPos );
			pSI->MaximumPlayers = axgsq_get_byte( cInput, &iPos );
			pSI->NumberOfBots = axgsq_get_byte( cInput, &iPos );
			pSI->Dedicated = axgsq_get_byte( cInput, &iPos );
			pSI->OS = axgsq_get_byte( cInput, &iPos );
			pSI->Password = axgsq_get_byte( cInput, &iPos );
			pSI->Secure = axgsq_get_byte( cInput, &iPos );
			pSI->GameVersion = axgsq_get_string( cInput, &iPos );
			// A2S_SERVERQUERY_GETCHALLENGE - 
			//   Due to a valve update breaking the protocol for 
			//   goldsource servers we use an invalid A2S_PLAYER request.
			// "\xFF\xFF\xFF\xFF\x55\xFF\xFF\xFF\xFF"
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x55\xFF\xFF\xFF\xFF", 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			iPos = 5;
			unsigned char* cChallenge = (unsigned char*) malloc( 5 );
			if( cChallenge == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				free( cChallenge );
				axgsq_error( "Mem alloc error\n" );
				return NULL;
			}
			cChallenge[0] = axgsq_get_byte( cInput, &iPos );
			cChallenge[1] = axgsq_get_byte( cInput, &iPos );
			cChallenge[2] = axgsq_get_byte( cInput, &iPos );
			cChallenge[3] = axgsq_get_byte( cInput, &iPos );
			cChallenge[4] = 0;
			// A2S_PLAYER
			// "\xFF\xFF\xFF\xFF\x55%s"
			unsigned char *cTemp = (unsigned char *) malloc( 10 );
			if( cTemp == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				free( cChallenge );
				free( cTemp );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			sprintf( (char *)cTemp, "\xFF\xFF\xFF\xFF\x55%s", cChallenge );
			free( cChallenge );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)cTemp, 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				free( cTemp );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			free( cTemp );
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			iPos = 5;
			pSI->NumPlayers = axgsq_get_byte( cInput, &iPos );
			int x;
			for( x = 0; x < pSI->NumPlayers; x++ )
			{
				pSI->Players[x].Index = axgsq_get_byte( cInput, &iPos );
				pSI->Players[x].PlayerName = axgsq_get_string( cInput, &iPos );
				pSI->Players[x].Kills = axgsq_get_long( cInput, &iPos );
				pSI->Players[x].TimeConnected = axgsq_get_float( cInput, &iPos );
			}
			//free( &x );
			free( cInput );
			//free( &iPos );
			pServerInfo->ServerInfo = pSI;
		}
		else if( pResource->iGameServer == AXGSQ_THESHIP )
		{
			pServerInfo->GameServer = pResource->iGameServer;
			struct axgsq_serverinfo_theship* pSI = (struct axgsq_serverinfo_theship*) malloc( sizeof(struct axgsq_serverinfo_theship) );
			if( pSI == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( pSI, 0, sizeof( pSI ) );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x54Source Engine Query\x00", 25, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			cInput = (unsigned char*) malloc( 2049 );
			if( cInput == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			iPos = 5;
			pSI->Version = axgsq_get_byte( cInput, &iPos );
			pSI->ServerName = axgsq_get_string( cInput, &iPos );
			pSI->Map = axgsq_get_string( cInput, &iPos );
			pSI->GameDirectory = axgsq_get_string( cInput, &iPos );
			pSI->GameDescription = axgsq_get_string( cInput, &iPos );
			pSI->AppID = axgsq_get_short( cInput, &iPos );
			pSI->NumberOfPlayers = axgsq_get_byte( cInput, &iPos );
			pSI->MaximumPlayers = axgsq_get_byte( cInput, &iPos );
			pSI->NumberOfBots = axgsq_get_byte( cInput, &iPos );
			pSI->Dedicated = axgsq_get_byte( cInput, &iPos );
			pSI->OS = axgsq_get_byte( cInput, &iPos );
			pSI->Password = axgsq_get_byte( cInput, &iPos );
			pSI->Secure = axgsq_get_byte( cInput, &iPos );
			pSI->GameMode = axgsq_get_byte( cInput, &iPos );
			pSI->WitnessCount = axgsq_get_byte( cInput, &iPos );
			pSI->WitnessTime = axgsq_get_byte( cInput, &iPos );
			pSI->GameVersion = axgsq_get_string( cInput, &iPos );
			// A2S_SERVERQUERY_GETCHALLENGE - 
			//   Due to a valve update breaking the protocol for 
			//   goldsource servers we use an invalid A2S_PLAYER request.
			// "\xFF\xFF\xFF\xFF\x55\xFF\xFF\xFF\xFF"
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x55\xFF\xFF\xFF\xFF", 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			iPos = 5;
			unsigned char* cChallenge = (unsigned char*) malloc( 5 );
			if( cChallenge == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				free( cChallenge );
				axgsq_error( "Mem alloc error\n" );
				return NULL;
			}
			cChallenge[0] = axgsq_get_byte( cInput, &iPos );
			cChallenge[1] = axgsq_get_byte( cInput, &iPos );
			cChallenge[2] = axgsq_get_byte( cInput, &iPos );
			cChallenge[3] = axgsq_get_byte( cInput, &iPos );
			cChallenge[4] = 0;
			// A2S_PLAYER
			// "\xFF\xFF\xFF\xFF\x55%s"
			unsigned char *cTemp = (unsigned char *) malloc( 10 );
			if( cTemp == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				free( cChallenge );
				free( cTemp );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			sprintf( (char *)cTemp, "\xFF\xFF\xFF\xFF\x55%s", cChallenge );
			free( cChallenge );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)cTemp, 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				free( cTemp );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			free( cTemp );
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			iPos = 5;
			pSI->NumPlayers = axgsq_get_byte( cInput, &iPos );
			int x;
			for( x = 0; x < pSI->NumPlayers; x++ )
			{
				pSI->Players[x].Index = axgsq_get_byte( cInput, &iPos );
				pSI->Players[x].PlayerName = axgsq_get_string( cInput, &iPos );
				pSI->Players[x].Kills = axgsq_get_long( cInput, &iPos );
				pSI->Players[x].TimeConnected = axgsq_get_float( cInput, &iPos );
			}
			//free( &x );
			free( cInput );
			//free( &iPos );
			pServerInfo->ServerInfo = pSI;
		}
		else if( pResource->iGameServer == AXGSQ_HALO )
		{
			pServerInfo->GameServer = pResource->iGameServer;
			struct axgsq_serverinfo_halo* pSI = (struct axgsq_serverinfo_halo*) malloc( sizeof(struct axgsq_serverinfo_halo) );
			if( pSI == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( pSI, 0, sizeof( pSI ) );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFE\xFD\x00\x77\x6A\x9D\x9D\xFF\xFF\xFF\xFF", 11, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket send error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			cInput = (unsigned char*) malloc( 2049 );
			if( cInput == NULL )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Memory allocation error in axgsq_get_serverinfo()\n" );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				//free( &iPos );
				free( pSI );
				axgsq_error( "Socket recv error in axgsq_get_serverinfo()\n" );
				return NULL;
			}

			//printf( "Getting info\n" );
			iPos = 4;
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->Hostname = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->GameVer = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->HostPort = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->MaxPlayers = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->Password = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->MapName = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->Dedicated = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->GameMode = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->Game_Classic = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->NumPlayers = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->GameType = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->TeamPlay = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->GameVariant = axgsq_get_string( cInput, &iPos );
			free( axgsq_get_string( cInput, &iPos ) );
			pSI->FragLimit = axgsq_get_string( cInput, &iPos );
			//printf( "Done Getting info\n" );

			free( cInput );
			//free( &iPos );
			pServerInfo->ServerInfo = pSI;
		}
		else
		{
			free( pServerInfo );
			free( cInput );
			//free( &iPos );
			axgsq_error( "Unknown gameserver type given to axgsq_serverinfo();\n" );
			return NULL;
		}
		return pServerInfo;
	}
	else
	{
		axgsq_error( "Invalid resource passed to axgsq_get_serverinfo()\n" );
		return NULL;
	}
}



/*
	HELPER FUNCTIONS
*/

int axgsq_isIpAddress( const char* cConnectionString )
{
	int len = (int)strlen( cConnectionString );
	if( len <= 15 && len >= 7 )
	{
		int x;
		for( x = 0; x < len; x++ )
		{
			if( !( ( cConnectionString[x] >= 0x30 && cConnectionString[x] <= 0x39 ) || cConnectionString[x] == 0x2E ) )
			{
				//free( &len );
				//free( &x );
				return 0;
			}
		}
		//free( &len );
		//free( &x );
		return 1;
	}
	//free( &len );
	return 0;
}

#ifdef _WIN32
int axgsq_send( SOCKET s, const unsigned char *buf, int len, int flags )
#else /*_WIN32*/
int axgsq_send( int s, const unsigned char *buf, int len, int flags )
#endif /*_WIN32*/
{
	fd_set set;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO( &set );
	FD_SET( s, &set );
	int tmp = select( s+1, NULL, &set, NULL, &tv );
	//axgsq_error( "send int = %d\n", tmp );
	//free( &set );
	//free( &tv );
	if( tmp > 0 )
		return send( s, (const char *)buf, len, flags );
	return -1;
}

#ifdef _WIN32
int axgsq_recv( SOCKET s, unsigned char *buf, int len, int flags )
#else /*_WIN32*/
int axgsq_recv( int s, unsigned char *buf, int len, int flags )
#endif /*_WIN32*/
{
	fd_set set;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO( &set );
	FD_SET( s, &set );
	int tmp = select( s+1, &set, NULL, NULL, &tv );
	//axgsq_error( "recv int = %d\n", tmp );
	//free( &set );
	//free( &tv );
	if( tmp > 0 )
		return recv( s, (char *)buf, len, flags );
	return -1;
}

int axgsq_error( const char *fmt, ... )
{
	if( iIsDebug == 1 )
	{
		int ret;
		va_list ap;
		va_start( ap, fmt );
		ret = vprintf( fmt, ap );
		va_end( ap );
		return ret;
	}
	return 0;
}

unsigned char axgsq_get_byte( unsigned char* cInput, int* iPos )
{
	unsigned char Ret = (unsigned char)cInput[ *iPos ];
	*iPos = *iPos + 1;
	return Ret;
}

short axgsq_get_short( unsigned char* cInput, int* iPos )
{
	unsigned short Ret = *(short*)&cInput[ *iPos ];
	*iPos = *iPos + 2;
	return Ret;
}

long axgsq_get_long( unsigned char* cInput, int* iPos )
{
	long Ret = *(long*)&cInput[ *iPos ];
	*iPos = *iPos + 4;
	return Ret;
}

float axgsq_get_float( unsigned char* cInput, int* iPos )
{
	float Ret = *(float*)&cInput[ *iPos ];
	*iPos = *iPos + 4;
	return Ret;
}

unsigned char* axgsq_get_string( unsigned char* cInput, int* iPos )
{
	char* cTemp = strchr( (char*) cInput + (int) *iPos, 0 );
	int iStrLen = (int) ( (char*) cTemp - (char*) cInput + 1 - (int) *iPos );
	if( iStrLen != -1 )
	{
		unsigned char* Ret = (unsigned char*) malloc( iStrLen + 1 );
		if( Ret == NULL )
		{
			axgsq_error( "Memory allocation error in axgsq_get_string();\n" );
			return NULL;
		}
		int x;
		for( x = 0; x <= iStrLen; x++ )
			Ret[ x ] = cInput[ *iPos + x ];
		//free( &x );
		*iPos = *iPos + iStrLen;
		//free( &iStrLen );
		return Ret;
	}
	//free( &iStrLen );
	return NULL;
}
