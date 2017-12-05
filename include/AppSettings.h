/*
 * AppSettings.h
 *
 *  Created on: 13 ��� 2015 �.
 *      Author: Anakod
 */

#include <SmingCore/SmingCore.h>

#ifndef INCLUDE_APPSETTINGS_H_
#define INCLUDE_APPSETTINGS_H_

#define APP_GLOBAL_SETTINGS_FILE ".settings.conf" // leading point for security reasons :)
#define APP_PERIPH_SETTINGS_FILE ".periph_settings.conf"
#define APP_MQTT_SETTINGS_FILE ".mqtt_settings.conf"

struct ApplicationSettingsStorage
{
	/* Global Config */
	// IP Config
	IPAddress ip;
	IPAddress netmask;
	IPAddress gateway;
	String ssid;
	String password;
	bool dhcp = true;

	bool udp = false;
	String udp_port;

	/* MQTT Config */
	// General MQTT Config
	String mqtt_server;
	String mqtt_port;
	String mqtt_userid;
	String mqtt_login;
	String mqtt_password;
	String mqtt_topic_lwt;
	String mqtt_topic_cmd;
	String mqtt_topic_pub;

	/* Periph Config */
	// Peripheral Config
	uint16_t timer_delay = 5000;

	bool relay = false;
	String relay_topic_cmd; //temp_adc_pub
	String relay_topic_cmd_old;
	String relay_topic_pub;

	bool adc = false;
	bool adc_pub = false;
	String adc_topic; //topic_adc_pub

	bool temp_dht11 = false;
	bool temp_dht11_pub = false;
	String temp_dht11_temp_topic; //topic_dht11temp_pub
	String temp_dht11_humi_topic; //topic_dht11humi_pub

	bool temp_ds18b20 = false;
	bool temp_ds18b20_pub = false;
	String temp_ds18b20_topic; //topic_ds18b20_pub

	bool temp_lm75 = false;
	bool temp_lm75_pub = false;
	String temp_lm75_topic; //topic_lm75_pub

	bool rcswitch = false;
	String rcswitch_topic;
	uint8_t rcswitch_count = 0;
	int8_t rcswitch_pin = -1;
	Vector<Vector <String>> rcswitch_dev;

	void loadAll()
	{
		loadGlobal();
		loadMQTT();
		loadPeriph();
	}

