## Functions ##

---


### axgsq\_debug ###

**Description**<br>
<pre><code>void axgsq_debug( int iDebug )<br>
</code></pre>
<b>axgsq_debug()</b> enables or disables the output of debugging information.<br>
<br><br><br>

<b>Parameters</b><br>
<i>iDebug</i><br>
<pre><code>If iDebug is equal to 1 then debugging information will be output to the console.<br>
If iDebug is equal to 0 then debugging information will not be output to the console.<br>
</code></pre>
<br>

<b>Return Values</b><br>
void<br>
<br><br>

<hr />
<h3>axgsq_connect</h3>

<b>Description</b><br>
<pre><code>struct axgsq_res * axgsq_connect( int iGameServer, const char * cConnectionString, int iPort )<br>
</code></pre>
<b>axgsq_connect()</b> creates a connection to a game server and makes it ready to start sending queries.<br>
<br><br><br>

<b>Parameters</b><br>
<i>iGameServer</i><br>
<pre><code>This should be one of the defined enums i.e. AXGSQ_SOURCE or AXGSQ_THESHIP.<br>
</code></pre>
<i>cConnectionString</i><br>
<pre><code>This can be eather an IP address or a hostname the function automaticly converts hostnames to IP addresses.<br>
</code></pre>
<i>iPort</i><br>
<pre><code>This should be the port that the gameserver is listening on.<br>
</code></pre>
<br>

<b>Return Values</b><br>
struct axgsq_res <code>*</code>
<br><br>

<hr />