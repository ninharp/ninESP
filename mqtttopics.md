---
title: mqtttopics
---
# MQTT Topic List
<table border="1" align="center" width="80%">
<tr><th>Topic</th><th>Description</th></tr>
<tr><td colspan="2"><center><b>MQTT Settings Page</b></center></td></tr>
<tr><td>LWT Topic</td><td>Default: <i>lwt/client</i><br><br>Last Will and Testament Topic<br>In this topic the actual IP Adress will be published or after timeout a "Offline" string</td></tr>
<tr><td>Command Topic</td><td>Default: <i>cmd/client</i><br><br>General Command Topic where global commands can be run (not implemented yet)</td></tr>
<tr><td>Publish Topic</td><td>Default: <i>clients/client</i><br><br>General Publiush Topic where global message can be seen (not implemented yet)</td></tr>
<tr><td colspan="2"><center><b>Peripheral Settings Page - Relay Module</b></center></td></tr>
<tr><td>Switch Command Topic</td><td>Default: <i>client/relay</i><br><br>Switch the state of the relay<br>"on" or 1 enables relay<br>"off" or 0 disables relay<br><br>Should be retained and QoS of at least 1 to recover the last state on reboot</td></tr>
<tr><td>Switch Publish Topic</td><td>Default: <i>client/relaystate</i><br><br>Actual state of the relay</td></tr>
<tr><td colspan="2"><center><b>Peripheral Settings Page - Analog Sensor Module</b></center></td></tr>
<tr><td>Publish Topic ADC Value</td><td>Default: <i>client/adc</i><br><br>Actual value of Analog Port</td></tr>
<tr><td colspan="2"><center><b>Peripheral Settings Page - RCSwitch Module</b></center></td></tr>
<tr><td>Command Topic Prefix</td><td>Default: <i>client/rcswitch/</i><br><br>Prefix to control the RC Plugs<br>For every defined RC Plug will be one topic reserved and could be switched then<br>the same way like the relay module for example:<br>client/rcswitch/1 for the first defined RC Plug</td></tr>
<tr><td colspan="2"><center><b>Peripheral Settings Page - Motion Sensor Module</b></center></td></tr>
<tr><td>Publish Topic</td><td>Default: <i>client/motion</i><br><br>Actual state of motion<br>0 for no motion detected<br>1 for motion deteced</td></tr>
<tr><td colspan="2"><center><b>Peripheral Settings Page - MAX72XX LED Matrix Module</b></center></td></tr>
<tr><td>Topic Prefix</td><td>Default: <i>client/display/</i><br><br>The Prefix of the control topic of the display</td></tr>
<tr><td>Enable Topic</td><td>Default: <i>enable (client/display/enable)</i><br><br>Enable or disable display output<br>0 for disable<br>1 for enable</td></tr>
<tr><td>Display Text Topic</td><td>Default: <i>text (client/display/text)</i><br><br>The text to display</td></tr>
<tr><td>Scrolling Topic</td><td>Default: <i>scroll (client/display/scroll)</i><br><br>Enable scrolling of the displayed text<br>0 for disable<br>1 for enable</td></tr>
<tr><td>Scrolling Speed Topic</td><td>Default: <i>speed (client/display/speed)</i><br><br>The delay of the text effect in ms.<br>Should be a value between 1 and 200</td></tr>
<tr><td>Charwidth Topic</td><td>Default: <i>charwidth (client/display/charwidth)</i><br><br>The amount of pixels between every character displayed<br>Should be a value between 1 and 5</td></tr>
<tr><td>Intensity Topic</td><td>Default: <i>intensity (client/display/intensity)</i><br><br>Sets the intensity of the display<br>Should be a value between 0 and 15 (default is 9)</td></tr>
<tr><td>Invert Display Topic</td><td>Default: <i>invert (client/display/invert)</i><br><br>Enable inverting of the displayed text<br>0 for disable<br>1 for enable</td></tr>
<tr><td>Text Alignment Topic</td><td>Default: <i>alignment (client/display/alignment)</i><br><br>Alignment of the displayed text<br>0 for left aligned<br>1 for centered text<br>2 for right aligned</td></tr>
<tr><td>Pause Time Topic</td><td>Default: <i>pause (client/display/pause)</i><br><br>Delay time in ms of the staying of the displayed text if an text out effect is greater or equal than 1</td></tr>
<tr><td>Effect In Topic</td><td>Default: <i> effectin (client/display/effectin)</i><br><br>
<table border="1">
<tr><td>0</td><td>Used as a place filler, executes no operation</td></tr>
<tr><td>1</td><td>Text just appears (printed)</td></tr>
<tr><td>2</td><td>Text scrolls up through the display</td></tr>
<tr><td>3</td><td>Text scrolls down through the display</td></tr>
<tr><td>4</td><td>Text scrolls right to left on the display</td></tr>
<tr><td>5</td><td>Text scrolls left to right on the display</td></tr>
<tr><td>6</td><td>Text enters and exits using user defined sprite</td></tr>
<tr><td>7</td><td>Text enters and exits a slice (column) at a time from the right</td></tr>
<tr><td>8</td><td>Text enters and exits in columns moving in alternate direction (U/D)</td></tr>
<tr><td>9</td><td> Text enters and exits by fading from/to 0 and intensity setting</td></tr>
<tr><td>10</td><td>Text dissolves from one display to another</td></tr>
<tr><td>11</td><td>Text is replaced behind vertical blinds</td></tr>
<tr><td>12</td><td>Text enters and exits as random dots</td></tr>
<tr><td>13</td><td>Text appears/disappears one column at a time, looks like it is wiped on and off</td></tr>
<tr><td>14</td><td>WIPE with a light bar ahead of the change</td></tr>
<tr><td>15</td><td>Scan the LED column one at a time then appears/disappear at end</td></tr>
<tr><td>16</td><td>Scan a blank column through the text one column at a time then appears/disappear at end</td></tr>
<tr><td>17</td><td>Scan the LED row one at a time then appears/disappear at end</td></tr>
<tr><td>18</td><td>Scan a blank row through the text one row at a time then appears/disappear at end</td></tr>
<tr><td>19</td><td>Appear and disappear from the center of the display, towards the ends</td></tr>
<tr><td>20</td><td>OPENING with light bars ahead of the change</td></tr>
<tr><td>21</td><td>Appear and disappear from the ends of the display, towards the middle</td></tr>
<tr><td>22</td><td>CLOSING with light bars ahead of the change</td></tr>
<tr><td>23</td><td>Text moves in/out in a diagonal path up and left (North East)</td></tr>
<tr><td>24</td><td>Text moves in/out in a diagonal path up and right (North West)</td></tr>
<tr><td>25</td><td>Text moves in/out in a diagonal path down and left (South East)</td></tr>
<tr><td>26</td><td>Text moves in/out in a diagonal path down and right (North West)</td></tr>
<tr><td>27</td><td>Text grows from the bottom up and shrinks from the top down</td></tr>
<tr><td>28</td><td>Text grows from the top down and and shrinks from the bottom up</td></tr>
</table></td></tr>
<tr><td>Effect Out Topic</td><td>Default: <i>effectout (client/display/effectout)</i><br><br>Same list as in Effect in Topic description. For a static text use no effect (0)</td></tr>
<tr><td>Reset Display Option Topic</td><td>Default: <i>reset (client/display/reset)</i><br><br>Reset Display animations on changing settings?<br>0 for disable<br>1 for enable (default)</td></tr>
</table>