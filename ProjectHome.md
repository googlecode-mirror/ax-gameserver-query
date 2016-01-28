**Notice**<br>
The trunk of this project is the current development state and is not guaranteed to run or work.<br>
For working builds look on the Downloads page of this project.<br>
<br>
<br>
<b>Introduction</b><br>
The purpose of this project is to create a game sever query library and a PHP extension wrapper for it.<br>
The project was started due to there not being a PHP extension for querying source engine game servers.  I have created a PHP class for the job of querying them in the past but the speed offered by compiled c code is must greater than the speed of the PHP classes that are available, so hence this project was created.<br>
<br>
This projects latest build (0.1.40) only supports source engine servers but we are currently working on support for other game server and game querying protocols.<br>
<br>
<br>
<b>Download Links</b><br>
AX GameServer Query v0.1.40 (binary-win32) (.zip): <a href='http://ax-gameserver-query.googlecode.com/files/php_ax_gameserver_query%20v0.1.40.zip'>Download</a><br>
AX GameServer Query v0.1.40 (source) (.tar.gz): <a href='http://ax-gameserver-query.googlecode.com/files/ax_gameserver_query%20v0.1.40.tar.gz'>Download</a><br>
AX GameServer Query v0.1.40 (source) (.zip): <a href='http://ax-gameserver-query.googlecode.com/files/ax_gameserver_query%20v0.1.40.zip'>Download</a><br>
<br>
<br>
<b>Supported Games in v0.1.40</b><br>
<ul><li>Source (Counter-Strike:Source, HL2:Deathmatch, ect)<br>
</li><li>The Ship<br>
<br>
<br>
<b>Samples</b><br>
PHP:<br>
<pre><code>&lt;?php
$res = axgsq_connect( AXGSQ_SOURCE, "[server ip or hostname]", [server port] );
if( is_resource( $res ) )
{
    $server_info = axgsq_get_serverinfo( $res );
    axgsq_disconnect( $res );
    echo "Server Name: {$server_info["ServerInfo"]["ServerName"]}&lt;br/&gt;\n";
    echo "Map: {$server_info["ServerInfo"]["Map"]}&lt;br/&gt;\n";
}
?&gt;
</code></pre>
C:<br>
<pre><code>#include &lt;stdio.h&gt;
#include "ax_gameserver_query.h"

int main( int argv, char** argv )
{
    struct axgsq_res* pResource = axgsq_connect( AXGSQ_SOURCE, "[server ip or hostname]", [server port] );
    if( pResource != NULL )
    {
        struct axgsq_serverinfo* pServerInfo = axgsq_get_serverinfo( pResource );
        struct axgsq_serverinfo_source* pSI = (struct axgsq_serverinfo_source*) pServerInfo-&gt;ServerInfo;
        axgsq_disconnect( pResource );
        printf( "Server Name: %s\n", pSI-&gt;ServerName );
        printf( "Map: %s\n", pSI-&gt;Map );
    }
    return 0;
}
</code></pre>