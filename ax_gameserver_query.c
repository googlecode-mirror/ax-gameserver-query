#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ax_gameserver_query.h"

int axgsq_isIpAddress( const char* cConnectionString );
unsigned char axgsq_get_byte( unsigned char* cInput, int* iPos );
unsigned short axgsq_get_short( unsigned char* cInput, int* iPos );
unsigned char* axgsq_get_string( unsigned char* cInput, int* iPos );

struct axgsq_res* axgsq_connect( int iGameServer, const char* cConnectionString, int iPort )
{
	// Init our resource struct
	struct axgsq_res* pResource = (struct axgsq_res*) malloc( sizeof( struct axgsq_res ) );
	if( pResource == NULL )
	{
		// Memory allocation error
		printf( "Memory allocation error in axgsq_connect();\n" );
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
		iSocketType = SOCK_DGRAM;
		iSocketProtocol = IPPROTO_UDP;
		break;
	default:
		printf( "Unknown gameserver type given to axgsq_connect();\n" );
		return NULL;
	}
	pResource->iSocketType = iSocketType;
	pResource->iSocketProtocol = iSocketProtocol;


#ifdef _WIN32
	WSADATA pWSAData;
	if( WSAStartup( MAKEWORD( 2, 0 ), &pWSAData ) != 0 )
	{
		// Winsock startup error
		printf( "Winsock starup error in axgsq_connect();\n" );
		return NULL;
	}
#endif /* _WIN32 */
	pResource->pSocket = socket( AF_INET, iSocketType, iSocketProtocol );
#ifdef _WIN32
	if( pResource->pSocket == INVALID_SOCKET )
	{
		WSACleanup();
		printf( "Socket creation error in axgsq_connect();\n" );
		return NULL;
	}
#else /* _WIN32 */
	if( pResource->pSocket < 0 )
	{
		printf( "Socket creation error in axgsq_connect();\n" );
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
			printf( "Hostname resolution failed in axgsq_connect();\n" );
			return NULL;
		}
		pResource->pSockAddr.sin_addr.s_addr = *((unsigned long*)pHost->h_addr_list[0]);
	}
	pResource->pSockAddr.sin_port = htons( (u_short)iPort );
#ifdef _WIN32
	if( connect( pResource->pSocket, (struct sockaddr*)&pResource->pSockAddr, sizeof( pResource->pSockAddr ) ) == -1 )
	{
		WSACleanup();
		printf( "Socket connection error in axgsq_connect();\n" );
		return NULL;
	}
#else /* _WIN32 */
	if( connect( pResource->pSocket, (struct sockaddr*)&pResource->pSockAddr, sizeof( pResource->pSockAddr ) ) < 0 )
	{
		printf( "Socket connection error in axgsq_connect();\n" );
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
	struct axgsq_serverinfo* pServerInfo = (struct axgsq_serverinfo*) malloc( sizeof(struct axgsq_serverinfo) );
	if( pServerInfo == NULL )
		return NULL;
	memset( pServerInfo, 0, sizeof(struct axgsq_serverinfo) );

	unsigned char* cInput = NULL;
	int iPos = 0;
	switch( pResource->iGameServer )
	{
	case AXGSQ_SOURCE:
		pServerInfo->iGameServer = AXGSQ_SOURCE;
		if( send( pResource->pSocket, "\xFF\xFF\xFF\xFF\x54Source Engine Query\x00", 25, 0 ) == -1 )
			return NULL;
		cInput = (unsigned char*) malloc( 2049 );
		memset( cInput, 0, 2049 );
		if( recv( pResource->pSocket, (char*)cInput, 2048, 0 ) < 1 )
			return NULL;
		struct axgsq_serverinfo_source* pSI = (struct axgsq_serverinfo_source*) malloc( sizeof(struct axgsq_serverinfo_source) );
		if( pSI == NULL )
			return NULL;
		memset( pSI, 0, sizeof(struct axgsq_serverinfo_source) );
		iPos = 4;
		pSI->Type = axgsq_get_byte( cInput, &iPos );
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
		free( cInput );
		pServerInfo->pSI = pSI;
		break;
	default:
		printf( "Unknown gameserver type given to axgsq_serverinfo();\n" );
		return NULL;
	}
	return pServerInfo;
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
			if( !( ( cConnectionString[x] >= 0x30 && cConnectionString[x] <= 0x39 ) || cConnectionString[x] == 0x2E ) )
				return 0;
		return 1;
	}
	return 0;
}

unsigned char axgsq_get_byte( unsigned char* cInput, int* iPos )
{
	unsigned char Ret = (unsigned char)cInput[ *iPos ];
	*iPos = *iPos + 1;
	return Ret;
}

unsigned short axgsq_get_short( unsigned char* cInput, int* iPos )
{
	unsigned short Ret = ( cInput[ *iPos + 1 ] << 8 ) | cInput[ *iPos ];
	*iPos = *iPos + 2;
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
			printf( "Memory allocation error in axgsq_get_string();\n" );
			return NULL;
		}
		int x;
		for( x = 0; x <= iStrLen; x++ )
			Ret[ x ] = cInput[ *iPos + x ];
		*iPos = *iPos + iStrLen;
		return Ret;
	}
	return NULL;
}