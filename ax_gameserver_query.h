#ifndef _AX_GAMESERVER_QUERY_H
#define _AX_GAMESERVER_QUERY_H

#ifdef _WIN32
#include <winsock2.h>
#else /* _WIN32 */
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* _WIN32 */

struct axgsq_res
{
#ifdef _WIN32
	SOCKET pSocket;
#else /* _WIN32 */
	int pSocket;
#endif /* _WIN32 */
	struct sockaddr_in pSockAddr;
	int iGameServer;
	int iSocketType;
	int iSocketProtocol;
};

enum
{
	AXGSQ_SOURCE
};

struct axgsq_serverinfo
{
	int iGameServer;
	void* pSI;
};

struct axgsq_serverinfo_source
{
	int Type;
	int Version;
	char* ServerName;
	char* Map;
	char* GameDirectory;
	char* GameDescription;
	int AppID;
	int NumberOfPlayers;
	int MaximumPlayers;
	int NumberOfBots;
	char Dedicated;
	char OS;
	int Password;
	int Secure;
	char* GameVersion;
};



struct axgsq_res* axgsq_connect( int iGameServer, const char* cConnectionString, int iPort );
int axgsq_disconnect( struct axgsq_res* pResource );
struct axgsq_serverinfo* axgsq_get_serverinfo( struct axgsq_res* pResource );

#endif /* _AX_GAMESERVER_QUERY_H */