/*
 * application.h
 *
 *  Created on: 21.12.2017
 *      Author: michael
 */

#ifndef APP_APPLICATION_H_
#define APP_APPLICATION_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <AppSettings.h>
#include <Libraries/RCSwitch/RCSwitch.h>
#include <RelaySwitch.h>
#include <ninMQTTClient.h>
#include <LedMatrix.h>
#include <app_defaults.h>

void connectOk(IPAddress ip, IPAddress mask, IPAddress gateway);
void connectFail(String ssid, uint8_t ssidLength, uint8_t *bssid, uint8_t reason);
void onMQTTMessageReceived(String topic, String message);
void startServices();

const String app_version = String(VER_MAJOR) + "." +String(VER_MINOR) + " build " + String(VER_BUILD);

#endif /* APP_APPLICATION_H_ */
