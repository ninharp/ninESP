/**
 * Project: ninHOME_Node
 * @file app_config.h
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * This file includes all default settings
 *
 */


#ifndef INCLUDE_APP_CONFIG_H_
#define INCLUDE_APP_CONFIG_H_

/* Default Wifi Accesspoint Name for Setup */
#define NINHOME_AP_NAME "ninHome Configuration"

/* mDNS Settings */
#define DEFAULT_MDNS_HOSTNAME "ninhome"
#define DEFAULT_MDNS_SERVERNAME "ninHOME"
#define DEFAULT_MDNS_SERVERPORT 80

#define LED_SETUP_PIN 15 // 15/D8  // Display = 0 / Main = 15
//#define RESET_PIN 13 // 13/D7  // Display = 13 / Main = 4

/* Relay Settings */
#define DEFAULT_RELAY false
#define DEFAULT_RELAY_PIN 14 //D5
#define DEFAULT_RELAY_TOPIC_CMD "client/relay"
#define DEFAULT_RELAY_TOPIC_PUB "client/relaystate"

/* Key debouncing intervall (100ms) */
#define DEFAULT_KEYINPUT false
#define DEFAULT_KEYINPUT_PIN 5 //D1
#define DEFAULT_KEY_POLL_INTERVAL 100
#define DEFAULT_KEYINPUT_INVERT false

/* Default Sensor Timer Interval (30sec) */
#define DEFAULT_SENSOR_TIMER_INTERVAL 30000

/* ADC Settings */
#define DEFAULT_ADC false
#define DEFAULT_ADC_PUBLISH false
#define DEFAULT_ADC_TOPIC "client/adc"

/* MQTT Settings */
#define DEFAULT_MQTT_SERVER "test.mosquitto.org"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_LWT "lwt/client/id"
#define DEFAULT_MQTT_CMD "lwt/#"
#define DEFAULT_MQTT_PUB "default/topic"
#define DEFAULT_MQTT_USERID "ninHOME_Client"
#define DEFAULT_MQTT_LOGIN "user"
#define DEFAULT_MQTT_PASS "password"

/* FTP Settings */
#define DEFAULT_FTP_USER "michael"
#define DEFAULT_FTP_PASS "123"
#define DEFAULT_FTP_PORT 21

/* UDP Settings */
#define DEFAULT_UDP true
#define DEFAULT_UDP_PORT 1234

/* RCSwitch Settings */
#define DEFAULT_RCSWITCH false
#define DEFAULT_RCSWITCH_PIN 4 //D2
#define DEFAULT_RCSWITCH_TOPIC "client/rcswitch"

#endif /* INCLUDE_APP_CONFIG_H_ */
