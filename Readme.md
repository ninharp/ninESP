# Introduction

ninESP is a modular firmware for ESP8266 SoC based on SmingFramework.
It is designed to work with a few kinds of sensor which you can attach to it
and then it acts as an MQTT Client. So you can easily switch relays or simply as a
relay server for RC Plugs (All RCSwitch compatible Plugs and Protocols) or just a
timer based MQTT publish of sensor values. Also it can start an UDP Server to act/react
for most of the peripherals.

Everything should be accessible and especially configurable from a simple webinterface.
The Webinterface based on the NetworkConfig Example of the SmingFramework.

On first run it starts a Wifi Accesspoint from itself with name "ninHome Configuration"
which you can connect from any Wifi enabled device and then connect with a webbrowser
to "http://192.168.4.1" to get into the webinterface.
There you can first configure the Wifi it should connect to, the mqtt settings and the 
connected periphals and the corresponding mqtt topics.

In Version 0.2 the library for the MAX7219 control changed to a more modern and flexible
library which can do a lot of more animations. 
MD_MAX72xx and MD_Parola which are from [MajicDesigns](https://github.com/MajicDesigns) 
Since this version (0.2) your flash size has to be at least a 1M type. 

The firmware itself should be mostly compatible to the closed source firmware for the
inwall esp8266 which you can buy on ebay.

You can find the latest firmware version in a binary format in the [firmware.tar.gz](https://github.com/ninharp/ninESP/blob/master/firmware.tar.gz?raw=true)  file in the root of this repository

Any contribution is very warm welcome...

# Screenshots

![wifi networks](https://raw.github.com/ninharp/ninESP/master/doc/screenshots/web_networks_small.jpg "Wifi Network Settings")
![mqtt settings](https://raw.github.com/ninharp/ninESP/master/doc/screenshots/web_mqtt.jpg "MQTT Settings")

# Peripheral Modules

In this list you see the peripheral modules which are currently build in or in development state

# Known Working Boards/Chips

Here you can find a list of development boards and chips which are known to work with ninESP firmware.

# Dependencies

The project relies on SmingFramework for ESP8266, the SmingFramework is as submodule added but not the 
[esp-open-sdk](https://github.com/pfalcon/esp-open-sdk) , which you had to install also and by yourself.
As other submodules are the libraries of MajicDesigns included.
[MD_MAX72xx](https://github.com/MajicDesigns/MD_MAX72XX) - MAX72xx Driver Library
[MD_Parola](https://github.com/MajicDesigns/MD_Parola) - Animation Library for MAX72XX

~~It uses a slightly modified version of the SmingFramework MQTTClient Class, cause i needed
some small additions to that.~~



Last Edited on: Samstag, 19. Mai 2018 01:24 

   