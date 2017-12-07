/**
 * Project: ninHOME_Node
 * @file AppSettings.h
 * @author Anakod, Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * This file includes all config file dependent functions
 *
 */

#include <SmingCore/SmingCore.h>

#ifndef INCLUDE_APPSETTINGS_H_
#define INCLUDE_APPSETTINGS_H_

/* Config file definitions, with leading point for security reasons */
#define APP_GLOBAL_SETTINGS_FILE ".settings.conf"
#define APP_PERIPH_SETTINGS_FILE ".periph_settings.conf"
#define APP_MQTT_SETTINGS_FILE ".mqtt_settings.conf"

struct ApplicationSettingsStorage
{
	/* Network Config */
	IPAddress ip;
	IPAddress netmask;
	IPAddress gateway;
	String ssid;
	String password;
	bool dhcp = true;

	bool udp = DEFAULT_UDP;
	uint16_t udp_port = DEFAULT_UDP_PORT;

	/* MQTT Config */
	bool mqtt_enabled = false;
	String mqtt_server = DEFAULT_MQTT_SERVER;
	uint16_t mqtt_port = DEFAULT_MQTT_PORT;
	String mqtt_userid = DEFAULT_MQTT_USERID;
	String mqtt_login = DEFAULT_MQTT_LOGIN;
	String mqtt_password = DEFAULT_MQTT_PASS;
	String mqtt_topic_lwt = DEFAULT_MQTT_LWT;
	String mqtt_topic_cmd = DEFAULT_MQTT_CMD;
	String mqtt_topic_pub = DEFAULT_MQTT_PUB;

	/* Periph Config */
	uint16_t timer_delay = DEFAULT_SENSOR_TIMER_INTERVAL;

	bool relay = DEFAULT_RELAY;
	int8_t relay_pin = DEFAULT_RELAY_PIN;
	String relay_topic_cmd = DEFAULT_RELAY_TOPIC_CMD;
	String relay_topic_cmd_old = "";
	String relay_topic_pub = DEFAULT_RELAY_TOPIC_PUB;
	bool keyinput = DEFAULT_KEYINPUT;
	bool keyinput_invert = DEFAULT_KEYINPUT_INVERT;
	int8_t keyinput_pin = DEFAULT_KEYINPUT_PIN;

	bool adc = DEFAULT_ADC;
	bool adc_pub = DEFAULT_ADC_PUBLISH;
	String adc_topic = DEFAULT_ADC_TOPIC;

	bool rcswitch = DEFAULT_RCSWITCH;
	String rcswitch_topic = DEFAULT_RCSWITCH_TOPIC;
	uint8_t rcswitch_count = 0;
	int8_t rcswitch_pin = DEFAULT_RCSWITCH_PIN;
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
			udp_port = String(network["udp_port"].asString()).toInt();

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
			mqtt_enabled = mqtt["enabled"];
			mqtt_server = mqtt["server"].asString();
			mqtt_port = String(mqtt["port"].asString()).toInt();
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
		if (existPeriph())
		  {
			int size = fileGetSize(APP_PERIPH_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_PERIPH_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& periph = root["peripherals"];

			timer_delay = String(periph["timer_delay"].asString()).toInt();

			/* Relay Settings */
			JsonObject& jrelay = periph["relay"];
			relay = jrelay["enabled"];
			relay_pin = String(jrelay["pin"].asString()).toInt();
			keyinput = jrelay["keyinput"];
			keyinput_invert = jrelay["invert"];
			keyinput_pin = String(jrelay["pin"].asString()).toInt();
			relay_topic_pub = jrelay["topic_pub"].asString();
			relay_topic_cmd = jrelay["topic_cmd"].asString();

			/* ADC Settings */
			JsonObject& jadc = periph["adc"];
			adc = jadc["enabled"];
			adc_pub = jadc["publish"];
			adc_topic = jadc["topic"].asString();

			/* RCSwitch Settings */
			JsonObject& rcs = periph["rcswitch"];
			rcswitch = rcs["enabled"];
			rcswitch_topic = rcs["topic"].asString();
			String tmp = rcs["count"].asString();
			if (tmp.length() >= 1)
				rcswitch_count = tmp.toInt();

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

			/* MAX7219 Display Settings */
			//TODO MAX7219 integration

			delete[] jsonString;

			debugf("rcswitch_topic = %s", rcswitch_topic.c_str());
			debugf("rcswitch_count = %d", rcswitch_count);
			for (int i = 0; i < rcswitch_count; i++) {
				debugf("rcswitch_dev[%d] = %s-%s", i, rcswitch_dev[i][0].c_str(), rcswitch_dev[i][1].c_str());
			}
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
		mqtt["enabled"] = mqtt_enabled;
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
		JsonObject& jrelay = jsonBuffer.createObject();
		JsonObject& jadc = jsonBuffer.createObject();

		root["peripherals"] = periph;

		periph["timer_delay"] = timer_delay;

		jrelay["enabled"] = relay;
		jrelay["pin"] = relay_pin;
		jrelay["keyinput"] = keyinput;
		jrelay["keyinput_invert"] = keyinput_invert;
		jrelay["keyinput_pin"] = keyinput_pin;
		jrelay["topic_pub"] = relay_topic_pub;
		jrelay["topic_cmd"] = relay_topic_cmd;
		periph["relay"] = jrelay;

		jadc["enabled"] = adc;
		jadc["publish"] = adc_pub;
		jadc["topic"] = adc_topic;
		periph["adc"] = jadc;

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
		periph["rcswitch"] = rcs;

		JsonObject& rc_devs = jsonBuffer.createObject();
		rcs["devices"] = rc_devs;
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
