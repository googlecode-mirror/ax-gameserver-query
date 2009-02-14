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
#include <string.h>
#include "ax_gameserver_query.h"

int axgsq_isIpAddress( const char* cConnectionString );
int axgsq_istrstr( int _StartPos, const unsigned char* _Str, int _StrLen, const unsigned char* _SubStr, int _SubStrLen );
#ifdef _WIN32
int axgsq_send( SOCKET s, const unsigned char *buf, int len, int flags );
int axgsq_recv( SOCKET s, unsigned char *buf, int len, int flags );
#else /*_WIN32*/
int axgsq_send( int s, const unsigned char *buf, int len, int flags );
int axgsq_recv( int s, unsigned char *buf, int len, int flags );
#endif /*_WIN32*/
unsigned char axgsq_get_byte( unsigned char* cInput, int* iPos );
short axgsq_get_short( unsigned char* cInput, int* iPos );
long axgsq_get_long( unsigned char* cInput, int* iPos );
float axgsq_get_float( unsigned char* cInput, int* iPos );
unsigned char* axgsq_get_string( unsigned char* cInput, int* iPos );

char* axgsq_version( void )
{
	return (char*)"v0.1.40";
}

struct axgsq_res* axgsq_connect( int iGameServer, const char* cConnectionString, int iPort )
{
	struct hostent *pHost;
	struct axgsq_res *pResource;
#ifdef _WIN32
	WSADATA* pWSAData;
#endif /* _WIN32 */
	pResource = (struct axgsq_res*) malloc( sizeof( struct axgsq_res ) );
	if( pResource == NULL )
	{
		free( pResource );
		return NULL;
	}
	memset( pResource, 0, sizeof( struct axgsq_res ) );
	pResource->iGameServer = iGameServer;
	switch( iGameServer )
	{
	case AXGSQ_SOURCE:
	case AXGSQ_THESHIP:
		pResource->iSocketType = SOCK_DGRAM;
		pResource->iSocketProtocol = IPPROTO_UDP;
		break;
	default:
		free( pResource );
		return NULL;
	}
#ifdef _WIN32
	pWSAData = (WSADATA*) malloc( sizeof( WSADATA ) );
	if( pWSAData == NULL )
	{
		free( pResource );
		free( pWSAData );
	}
	memset( pWSAData, 0, sizeof( WSADATA ) );
	if( WSAStartup( MAKEWORD( 2, 0 ), pWSAData ) != 0 )
	{
		free( pResource );
		free( pWSAData );
		return NULL;
	}
	free( pWSAData );
#endif /* _WIN32 */
	pResource->pSocket = socket( AF_INET, pResource->iSocketType, pResource->iSocketProtocol );
#ifdef _WIN32
	if( pResource->pSocket == INVALID_SOCKET )
	{
		free( pResource );
		WSACleanup();
		return NULL;
	}
#else /* _WIN32 */
	if( pResource->pSocket < 0 )
	{
		free( pResource );
		return NULL;
	}
#endif /* _WIN32 */
	pResource->pSockAddr.sin_family = AF_INET;
	if( axgsq_isIpAddress( cConnectionString ) )
		pResource->pSockAddr.sin_addr.s_addr = inet_addr( cConnectionString );
	else
	{
		if( ( pHost = gethostbyname( cConnectionString ) ) == NULL )
		{
			free( pResource );
			return NULL;
		}
		pResource->pSockAddr.sin_addr.s_addr = *((unsigned long*)pHost->h_addr_list[0]);
	}
	pResource->pSockAddr.sin_port = htons( (u_short)iPort );
#ifdef _WIN32
	if( connect( pResource->pSocket, (struct sockaddr*)&pResource->pSockAddr, sizeof( pResource->pSockAddr ) ) == -1 )
	{
		free( pResource );
		WSACleanup();
		return NULL;
	}
#else /* _WIN32 */
	if( connect( pResource->pSocket, (struct sockaddr*)&pResource->pSockAddr, sizeof( pResource->pSockAddr ) ) < 0 )
	{
		free( pResource );
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
	struct axgsq_serverinfo *pServerInfo;
	void *pSI;
	unsigned char *cInput;
	unsigned char *cChallenge;
	unsigned char *cTemp;
	int iPos = 0;
	int x;
	if( pResource != NULL )
	{
		pServerInfo = (struct axgsq_serverinfo*) malloc( sizeof(struct axgsq_serverinfo) );
		if( pServerInfo == NULL )
		{
			free( pServerInfo );
			return NULL;
		}
		memset( pServerInfo, 0, sizeof(struct axgsq_serverinfo) );
		if( pResource->iGameServer == AXGSQ_SOURCE )
		{
			pServerInfo->GameServer = pResource->iGameServer;
			pSI = (struct axgsq_serverinfo_source*) malloc( sizeof(struct axgsq_serverinfo_source) );
			if( pSI == NULL )
			{
				free( pServerInfo );
				free( pSI );
				return NULL;
			}
			memset( pSI, 0, sizeof( pSI ) );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x54Source Engine Query\x00", 25, 0 ) == -1 )
			{
				free( pServerInfo );
				free( pSI );
				return NULL;
			}
			cInput = (unsigned char*) malloc( 2049 );
			if( cInput == NULL )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			iPos = 5;
			((struct axgsq_serverinfo_source *)pSI)->Version = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->ServerName = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->Map = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->GameDirectory = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->GameDescription = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->AppID = axgsq_get_short( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->NumberOfPlayers = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->MaximumPlayers = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->NumberOfBots = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->Dedicated = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->OS = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->Password = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->Secure = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_source *)pSI)->GameVersion = axgsq_get_string( cInput, &iPos );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x55\xFF\xFF\xFF\xFF", 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			iPos = 5;
			cChallenge = (unsigned char*) malloc( 5 );
			if( cChallenge == NULL )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				free( cChallenge );
				return NULL;
			}
			cChallenge[0] = axgsq_get_byte( cInput, &iPos );
			cChallenge[1] = axgsq_get_byte( cInput, &iPos );
			cChallenge[2] = axgsq_get_byte( cInput, &iPos );
			cChallenge[3] = axgsq_get_byte( cInput, &iPos );
			cChallenge[4] = 0;
			cTemp = (unsigned char *) malloc( 10 );
			if( cTemp == NULL )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				free( cChallenge );
				free( cTemp );
				return NULL;
			}
			sprintf( (char *)cTemp, "\xFF\xFF\xFF\xFF\x55%s", cChallenge );
			free( cChallenge );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)cTemp, 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				free( cTemp );
				return NULL;
			}
			free( cTemp );
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			iPos = 5;
			((struct axgsq_serverinfo_source *)pSI)->NumPlayers = axgsq_get_byte( cInput, &iPos );
			for( x = 0; x < ((struct axgsq_serverinfo_source *)pSI)->NumPlayers; x++ )
			{
				((struct axgsq_serverinfo_source *)pSI)->Players[x].Index = axgsq_get_byte( cInput, &iPos );
				((struct axgsq_serverinfo_source *)pSI)->Players[x].PlayerName = axgsq_get_string( cInput, &iPos );
				((struct axgsq_serverinfo_source *)pSI)->Players[x].Kills = axgsq_get_long( cInput, &iPos );
				((struct axgsq_serverinfo_source *)pSI)->Players[x].TimeConnected = axgsq_get_float( cInput, &iPos );
			}
			free( cInput );
			pServerInfo->ServerInfo = pSI;
		}
		else if( pResource->iGameServer == AXGSQ_THESHIP )
		{
			pServerInfo->GameServer = pResource->iGameServer;
			pSI = (struct axgsq_serverinfo_theship*) malloc( sizeof(struct axgsq_serverinfo_theship) );
			if( pSI == NULL )
			{
				free( pServerInfo );
				//free( cInput );
				free( pSI );
				return NULL;
			}
			memset( pSI, 0, sizeof( pSI ) );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x54Source Engine Query\x00", 25, 0 ) == -1 )
			{
				free( pServerInfo );
				//free( cInput );
				free( pSI );
				return NULL;
			}
			cInput = (unsigned char*) malloc( 2049 );
			if( cInput == NULL )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			iPos = 5;
			((struct axgsq_serverinfo_theship *)pSI)->Version = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->ServerName = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->Map = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->GameDirectory = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->GameDescription = axgsq_get_string( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->AppID = axgsq_get_short( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->NumberOfPlayers = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->MaximumPlayers = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->NumberOfBots = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->Dedicated = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->OS = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->Password = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->Secure = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->GameMode = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->WitnessCount = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->WitnessTime = axgsq_get_byte( cInput, &iPos );
			((struct axgsq_serverinfo_theship *)pSI)->GameVersion = axgsq_get_string( cInput, &iPos );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)"\xFF\xFF\xFF\xFF\x55\xFF\xFF\xFF\xFF", 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, (unsigned char *)cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			iPos = 5;
			cChallenge = (unsigned char*) malloc( 5 );
			if( cChallenge == NULL )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				free( cChallenge );
				return NULL;
			}
			cChallenge[0] = axgsq_get_byte( cInput, &iPos );
			cChallenge[1] = axgsq_get_byte( cInput, &iPos );
			cChallenge[2] = axgsq_get_byte( cInput, &iPos );
			cChallenge[3] = axgsq_get_byte( cInput, &iPos );
			cChallenge[4] = 0;
			cTemp = (unsigned char *) malloc( 10 );
			if( cTemp == NULL )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				free( cChallenge );
				free( cTemp );
				return NULL;
			}
			sprintf( (char *)cTemp, "\xFF\xFF\xFF\xFF\x55%s", cChallenge );
			free( cChallenge );
			if( axgsq_send( pResource->pSocket, (const unsigned char *)cTemp, 9, 0 ) == -1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				free( cTemp );
				return NULL;
			}
			free( cTemp );
			memset( cInput, 0, 2049 );
			if( axgsq_recv( pResource->pSocket, cInput, 2048, 0 ) < 1 )
			{
				free( pServerInfo );
				free( cInput );
				free( pSI );
				return NULL;
			}
			iPos = 5;
			((struct axgsq_serverinfo_theship *)pSI)->NumPlayers = axgsq_get_byte( cInput, &iPos );
			for( x = 0; x < ((struct axgsq_serverinfo_theship *)pSI)->NumPlayers; x++ )
			{
				((struct axgsq_serverinfo_theship *)pSI)->Players[x].Index = axgsq_get_byte( cInput, &iPos );
				((struct axgsq_serverinfo_theship *)pSI)->Players[x].PlayerName = axgsq_get_string( cInput, &iPos );
				((struct axgsq_serverinfo_theship *)pSI)->Players[x].Kills = axgsq_get_long( cInput, &iPos );
				((struct axgsq_serverinfo_theship *)pSI)->Players[x].TimeConnected = axgsq_get_float( cInput, &iPos );
			}
			free( cInput );
			pServerInfo->ServerInfo = pSI;
		}
		else
		{
			free( pServerInfo );
			//free( cInput );
			return NULL;
		}
		return pServerInfo;
	}
	else
	{
		return NULL;
	}
}

