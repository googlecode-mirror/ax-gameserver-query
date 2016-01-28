## The Ship ##


---

**A2S\_INFO**

---


Client Sends:
```
FF FF FF FF 54 53 6F 75 72 63 65 20 45 6E 67 69 | ÿÿÿÿTSource Engi 
6E 65 20 51 75 65 72 79 00                      | ne Query
```

Server Sends:
| **Data** | **Type** | **Comment** |
|:---------|:---------|:------------|
| Header   | long     | Should be equal to -1 (0xFFFFFFFF). |
| Type     | byte     | Should be equal to 'l' (0x49) |
| Version  | byte     | Network version. 0x07 is the current Steam version.  Goldsource games will return 48 (0x30), also referred to as protocol version. |
| Server Name | string   | The Source server name. |
| Map      | string   | The current map being played. |
| Game Directory | string   | The name of the folder containing the game files |
| Game Description | string   | A friendly string name for the game type |
| AppID    | short    | [See here](http://developer.valvesoftware.com/wiki/Steam_Application_IDs). |
| Number Of Players | byte     | The number of players currently on the server. |
| Maximum Players | byte     | The maximum allowed players for the server. |
| Number Of Bots | byte     | Number of bot players currently on the server. |
| Dedicated | byte     | 'l' for listen, 'd' for dedicated, 'p' for SourceTV. |
| OS       | byte     | Host operating system. 'l' for Linux, 'w' for Windows. |
| Password | byte     | If set to 0x01, a password is required to join this server. |
| Secure   | byte     | If set to 0x01, this server is VAC secured. |
| Game Mode | byte     | 0x00 for Hunt, 0x01 for Elimination, 0x02 for Duel, 0x03 for Deathmatch, 0x04 for Team VIP, 0x05 for Team Elimination |
| Witness Count | byte     | The minimum number of witnesses for a player to be arrested. |
| Witness Time | byte     | Time in seconds before player is arrested while being witnessed. |
| Game Version | string   | The version of the game. |


---

**A2S\_SERVERQUERY\_GETCHALLENGE**

---

<br>Quote Valve Dev wiki:<br>
<pre><code>*Note:* You can also send A2S_PLAYER and A2S_RULES queries with a challenge value of -1 (0xFF FF FF FF) and they will respond with a challenge value to use.<br>
*Warning:* As of November 2, 2008, the implementation of this query does not function in Goldsource servers.<br>
</code></pre>

Client Sends:<br>
<pre><code>FF FF FF FF 57                                  | ÿÿÿÿW<br>
</code></pre>

Server Sends:<br>
<table><thead><th> <b>Data</b> </th><th> <b>Type</b> </th><th> <b>Comment</b> </th></thead><tbody>
<tr><td> Header      </td><td> long        </td><td> Should be equal to -1 (0xFFFFFFFF). </td></tr>
<tr><td> Type        </td><td> byte        </td><td> Should be equal to 'A' (0x41). </td></tr>
<tr><td> Challenge   </td><td> long        </td><td> The challenge number to use. </td></tr></tbody></table>

<hr />
<b>A2S_PLAYER</b>
<hr />
Client Sends:<br>
<pre><code>FF FF FF FF 55 .. .. .. ..                      | ÿÿÿÿU....<br>
</code></pre>

Server Sends:<br>
<table><thead><th> <b>Data</b> </th><th> <b>Type</b> </th><th> <b>Comment</b> </th></thead><tbody>
<tr><td> Header      </td><td> long        </td><td> Should be equal to -1 (0xFFFFFFFF). </td></tr>
<tr><td> Type        </td><td> byte        </td><td> Should be equal to 'D' (0x44). </td></tr>
<tr><td> Num Players </td><td> byte        </td><td> The number of players reported in this responce. </td></tr></tbody></table>

The for each player the server sends:<br>
<table><thead><th> <b>Data</b> </th><th> <b>Type</b> </th><th> <b>Comment</b> </th></thead><tbody>
<tr><td> Index       </td><td> byte        </td><td> The index for this entry. </td></tr>
<tr><td> Player Name </td><td> string      </td><td> Player's Name. </td></tr>
<tr><td> Kills       </td><td> long        </td><td> Number of kills this player has. </td></tr>
<tr><td> Time Connected </td><td> float       </td><td> The time in seconds this player has been connected. </td></tr></tbody></table>

<hr />
<b>A2S_RULES</b>
<hr />
Client Sends:<br>
<pre><code>FF FF FF FF 56 .. .. .. ..                      | ÿÿÿÿV....<br>
</code></pre>

Server Sends:<br>
<table><thead><th> <b>Data</b> </th><th> <b>Type</b> </th><th> <b>Comment</b> </th></thead><tbody>
<tr><td> Header      </td><td> long        </td><td> Should be equal to -1 (0xFFFFFFFF). </td></tr>
<tr><td> Type        </td><td> byte        </td><td> Should be equal to 'E' (0x45). </td></tr>
<tr><td> Num Rules   </td><td> short       </td><td> The number of rules reported in this response. </td></tr></tbody></table>

Then for each rule the following fields are sent:<br>
<table><thead><th> <b>Data</b> </th><th> <b>Type</b> </th><th> <b>Comment</b> </th></thead><tbody>
<tr><td> Rule Name   </td><td> string      </td><td> The name of the rule. </td></tr>
<tr><td> Rule Value  </td><td> string      </td><td> The rule's value. </td></tr></tbody></table>

<hr />