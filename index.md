---
title: home
---

# What is ninESP?

ninESP is a easy to use modular firmware for ESP8266 SoCs.

It work with different kinds of sensors and actuators which you can attach to it.
Then it acts as an MQTT Client which serves you telemetry data or let you control things. 
So you can easily switch relays, use it as a relay server for RC Plugs 
(All RCSwitch compatible Plugs and Protocols), timer based MQTT publish of sensor values or simply as a wifi LED dot matrix display.
 
You can find all currently supported sensors/actuators in the "Peripheral Modules" section.

All these sensors/actuators and other individual settings are easy to reach on the internal webinterface so there is no need to reflash the whole firmware. And if a big update occurs you can easily update the firmware over the webinterface aswell (*not fully implemented*)

It also serves you a serial terminal and also and an UDP server
from where you can interact with your connected peripherals.
Protocol description of UDP and serial commands in the "Protocols" section.

The UDP protocol is mostly compatible to the [esp8266 inwall system](https://ex-store.de/ESP8266-WiFi-Relay-V31)  which are sold at exs. So there proprietary apps can also be used.

In Version 0.2 the library for the MAX7219 control changed to a more modern and flexible library which can do a lot of more animations.
 
Now it uses MD_MAX72xx and MD_Parola which are from [MajicDesigns](https://github.com/MajicDesigns) 
Since this version (0.2) your flash size has to be at least a 1M type. 

You can find the latest firmware version in a binary format in the firmware folder in the root of this repository