void axgsq_dealloc( struct axgsq_serverinfo* pSI )
{
	int x;
	void* pSIs = pSI->ServerInfo;
	switch( pSI->GameServer )
	{
	case AXGSQ_SOURCE:
		free( ((struct axgsq_serverinfo_source*)pSIs)->ServerName );
		free( ((struct axgsq_serverinfo_source*)pSIs)->Map );
		free( ((struct axgsq_serverinfo_source*)pSIs)->GameDirectory );
		free( ((struct axgsq_serverinfo_source*)pSIs)->GameDescription );
		free( ((struct axgsq_serverinfo_source*)pSIs)->GameVersion );
		for( x = 0; x < ((struct axgsq_serverinfo_source*)pSIs)->NumPlayers; x++ )
			free( ((struct axgsq_serverinfo_source*)pSIs)->Players[x].PlayerName );
		free( pSI->ServerInfo );
		free( pSI );
		break;
	case AXGSQ_THESHIP:
		free( ((struct axgsq_serverinfo_theship*)pSIs)->ServerName );
		free( ((struct axgsq_serverinfo_theship*)pSIs)->Map );
		free( ((struct axgsq_serverinfo_theship*)pSIs)->GameDirectory );
		free( ((struct axgsq_serverinfo_theship*)pSIs)->GameDescription );
		free( ((struct axgsq_serverinfo_theship*)pSIs)->GameVersion );
		for( x = 0; x < ((struct axgsq_serverinfo_theship*)pSIs)->NumPlayers; x++ )
			free( ((struct axgsq_serverinfo_theship*)pSIs)->Players[x].PlayerName );
		free( pSI->ServerInfo );
		free( pSI );
		break;
	}
}


