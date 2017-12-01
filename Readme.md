# Introduction

ninESP is a modular firmware for ESP8266 SoC based on SmingFramework.
It is designed to work with a few kinds of sensor which you can attach to it
and then it acts as an MQTT Client. So you can easily switch relays or simply as a
relay server for RC Plugs (All RCSwitch compatible Plugs and Protocols) or just a
timer based MQTT publish of sensor values. Also it can start an UDP Server to act/react
for most of the peripherals.

Everything should be accessible and especially configurable from a simple webinterface.
The Webinterface based on the NetworkConfig Example from the SmingFramework.

On first run it starts a Wifi Accesspoint from itself with name "ninHome Configuration"
which you can connect from any Wifi enabled device and then connect with a webbrowser
to "192.168.4.1" to get into the webinterface.
There you can first configure the Wifi it should connect to, the mqtt settings and the 
connected periphals and the corresponding mqtt topics.

The firmware itself should be mostly compatible to the closed source firmware for the
inwall esp8266 which you can buy on ebay.

Any contribution is very warm welcome...

# Screenshots

![wifi networks](https://raw.github.com/ninharp/ninESP/master/doc/screenshots/web_networks_small.jpg "Wifi Network Settings")
![mqtt settings](https://raw.github.com/ninharp/ninESP/master/doc/screenshots/web_mqtt.jpg "MQTT Settings")

# Dependencies

The project relies on SmingFramework for ESP8266, the SmingFramework is as submodule added but not the 
esp-open-sdk, which you had to install also.

It uses a slightly modified version of the SmingFramework MQTTClient Class, cause i needed
some small additions to that.



   