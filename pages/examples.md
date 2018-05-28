---
title: Examples
tag: examples
---
# Examples

## Examples with mosquitto client:

### MAX72XX Module:

In the examples we use a topic prefix of "client/display/"

Send a new text "Hello World" to Display with topic "client/display/text"
```
mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/display/text -m "Hello World"
```

Enable inverting of the Display with topic "client/display/invert"
```
mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/display/invert -m 1
```

### Relay Module:

Switch relay to ON state on topic "client/relay"
```
mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/relay -m 1
```

### RCSwitch Module:

In the examples we use a topic prefix of "client/rcswitch/"

Switch Plug 1 to OFF state on topic "client/rcswitch/1"
```
mosquitto_pub -h YOURMQTTBROKER -u MQTTUSER -P MQTTPASS -r -t client/rcswitch/1 -m 0
```


You can use any mqtt client to interact with ninESP, for example an mobile android mqtt client is 
[MQTT Dash](https://play.google.com/store/apps/details?id=net.routix.mqttdash)
