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
#include <app_defaults.h>
#include <MD_Parola.h>

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

	/* Status LED Config */
	int8_t status_led_pin = -1;
	bool status_led_inv = false;

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

	/* Sensor Timer Config */
	uint16_t timer_delay = DEFAULT_SENSOR_TIMER_INTERVAL;

	/* Relay Config */
	bool relay = DEFAULT_RELAY;
	int8_t relay_pin = DEFAULT_RELAY_PIN;
	int8_t relay_status_pin = DEFAULT_RELAY_STATUS_PIN;
	bool relay_status_invert = DEFAULT_RELAY_STATUS_INVERT;
	bool relay_invert = DEFAULT_RELAY_INVERT;
	String relay_topic_cmd = DEFAULT_RELAY_TOPIC_CMD;
	String relay_topic_cmd_old = "";
	String relay_topic_pub = DEFAULT_RELAY_TOPIC_PUB;

	/* Key Input Config */
	bool keyinput = DEFAULT_KEYINPUT;
	bool keyinput_invert = DEFAULT_KEYINPUT_INVERT;
	int8_t keyinput_pin = DEFAULT_KEYINPUT_PIN;
	uint16_t keyinput_debounce = DEFAULT_KEYINPUT_DEBOUNCE_MS;

	/* ADC Config */
	bool adc = DEFAULT_ADC;
	bool adc_pub = DEFAULT_ADC_PUBLISH;
	String adc_topic = DEFAULT_ADC_TOPIC;

	/* RCSwitch Config */
	bool rcswitch = DEFAULT_RCSWITCH;
	//TODO: Prefix for rcswitch in config
	String rcswitch_topic_prefix = DEFAULT_RCSWITCH_TOPIC_PREFIX;
	uint8_t rcswitch_count = 0;
	int8_t rcswitch_pin = DEFAULT_RCSWITCH_PIN;
	Vector<Vector <String>> rcswitch_dev;

	/* MAX7219 Display Config */
	bool max7219 = DEFAULT_MAX7219;
	uint8_t max7219_count = DEFAULT_MAX7219_COUNT;
	int8_t max7219_ss_pin = DEFAULT_MAX7219_SS_PIN;
	String max7219_text = DEFAULT_MAX7219_TEXT;
	uint16_t max7219_speed = DEFAULT_MAX7219_SPEED;
	uint16_t max7219_pause = DEFAULT_MAX7219_SPEED;
	textEffect_t max7219_effect_in = DEFAULT_MAX7219_EFFECT_IN;
	textEffect_t max7219_effect_out = DEFAULT_MAX7219_EFFECT_OUT;
	textPosition_t max7219_alignment = DEFAULT_MAX7219_ALIGNMENT;
	uint8_t max7219_orientation = DEFAULT_MAX7219_ORIENTATION;
	String max7219_topic_prefix = DEFAULT_MAX7219_TOPIC_PREFIX;
	String max7219_topic_enable = DEFAULT_MAX7219_TOPIC_ENABLE;
	String max7219_topic_scroll = DEFAULT_MAX7219_TOPIC_SCROLL;
	String max7219_topic_speed = DEFAULT_MAX7219_TOPIC_SPEED;
	String max7219_topic_charwidth = DEFAULT_MAX7219_TOPIC_CHARWIDTH;
	String max7219_topic_intensity = DEFAULT_MAX7219_TOPIC_INTENSITY;
	String max7219_topic_alignment = DEFAULT_MAX7219_TOPIC_ALIGNMENT;
	String max7219_topic_text = DEFAULT_MAX7219_TOPIC_TEXT;
	String max7219_topic_pause = DEFAULT_MAX7219_TOPIC_PAUSE;
	String max7219_topic_effect_in = DEFAULT_MAX7219_TOPIC_EFFECT_IN;
	String max7219_topic_effect_out = DEFAULT_MAX7219_TOPIC_EFFECT_OUT;

	/* Motion Sensor Config */
	bool motion = DEFAULT_MOTION_SENSOR;
	int8_t motion_pin = DEFAULT_MOTION_PIN;
	bool motion_invert = DEFAULT_MOTION_INVERT;
	uint16_t motion_interval = DEFAULT_MOTION_INTERVAL;
	String motion_topic = DEFAULT_MOTION_TOPIC;

	void loadAll()
	{
		loadNetwork();
		loadMQTT();
		loadPeriph();
	}

	void loadNetwork()
	{
		DynamicJsonBuffer jsonBuffer;
		if (existGlobal())
		{
			int size = fileGetSize(APP_GLOBAL_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_GLOBAL_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& general = root["general"];
			status_led_pin = general["status_pin"];
			status_led_inv = general["status_inv"];

			JsonObject& network = root["network"];
			ssid = network["ssid"].asString();
			password = network["password"].asString();

			dhcp = network["dhcp"];

			ip = network["ip"].asString();
			netmask = network["netmask"].asString();
			gateway = network["gateway"].asString();

			udp = network["udp"];
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

			/* Timer delay setting */
			timer_delay = String(periph["timer_delay"].asString()).toInt();

			/* Relay Settings */
			if (periph.containsKey("relay")) {
				JsonObject& jrelay = periph["relay"];
				relay = jrelay["enabled"];
				relay_pin = String(jrelay["pin"].asString()).toInt();
				relay_invert = jrelay["inverted"];
				relay_status_pin = String(jrelay["status_pin"].asString()).toInt();
				relay_status_invert = jrelay["status_invert"];
				keyinput = jrelay["keyinput"];
				keyinput_invert = jrelay["keyinput_invert"];
				keyinput_pin = String(jrelay["keyinput_pin"].asString()).toInt();
				keyinput_debounce = String(jrelay["keyinput_debounce"].asString()).toInt();
				relay_topic_pub = jrelay["topic_pub"].asString();
				relay_topic_cmd = jrelay["topic_cmd"].asString();
			}

			/* ADC Settings */
			if (periph.containsKey("adc")) {
				JsonObject& jadc = periph["adc"];
				adc = jadc["enabled"];
				adc_pub = jadc["publish"];
				adc_topic = jadc["topic"].asString();
			}

			/* RCSwitch Settings */
			if (periph.containsKey("rcswitch")) {
				JsonObject& rcs = periph["rcswitch"];
				rcswitch = rcs["enabled"];
				rcswitch_topic_prefix = rcs["topic"].asString();
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
			}

			/* MAX7219 Display Settings */
			if (periph.containsKey("max7219")) {
				JsonObject& jmax7219 = periph["max7219"];
				max7219 = jmax7219["enabled"];
				max7219_count = String(jmax7219["count"].asString()).toInt();
				max7219_ss_pin = String(jmax7219["pin"].asString()).toInt();
				max7219_topic_prefix = jmax7219["topic_prefix"].asString();
				max7219_topic_enable = jmax7219["topic_enable"].asString();
				max7219_topic_text = jmax7219["topic_text"].asString();
				max7219_topic_scroll = jmax7219["topic_scroll"].asString();
				max7219_topic_speed = jmax7219["topic_speed"].asString();
				max7219_topic_charwidth = jmax7219["topic_charwidth"].asString();
				max7219_topic_intensity = jmax7219["topic_intensity"].asString();
			}
			/* uint8_t max7219_orientation = DEFAULT_MAX7219_ORIENTATION; */

			/* Motion Sensor Settings */
			if (periph.containsKey("motion")) {
				JsonObject& jmotion = periph["motion"];
				motion = jmotion["enabled"];
				motion_invert = jmotion["inverted"];
				motion_pin = String(jmotion["pin"].asString()).toInt();
				motion_interval = String(jmotion["interval"].asString()).toInt();
				motion_topic = jmotion["topic"].asString();
			}

			delete[] jsonString;
		  }
	}

	void saveNetwork()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();

		JsonObject& general = jsonBuffer.createObject();
		root["general"] = general;
		general["status_pin"] = status_led_pin;
		general["status_inv"] = status_led_inv;

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
		JsonObject& jmax7219 = jsonBuffer.createObject();
		JsonObject& jmotion = jsonBuffer.createObject();

		root["peripherals"] = periph;

		periph["timer_delay"] = timer_delay;

		jrelay["enabled"] = relay;
		jrelay["pin"] = relay_pin;
		jrelay["inverted"] = relay_invert;
		jrelay["status_pin"] = relay_status_pin;
		jrelay["status_invert"] = relay_status_invert;
		jrelay["keyinput"] = keyinput;
		jrelay["keyinput_invert"] = keyinput_invert;
		jrelay["keyinput_pin"] = keyinput_pin;
		jrelay["keyinput_debounce"] = keyinput_debounce;
		jrelay["topic_pub"] = relay_topic_pub;
		jrelay["topic_cmd"] = relay_topic_cmd;
		periph["relay"] = jrelay;

		jadc["enabled"] = adc;
		jadc["publish"] = adc_pub;
		jadc["topic"] = adc_topic;
		periph["adc"] = jadc;

		rcs["enabled"] = rcswitch;
		rcs["pin"] = rcswitch_pin;
		rcs["topic"] = rcswitch_topic_prefix;
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

		jmax7219["enabled"] = max7219;
		jmax7219["count"] = max7219_count;
		jmax7219["pin"] = max7219_ss_pin;
		jmax7219["topic_prefix"] = max7219_topic_prefix;
		jmax7219["topic_enable"] = max7219_topic_enable;
		jmax7219["topic_text"] = max7219_topic_text;
		jmax7219["topic_scroll"] = max7219_topic_scroll;
		jmax7219["topic_speed"] = max7219_topic_speed;
		jmax7219["topic_charwidth"] = max7219_topic_charwidth;
		jmax7219["topic_intensity"] = max7219_topic_intensity;
		periph["max7219"] = jmax7219;

		jmotion["enabled"] = motion;
		jmotion["inverted"] = motion_invert;
		jmotion["pin"] = motion_pin;
		jmotion["interval"] = motion_interval;
		jmotion["topic"] = motion_topic;
		periph["motion"] = jmotion;

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
