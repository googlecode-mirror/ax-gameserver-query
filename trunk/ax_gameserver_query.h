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
	AXGSQ_ASE,
	AXGSQ_GAMESPY,
	AXGSQ_GAMESPY2,
	AXGSQ_GAMESPY3,
	AXGSQ_SOURCE,
	AXGSQ_THESHIP
	/*AXGSQ_HALO*/
};

struct axgsq_serverinfo
{
	int GameServer;
	void* ServerInfo;
};

struct axgsq_serverinfo_keyval
{
	unsigned char* Key;
	unsigned char* Value;
};

struct axgsq_serverinfo_gamespy
{
	unsigned char NumInfo;
	struct axgsq_serverinfo_keyval* Info;
	unsigned char NumPlayer;
	struct axgsq_serverinfo_keyval* Player;
	unsigned char NumTeam;
	struct axgsq_serverinfo_keyval* Team;
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

/*
struct axgsq_serverinfo_halo
{
	unsigned char* Hostname;
	unsigned char* GameVer;
	unsigned char* HostPort;
	unsigned char MaxPlayers;
	unsigned char Password;
	unsigned char* MapName;
	unsigned char Dedicated;
	unsigned char* GameMode;
	unsigned char Game_Classic;
	unsigned char NumPlayers;
	unsigned char* GameType;
	unsigned char TeamPlay;
	unsigned char* GameVariant;
	unsigned char FragLimit;
	unsigned char* Player_Flags;
	unsigned char* Game_Flags;
	struct
	{
		unsigned char* PlayerName;
		unsigned char Score;
		unsigned char* Ping;
		unsigned char Team;
	} Players[64];
	unsigned char TeamRedScore;
	unsigned char TeamBlueScore;
};
*/


void axgsq_debug( int iDebug );
struct axgsq_res* axgsq_connect( int iGameServer, const char* cConnectionString, int iPort );
int axgsq_disconnect( struct axgsq_res* pResource );
struct axgsq_serverinfo* axgsq_get_serverinfo( struct axgsq_res* pResource );
void axgsq_dealloc( struct axgsq_serverinfo* pSI );

#endif /* _AX_GAMESERVER_QUERY_H */
