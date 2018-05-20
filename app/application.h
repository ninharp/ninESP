/**
 * Project: ninHOME_Node
 * @file application.cpp
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * This is the main header file which holds the global variables and includes
 *
 */

#ifndef APP_APPLICATION_H_
#define APP_APPLICATION_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <AppSettings.h>
#include <Libraries/RCSwitch/RCSwitch.h>
#include <Libraries/LiquidCrystal/LiquidCrystal_I2C.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <RelaySwitch.h>
#include <SmingCore/Network/MqttClient.h>
#include <app_defaults.h>
#include <sprites.h>

void connectOk(IPAddress ip, IPAddress mask, IPAddress gateway);
void connectFail(String ssid, uint8_t ssidLength, uint8_t *bssid, uint8_t reason);
void onMQTTMessageReceived(String topic, String message);
void startServices();

const String app_version = String(VER_MAJOR) + "." +String(VER_MINOR) + " build " + String(VER_BUILD);

template<typename T>
struct DefferedObject
{
    DefferedObject(){}
    ~DefferedObject(){ value.~T(); }
    template<typename...TArgs>
    void Construct(TArgs&&...args)
    {
        new (&value) T(std::forward<TArgs>(args)...);
    }
public:
    union
    {
        T value;
    };
};

#endif /* APP_APPLICATION_H_ */
