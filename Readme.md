# Introduction

ninESP is a modular firmware for ESP8266 SoC based on SmingFramework.

It is designed to work with a few kinds of different sensors which you can attach to it
and then acts as an MQTT Client which serves you telemetry data or let you control things. 
So you can easily switch relays or simply as a relay server for RC Plugs 
(All RCSwitch compatible Plugs and Protocols) or just a timer based MQTT publish of sensor values. 
You can find all currently supported modules/sensors below in the "Peripheral Modules" section.

Also it can start an UDP Server and Serial Console to act/react for most of the peripherals, 
you can find a protocol description is down below.


Everything should be accessible and especially configurable from a simple webinterface.
The Webinterface based on the NetworkConfig Example of the SmingFramework.

On first run it starts a Wifi Accesspoint from itself with name *"ninHome Configuration"*
which you can connect from any Wifi enabled device and then connect with a webbrowser
to "http://192.168.4.1" to get into the webinterface.
There you can first configure the Wifi it should connect to, the mqtt settings and the 
connected periphals and the corresponding mqtt topics.
After you configured your ninESP you can find the current IP Address on the defined LWT MQTT 
Topic when the client is online, if it disconnects it will just say "Offline" in the LWT Topic.

In Version 0.2 the library for the MAX7219 control changed to a more modern and flexible
library which can do a lot of more animations. 
MD_MAX72xx and MD_Parola which are from [MajicDesigns](https://github.com/MajicDesigns) 
Since this version (0.2) your flash size has to be at least a 1M type. 

The firmware itself should be mostly compatible to the closed source firmware for the
inwall esp8266 which you can find and buy on ebay.

You can find the latest firmware version in a binary format in the firmware folder in the root of this repository

Also there is a [project description on hackaday.io](https://hackaday.io/project/28555-esp8266-max7219-dot-matrix-display-as-mqtt-client/)  about the MAX72xx Module in the ninESP

# Screenshots

![wifi networks](https://raw.github.com/ninharp/ninESP/master/doc/screenshots/web_networks_small.jpg "Wifi Network Settings")
![mqtt settings](https://raw.github.com/ninharp/ninESP/master/doc/screenshots/web_mqtt_small.jpg "MQTT Settings")

# Peripheral Modules

Peripheral modules which are currently build in or in development state

<table border="1" align="center" width="80%">
<tr><th>Periphery</th><th>Working state</th></tr>
<tr><td>Status LED on any free GPIO</td><td><font color="#11ff55"><b>working</b></font></td></tr>
<tr><td>Relay Module on any free GPIO<br>with optional Hardware switch support</td><td><font color="#11ff55"><b>working</b></font></td></tr>
<tr><td>Status LED on any free GPIO</td><td><font color="#11ff55"><b>working</b></font></td></tr>
<tr><td>Analog Sensor on Pin A0<br>Reference Voltage of 3v3</td><td><font color="#11ff55"><b>working</b></font></td></tr>
<tr><td>HC-SR501 Motion Sensor on any free GPIO</td><td><font color="#11ff55"><b>working</b></font></td></tr>
<tr><td>MAX7219 LED Matrix<br></td><td><font color="#11ff55"><b>working</b></font><br>currently only with Hardware SPI Support<br>Pins for HW SPI are IO13 (MOSI) and IO14 (CLK) the CS Pin is free assignable</td></tr>
<tr><td>RCSwitch Support for 868/433MHz Remote Plugs</td><td><font color="#ccddcc"><b>working</b></font><br>but configuration Webinterface missing, configuration of plugs currently in config file only</td></tr>
<tr><td>HD44780 LCD Display over I2C</td><td><font color="#dd0000"><b>not fully implemented</b></font></td></tr>
<tr><td>WS281x LED Strip</td><td><font color="#dd0000"><b>not fully implemented</b></font></td></tr>
<tr><td>HC-SR04 Ultrasonic Sensor</td><td><font color="#ff0000"><b>on the roadmap</b></font></td></tr>
</table><br>

# Known Working Boards/Chips

Here you find a list of development boards and modules which are known to work with ninESP firmware.
ESP-201 Modules will work but some of them are only manufactured with an 512k flash so it is too small
for the ninESP firmware.

<table border="1" align="center" width="80%">
<tr><th>Board / Module</th><th>Working state</th></tr>
<tr><td>WeMos D1 mini clone<br>
<img src="https://raw.github.com/ninharp/ninESP/master/doc/boards/WEMOSD1c.jpg" alt="wemosd1"></td><td>works</td></tr>
<tr><td>NodeMCU v0.9<br>
<img src="https://raw.github.com/ninharp/ninESP/master/doc/boards/NodeMCU09.jpg" alt="nodemcu"></td><td>works</td></tr>
<tr><td>ESP-201 Module<br>
<img src="https://raw.github.com/ninharp/ninESP/master/doc/boards/ESP201.jpg" alt="esp-201">
</td><td>work partially (need at least 1M flash)</td></tr>
</table><br>

# Dependencies

The project relies on SmingFramework for ESP8266, the SmingFramework is as submodule added but not the 
[esp-open-sdk](https://github.com/pfalcon/esp-open-sdk) , which you had to install also and by yourself.
As other submodules are the libraries of MajicDesigns included.
[MD_MAX72xx](https://github.com/MajicDesigns/MD_MAX72XX) - MAX72xx Driver Library
[MD_Parola](https://github.com/MajicDesigns/MD_Parola) - Animation Library for MAX72XX

~~It uses a slightly modified version of the SmingFramework MQTTClient Class, cause i needed
some small additions to that.~~

# How to use

If ninESP is configured correctly and is connected to your defined MQTT broker, you can send/receive data on the specified topics for the periphery you want to control,

Examples with mosquitto client:

#####MAX72XX Module:

In the examples we use a topic prefix of "client/display/"

Send a new text "Hello World" to Display with topic "client/display/text"
```mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/display/text -m "Hello World"```

Enable inverting of the Display with topic "client/display/invert"
```mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/display/invert -m 1```

#####Relay Module:

Switch relay to ON state on topic "client/relay"
```mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/relay -m 1```

#####RCSwitch Module:

In the examples we use a topic prefix of "client/rcswitch/"

Switch Plug 1 to OFF state on topic "client/rcswitch/1"
```mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/rcswitch/1 -m 0```

You can use any mqtt client to interact with ninESP, for example an mobile android mqtt client is 
[MQTT Dash](https://play.google.com/store/apps/details?id=net.routix.mqttdash)

# MQTT Topic Structure

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
<tr><td>Scrolling Topic</td><td>Default: <i>scroll (client/display/scroll)</i><br><br>Enable scrolling of the displayed text<br>0 for disable<br>1 for enable</td></td></tr>
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

# Serial Commands

You can interact with the firmware via the serial terminal and a baudrate of 115200baud 

<table border="1" align="center" width="80%">
<tr><th>Command</th><th>Reply</th></tr>
<tr><td>help</td><td>List of commands</td></tr>
<tr><td>ip</td><td>Dumps the actual ip and mac adress of the module.<br>
Example output:<pre>ip: 192.168.3.66 mac: ecfabc123458</pre></td></tr>
<tr><td>restart</td><td>Restarts the module</td></tr>
<tr><td>ls</td><td>Displays a list of files in the SPIFFS<br>
Example output:<pre>filecount 13
bootstrap-core.css.gz
periph.html
index.html
.mqtt_settings.conf
...</pre></td></tr>
<tr><td>info</td><td>Displays some system information<br>Example output:<pre>SDK: v1.5.2(80914727)
Free Heap: 28048
CPU Frequency: 80 MHz
System Chip ID: 143bc8
SPI Flash ID: 16405e
SPI Flash Size: 4194304</pre></td></tr>
</table><br>

# Flashing to your board

If you just want to flash the firmware without the compiling, you can take one of our binary releases from the firmware folder in the repository. 
There are zip archives with the firmware files in it. 
Its not one file builds so you have to flash it seperately. The flash addresses are the file names of the binaries.
Except of the spiff rom, which had to be in this case on memory address *0x5A000*. 
An example flashing command with esptool could look like this.

```esptool.py -p /dev/ttyUSB0 -b 230400 write_flash -ff 40m -fm qio -fs 32m 0x00000 0x00000.bin 0xa000 0xa000.bin 0x5A000 spiff_rom.bin```

# FAQ

*Q. My MAX7219 Displays are mirrored or just displaying crap, what can i do?*
A. Look in the MAX72XX Submodule folder and open the MAX72xx.h in edit mode and
    search for the MAX7219 module settings. There you can change the type of module you got
    and will hopefully fix the displaying issue.
    
*Q. The firmware want compile, it says "error: 'typedef' was ignored in this declaration"*
A. Apply the MD_Parola.patch from the repositorys root folder.
Or remove the "typedef" tag from MD_Parola.h by yourself

*Q. Can i see or download the configuration files for later use?*
A. You can see or download the configuration files in json format if you open
http://YOUR_ESP_IP/dumpcfg?type=TYPE 
and TYPE can be *mqtt*, *network* or *periph*


*Last Edited on: Sonntag, 20. Mai 2018 01:40*