	void loadGlobal()
	{
		DynamicJsonBuffer jsonBuffer;
		if (existGlobal())
		{
			int size = fileGetSize(APP_GLOBAL_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_GLOBAL_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& network = root["network"];
			ssid = network["ssid"].asString();
			password = network["password"].asString();

			dhcp = network["dhcp"];

			ip = network["ip"].asString();
			netmask = network["netmask"].asString();
			gateway = network["gateway"].asString();

			udp = network["udp"].asString();
			udp_port = network["udp_port"].asString();

			delete[] jsonString;
		}
	}

	void loadMQTT()
	{
		DynamicJsonBuffer jsonBuffer;
		if (existMQTT())
		{
			int size = fileGetSize(APP_MQTT_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_MQTT_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& mqtt = root["mqtt"];
			mqtt_server = mqtt["server"].asString();
			mqtt_port = mqtt["port"].asString();
			mqtt_userid = mqtt["userid"].asString();
			mqtt_login = mqtt["login"].asString();
			mqtt_password = mqtt["password"].asString();
			mqtt_topic_lwt = mqtt["lwt"].asString();
			mqtt_topic_cmd = mqtt["topic_cmd"].asString();
			mqtt_topic_pub = mqtt["topic_pub"].asString();

			delete[] jsonString;
		}
	}

	void loadPeriph()
	{
		DynamicJsonBuffer jsonBuffer;
		//if (existPeriph())
		//{
			int size = fileGetSize(APP_PERIPH_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_PERIPH_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& periph = root["peripherals"];
			timer_delay = atoi(periph["timer_delay"].asString());

			relay = periph["relay"];
			adc = periph["adc"];
			temp_dht11 = periph["temp_dht11"];
			temp_ds18b20 = periph["temp_ds18b20"];
			temp_lm75 = periph["temp_lm75"];

			adc_pub = periph["adc_pub"];
			temp_dht11_pub = periph["temp_dht11_pub"];
			temp_ds18b20_pub = periph["temp_ds18b20_pub"];
			temp_lm75_pub = periph["temp_lm75_pub"];

			relay_topic_pub = periph["relay_topic_pub"].asString();
			relay_topic_cmd = periph["relay_topic_cmd"].asString();
			adc_topic = periph["adc_topic"].asString();
			temp_dht11_temp_topic = periph["temp_dht11_temp_topic"].asString();
			temp_dht11_humi_topic = periph["temp_dht11_humi_topic"].asString();
			temp_ds18b20_topic = periph["temp_ds18b20_topic"].asString();
			temp_lm75_topic = periph["temp_lm75_topic"].asString();

			JsonObject& rcs = periph["rcswitch"];
			rcswitch = rcs["enabled"];
			rcswitch_topic = rcs["topic"].asString();
			String tmp = rcs["count"].asString();
			if (tmp.length() >= 1)
				rcswitch_count = atoi(tmp.c_str());

			if (rcswitch_count > 0) {
				JsonObject& rcdev = rcs["devices"];
				for (int i = 0; i < rcswitch_count; i++) {
					JsonObject& rcdev_item = rcdev[String(i)];
					Vector<String> item;
					item.addElement(rcdev_item["0"].asString());
					item.addElement(rcdev_item["1"].asString());
					rcswitch_dev.add(item);
					//rcswitch_dev[i][0] = rcdev_item["0"].asString();
					//rcswitch_dev[i][1] = rcdev_item["1"].asString();
				}
			}

			delete[] jsonString;

			debugf("rcswitch_topic = %s", rcswitch_topic.c_str());
			debugf("rcswitch_count = %d", rcswitch_count);
			for (int i = 0; i < rcswitch_count; i++) {
				debugf("rcswitch_dev[%d] = %s-%s", i, rcswitch_dev[i][0].c_str(), rcswitch_dev[i][1].c_str());
			}
	}

	void saveGlobal()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();

		JsonObject& network = jsonBuffer.createObject();
		root["network"] = network;
		network["ssid"] = ssid.c_str();
		network["password"] = password.c_str();

		network["dhcp"] = dhcp;

		// Make copy by value for temporary string objects
		network["ip"] = ip.toString();
		network["netmask"] = netmask.toString();
		network["gateway"] = gateway.toString();

		network["udp"] = udp;
		network["udp_port"] = udp_port;

		String rootString;
		root.printTo(rootString);
		fileSetContent(APP_GLOBAL_SETTINGS_FILE, rootString);
	}

	void saveMQTT()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();

		JsonObject& mqtt = jsonBuffer.createObject();
		root["mqtt"] = mqtt;
		mqtt["server"] = mqtt_server;
		mqtt["port"] = mqtt_port;
		mqtt["userid"] = mqtt_userid;
		mqtt["login"] = mqtt_login;
		mqtt["password"] = mqtt_password;
		mqtt["lwt"] = mqtt_topic_lwt;
		mqtt["topic_cmd"] = mqtt_topic_cmd;
		mqtt["topic_pub"] = mqtt_topic_pub;

		String rootString;
		root.printTo(rootString);
		fileSetContent(APP_MQTT_SETTINGS_FILE, rootString);
	}

	void savePeriph()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();
		JsonObject& periph = jsonBuffer.createObject();
		JsonObject& rcs = jsonBuffer.createObject();
		root["peripherals"] = periph;

		periph["timer_delay"] = timer_delay;

		periph["relay"] = relay;
		periph["adc"] = adc;
		periph["temp_dht11"] = temp_dht11;
		periph["temp_ds18b20"] = temp_ds18b20;
		periph["temp_lm75"] = temp_lm75;

		periph["rcswitch"] = rcs;
		rcs["enabled"] = rcswitch;
		rcs["pin"] = rcswitch_pin;
		rcs["topic"] = rcswitch_topic;
		rcs["count"] = rcswitch_count;
		if (rcswitch_count > 0) {
			JsonObject& rc_devs = jsonBuffer.createObject();
			rcs["devices"] = rc_devs;
			for (int i = 0; i < rcswitch_count; i++) {
				JsonObject& rc_dev = jsonBuffer.createObject();
				rc_devs[String(i)] = rc_dev;
				rc_dev["0"] = rcswitch_dev[i][0];
				rc_dev["1"] = rcswitch_dev[i][1];
			}
		}

		periph["adc_pub"] = adc_pub;
		periph["temp_dht11_pub"] = temp_dht11_pub;
		periph["temp_ds18b20_pub"] = temp_ds18b20_pub;
		periph["temp_lm75_pub"] = temp_lm75_pub;

		periph["relay_topic_cmd"] = relay_topic_cmd;
		periph["relay_topic_pub"] = relay_topic_pub;
		periph["adc_topic"] = adc_topic;
		periph["temp_dht11_temp_topic"] = temp_dht11_temp_topic;
		periph["temp_dht11_humi_topic"] = temp_dht11_humi_topic;
		periph["temp_ds18b20_topic"] = temp_ds18b20_topic;
		periph["temp_lm75_topic"] = temp_lm75_topic;

		periph["rcswitch_topic"] = rcswitch_topic;
		periph["rcswitch_count"] = rcswitch_count;
		JsonObject& rc_devs = jsonBuffer.createObject();
		periph["rcswitch_dev"] = rc_devs;
		for (int i = 0; i < rcswitch_count; i++) {
			JsonObject& rc_dev = jsonBuffer.createObject();
			rc_devs[String(i)] = rc_dev;
			rc_dev["0"] = rcswitch_dev[i][0];
			rc_dev["1"] = rcswitch_dev[i][1];
		}

		String rootString;
		root.printTo(rootString);
		fileSetContent(APP_PERIPH_SETTINGS_FILE, rootString);
	}

	bool existGlobal() { return fileExist(APP_GLOBAL_SETTINGS_FILE); }
	bool existPeriph() { return fileExist(APP_PERIPH_SETTINGS_FILE); }
	bool existMQTT() { return fileExist(APP_MQTT_SETTINGS_FILE); }
};

static ApplicationSettingsStorage AppSettings;

#endif /* INCLUDE_APPSETTINGS_H_ */
