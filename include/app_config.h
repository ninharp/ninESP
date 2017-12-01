/*
 * app_config.h
 *
 *  Created on: 30.11.2017
 *      Author: michael
 */

#ifndef INCLUDE_APP_CONFIG_H_
#define INCLUDE_APP_CONFIG_H_

#define NINHOME_AP_NAME "ninHome Configuration"

#define LED_SETUP_PIN 5
#define RESET_PIN 13

#define DEFAULT_MQTT_SERVER "test.mosquitto.org"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_LWT "lwt/client/id"
#define DEFAULT_MQTT_CMD "lwt/#"
#define DEFAULT_MQTT_PUB "default/topic"
#define DEFAULT_MQTT_USERID "ninHOME_Client"
#define DEFAULT_MQTT_LOGIN "user"
#define DEFAULT_MQTT_PASS "password"

#define DEFAULT_FTP_USER "michael"
#define DEFAULT_FTP_PASS "123"
#define DEFAULT_FTP_PORT 21

#define DEFAULT_RELAY_PIN 5

#define RCSWITCH_TX_PIN 4


#define LWT_TOPIC "lwt/client/id5"
#define SUBSCRIBE_TOPIC "lwt/#"
//#define PUBLISH_TOPIC ""



#endif /* INCLUDE_APP_CONFIG_H_ */
