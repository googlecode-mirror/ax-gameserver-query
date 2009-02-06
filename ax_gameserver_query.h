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
	AXGSQ_SOURCE,
	AXGSQ_THESHIP
};

struct axgsq_serverinfo
{
	int iGameServer;
	void* pSI;
};

struct axgsq_serverinfo_source
{
	unsigned char Version;
	unsigned char* ServerName;
	unsigned char* Map;
	unsigned char* GameDirectory;
	unsigned char* GameDescription;
	unsigned short AppID;
	unsigned char NumberOfPlayers;
	unsigned char MaximumPlayers;
	unsigned char NumberOfBots;
	unsigned char Dedicated;
	unsigned char OS;
	unsigned char Password;
	unsigned char Secure;
	unsigned char* GameVersion;
	unsigned char NumPlayers;
	struct
	{
		int Index;
		unsigned char* PlayerName;
		int Kills;
		float TimeConnected;
	} Players[64];
};

struct axgsq_serverinfo_theship
{
	unsigned char Version;
	unsigned char* ServerName;
	unsigned char* Map;
	unsigned char* GameDirectory;
	unsigned char* GameDescription;
	unsigned short AppID;
	unsigned char NumberOfPlayers;
	unsigned char MaximumPlayers;
	unsigned char NumberOfBots;
	unsigned char Dedicated;
	unsigned char OS;
	unsigned char Password;
	unsigned char Secure;
	unsigned char GameMode;
	unsigned char WitnessCount;
	unsigned char WitnessTime;
	unsigned char* GameVersion;
	unsigned char NumPlayers;
	struct
	{
		int Index;
		unsigned char* PlayerName;
		int Kills;
		float TimeConnected;
	} Players[64];
};


void axgsq_debug( int sDebug );
struct axgsq_res* axgsq_connect( int iGameServer, const char* cConnectionString, int iPort );
int axgsq_disconnect( struct axgsq_res* pResource );
struct axgsq_serverinfo* axgsq_get_serverinfo( struct axgsq_res* pResource );

#endif /* _AX_GAMESERVER_QUERY_H */