/*
	HELPER FUNCTIONS
*/

int axgsq_isIpAddress( const char* cConnectionString )
{
	int x;
	int len = (int)strlen( cConnectionString );
	if( len <= 15 && len >= 7 )
	{
		for( x = 0; x < len; x++ )
		{
			if( !( ( cConnectionString[x] >= 0x30 && cConnectionString[x] <= 0x39 ) || cConnectionString[x] == 0x2E ) )
			{
				return 0;
			}
		}
		return 1;
	}
	return 0;
}

int axgsq_istrstr( int _StartPos, const unsigned char* _Str, int _StrLen, const unsigned char* _SubStr, int _SubStrLen )
{
	int x, Match = 0;
	for( x = _StartPos; x < _StrLen; x++ )
	{
		if( _Str[x] == _SubStr[Match] )
			Match++;
		else
			Match = 0;
		if( Match == _SubStrLen )
			return x;
	}
	return -1;
}

#ifdef _WIN32
int axgsq_send( SOCKET s, const unsigned char *buf, int len, int flags )
#else /*_WIN32*/
int axgsq_send( int s, const unsigned char *buf, int len, int flags )
#endif /*_WIN32*/
{
	int tmp;
	fd_set set;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO( &set );
	FD_SET( s, &set );
	tmp = select( s+1, NULL, &set, NULL, &tv );
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
	int tmp;
	fd_set set;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO( &set );
	FD_SET( s, &set );
	tmp = select( s+1, &set, NULL, NULL, &tv );
	if( tmp > 0 )
		return recv( s, (char *)buf, len, flags );
	return -1;
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
	int x;
	unsigned char* Ret;
	char* cTemp = strchr( (char*) cInput + (int) *iPos, 0 );
	int iStrLen = (int) ( (char*) cTemp - (char*) cInput + 1 - (int) *iPos );
	if( iStrLen != -1 )
	{
		Ret = (unsigned char*) malloc( iStrLen + 1 );
		if( Ret == NULL )
		{
			return NULL;
		}
		for( x = 0; x <= iStrLen; x++ )
			Ret[ x ] = cInput[ *iPos + x ];
		*iPos = *iPos + iStrLen;
		return Ret;
	}
	return NULL;
}
