/**
 * Project: ninHOME_Node
 * @file application.cpp
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * This is the main application file
 *
 */
#include <application.h>
#include <webinterface.h>
//#include <ota.h>

class ninHOME;

extern BssList wNetworks;
extern String lastModified;

/* Forward declarations */
void startMqttClient();
void onReceiveUDP(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort);
void statusLed(bool state);
void motionSensorCheck();
void checkMQTTConnection(TcpClient& client, bool flag);
int splitString(String &what, int delim,  String *splits);

/* LCD Display instance */
LiquidCrystal_I2C *lcd; //(0x27);

/* MQTT client instance */
MqttClient mqtt(true, false);

/* MD Parola/MAX72xx LED Matrix instance */
MD_Parola *led;

/* UDP Connection instance */
UdpConnection udp = UdpConnection(onReceiveUDP);

/* RelaySwitch and RCSwitch instance */
RCSwitch rcSwitch = RCSwitch();
RelaySwitch relay = RelaySwitch();

/* Timer for MAX7219 LED Matrix */
//bool displayScroll = true;
//bool displayEnable = true;
//bool displayAnim = true;
//bool displayReset = true;
Timer ledMatrixTimer;

/* Timer to check for connection */
Timer checkConnectionTimer;

/* Timer for publishing sensor values */
Timer sensorPublishTimer;

/* Timer to check for motion on motion sensor */
Timer motionCheckTimer;
bool motionState = false;

Timer debounceTimer;
bool key_pressed = false;
long lastKeyPress = 0;
Timer relayTimer;

DefferedObject<MD_Parola> ledPtr;
DefferedObject<LiquidCrystal_I2C> lcdPtr;

/* IRQ Callback for interrupt of key input */
void IRAM_ATTR keyIRQHandler()
{
	if (AppSettings.relay && AppSettings.keyinput) {
		noInterrupts();
		if (AppSettings.keyinput_invert) {
			if (digitalRead(AppSettings.keyinput_pin) == LOW)
				key_pressed = true;
			else
				key_pressed = false;
		} else {
			if (digitalRead(AppSettings.keyinput_pin) == HIGH)
				key_pressed = true;
			else
				key_pressed = false;
		}
		//interrupts();
	}
}

void serialShowInfo() {
    Serial.printf("\r\nSDK: v%s\r\n", system_get_sdk_version());
    Serial.printf("Free Heap: %d\r\n", system_get_free_heap_size());
    Serial.printf("CPU Frequency: %d MHz\r\n", system_get_cpu_freq());
    Serial.printf("System Chip ID: %x\r\n", system_get_chip_id());
    Serial.printf("SPI Flash ID: %x\r\n", spi_flash_get_id());
    Serial.printf("SPI Flash Size: %d\r\n", (1 << ((spi_flash_get_id() >> 16) & 0xff)));
}

void relayTimerCb()
{
	relay.set(false);
	mqtt.publish(AppSettings.relay_topic_pub, "0", true);
}

/* Callback for UDP Receiving */
void onReceiveUDP(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort)
{
	//debugf("UDP Sever callback from %s:%d, %d bytes", remoteIP.toString().c_str(), remotePort, size);

	if (data[0] == '3' && data[1] == ';' && size == 4) {
		if (AppSettings.relay) {
			//debugf("UDP Relay %c", data[2]);
			uint8_t state = String(data[2]).toInt();
			relay.set(state);
			mqtt.publish(AppSettings.relay_topic_pub, String(data[2]), true);
			String ret = WifiStation.getMAC() + "," + data[0] + "," + WifiStation.getIP().toString() + "," + data[0] + "," + data[2];
			udp.sendStringTo(remoteIP, remotePort, ret);
			remoteIP[3] = 255;
			ret += "," + AppSettings.mqtt_userid;
			//debugf("%s %s", ret.c_str(), remoteIP.toString().c_str());
			udp.sendStringTo(remoteIP, remotePort, ret);
			//TODO bcast answer
		}
	}
	else if (data[0] == '9' && data[1] == ';') {
		String ms = "0";
		for (uint8_t i = 2; i < size-1; i++) {
			ms += data[i];
		}
		if (ms.toInt() > 1000) { /* at least 1 sec for delay mode */
			if (AppSettings.relay) {
				relay.set(true);
				relayTimer.initializeMs(ms.toInt(), relayTimerCb).startOnce();
				//debugf("UDP Relay Timer %d ms", ms.toInt());
				mqtt.publish(AppSettings.relay_topic_pub, "0", true);
				String ret = WifiStation.getMAC() + "," + data[0] + "," + WifiStation.getIP().toString() + "," + data[0] + "," + ms.toInt();
				udp.sendStringTo(remoteIP, remotePort, ret);
			}
		}
	}
	// Send echo to remote sender
	//String text = String("echo: ") + data + "[" + size + "]";
	//udp.sendStringTo(remoteIP, remotePort, text);
}

/* Callback for messages, arrived from MQTT server */
void onMQTTMessageReceived(String topic, String message)
{
	//debugf("Debug: %s - %s", topic.c_str(), message.c_str());

	/* If Relay is enabled and topic equals the topic from config then check payload */
	if (AppSettings.relay && AppSettings.relay_topic_cmd.equals(topic)) {
		//debugf("Relay Command received!");
		/* Check payload for valid message */
		if (message.equals("on") || message.equals("1")) { // Relay ON
			/* Set relay to on */
			relay.set(true);
			/* Check mqtt connection status */
			if (mqtt.getConnectionState() != eTCS_Connected)
				startMqttClient(); // Auto reconnect
			/* Publish current relay state */
			mqtt.publish(AppSettings.relay_topic_pub, "1", true);
		} else if (message.equals("off") || message.equals("0")) { // Relay OFF
			/* Set relay to off */
			relay.set(false);
			/* Check mqtt connection status */
			if (mqtt.getConnectionState() != eTCS_Connected)
				startMqttClient(); // Auto reconnect
			/* Publish current relay state */
			mqtt.publish(AppSettings.relay_topic_pub, "0", true);
		}
	}

	/* RC Switch relay */
	if (AppSettings.rcswitch && (AppSettings.rcswitch_count > 0)) {
		for (uint8_t c = 0; c < AppSettings.rcswitch_count; c++) {
			Vector<String> dev = AppSettings.rcswitch_dev.get(c);
			String m0 = dev.get(0);
			String m1 = dev.get(1);
			if (topic.equals(AppSettings.rcswitch_topic_prefix + String(c))) {
				if (message.equals("on") || message.equals("1")) { // Relay ON
					if (m0.length() == 5)
						rcSwitch.switchOn(m0.c_str(), m1.c_str());
					else
						rcSwitch.switchOn((int)m0.toInt(), (int)m1.toInt());
				} else if (message.equals("off") || message.equals("0")) { // Relay OFF
					if (m0.length() == 5)
						rcSwitch.switchOff(m0.c_str(), m1.c_str());
					else
						rcSwitch.switchOff((int)m0.toInt(), (int)m1.toInt());
				}
			}
		}
	}

	/* MAX7219 Display */
	if (AppSettings.max7219 && topic.startsWith(AppSettings.max7219_topic_prefix)) {
		message.replace(AppSettings.max7219_topic_prefix, "display");
		String piecesStr[5];
		uint8_t pieces = splitString(topic, '/', piecesStr);

		if (pieces > 3) {
			String cmdtopic = piecesStr[3];
			uint8_t zone_num = piecesStr[2].toInt();
			//Serial.printf("Topic: %d Pieces\n", pieces);
			//for (uint8_t z = 0; z < pieces; z++)
			//	Serial.printf("Piece: %s\n", piecesStr[z].c_str());
			if (cmdtopic.equals(AppSettings.max7219_topic_enable)) {
				AppSettings.max7219_display[zone_num].enabled = message.toInt();
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_text)) {
				AppSettings.max7219_display[zone_num].text = message;
				AppSettings.max7219_display[zone_num].anim = true;
				led->displayReset(zone_num);
				led->setTextBuffer(zone_num, (char*)AppSettings.max7219_display[zone_num].text.c_str());
				//led->displayText((char*)AppSettings.max7219_text.c_str(), AppSettings.max7219_alignment, led->getSpeed(), led->getPause(), AppSettings.max7219_effect_in, AppSettings.max7219_effect_out);
				Serial.printf("Displaying %s Text '%s' on zone %d with align of %d speed (%d/%d)\r\n", AppSettings.max7219_display[zone_num].scroll ? "Scroll" : "Static", AppSettings.max7219_display[zone_num].text.c_str(), zone_num, AppSettings.max7219_display[zone_num].alignment, led->getSpeed(zone_num), led->getPause(zone_num));
				Serial.printf("Text Effect In/Out %d/%d\r\n", AppSettings.max7219_display[zone_num].effect_in, AppSettings.max7219_display[zone_num].effect_out);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_speed)) {
				//ledMatrixTimer.setIntervalMs(message.toInt());
				//led->displayReset();
				led->setSpeed(zone_num, message.toInt());
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_pause)) {
				//ledMatrixTimer.setIntervalMs(message.toInt());
				led->setPause(zone_num, message.toInt());
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_charwidth)) {
				led->setCharSpacing(message.toInt());
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_invert)) {
				AppSettings.max7219_display[zone_num].anim = true;
				led->setInvert(message.toInt());
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_scroll)) {
				AppSettings.max7219_display[zone_num].anim = true;
				AppSettings.max7219_display[zone_num].scroll = message.toInt();
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_intensity)) {
				led->setIntensity(zone_num, message.toInt());
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_effect_in)) {
				AppSettings.max7219_display[zone_num].anim = true;
				AppSettings.max7219_display[zone_num].effect_in = (textEffect_t)message.toInt();
				led->setTextEffect(zone_num, AppSettings.max7219_display[zone_num].effect_in, AppSettings.max7219_display[zone_num].effect_out);
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_effect_out)) {
				AppSettings.max7219_display[zone_num].anim = true;
				AppSettings.max7219_display[zone_num].effect_out = (textEffect_t)message.toInt();
				led->setTextEffect(zone_num, AppSettings.max7219_display[zone_num].effect_in, AppSettings.max7219_display[zone_num].effect_out);
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_alignment)) {
				AppSettings.max7219_display[zone_num].anim = true;
				AppSettings.max7219_display[zone_num].alignment = (textPosition_t)message.toInt();
				led->setTextAlignment(AppSettings.max7219_display[zone_num].alignment);
				if (AppSettings.max7219_display[zone_num].reset) led->displayReset(zone_num);
			}
			else if (cmdtopic.equals(AppSettings.max7219_topic_reset)) {
				AppSettings.max7219_display[zone_num].reset = (bool)message.toInt();
				Serial.printf("Display Reset set to %d", AppSettings.max7219_display[zone_num].reset);
			}
		} else {
			//debugf("Not enough topic pieces!\n");
		}
	}
}


/* Timer callback function to publish values from attached sensors */
void sensorPublish()
{
	/* If ADC is enabled */
	if (AppSettings.adc) {
		/* Read out ADC value */
		int a = system_adc_read();
		/* If publishing of ADC Values is enabled, then publish */
		if (AppSettings.adc_pub) {
			/* Check mqtt connection status */
			if (mqtt.getConnectionState() != eTCS_Connected)
				startMqttClient(); // Auto reconnect

			/* Publish mqtt adc value */
			mqtt.publish(AppSettings.adc_topic, String(a), true);
			//debugf("sensorPublish() ADC published");
		}
	}
}

void onMessageDelivered(uint16_t msgId, int type)
{
	debugf("Message with id %d and QoS %d was delivered successfully.", msgId, (type == MQTT_MSG_PUBREC ? 2 : 1));
}

/* Start MQTT client and publish/subscribe to the used services */
void startMqttClient()
{
	// 1. [Setup]
	if(!mqtt.setWill(AppSettings.mqtt_topic_lwt, "offline", 2, true)) {
		debugf("Unable to set the last will and testament. Most probably there is not enough memory on the device.");
	}

	mqtt.setCompleteDelegate(checkMQTTConnection);
	mqtt.setCallback(onMQTTMessageReceived);

#ifdef ENABLE_SSL
	mqtt.addSslOptions(SSL_SERVER_VERIFY_LATER);

#include <ssl/private_key.h>
#include <ssl/cert.h>

	mqtt.setSslKeyCert(default_private_key, default_private_key_len, default_certificate, default_certificate_len, NULL,
					   /*freeAfterHandshake*/ false);

#endif

	/*
	// For testing purposes, try a few different URL formats
	#define MQTT_URL1 "mqtt://attachix.com:1883"
	#define MQTT_URL2 "mqtts://attachix.com:8883" // (Need ENABLE_SSL)
	#define MQTT_URL3 "mqtt://frank:fiddle@192.168.100.107:1883"
	*/

	// 2. [Connect]
	char mqtt_url[100] = {0};
	bool mqtt_ssl = false; // TODO: Add to webif
	sprintf(mqtt_url, "mqtt%s://%s:%s@%s:%d", mqtt_ssl ? "s" : "", AppSettings.mqtt_login.c_str(), AppSettings.mqtt_password.c_str(), AppSettings.mqtt_server.c_str(), AppSettings.mqtt_port);
	URL url(mqtt_url);
	Serial.printf("Connecting to \t%s\n", url.toString().c_str());

	if (mqtt.connect(url, AppSettings.mqtt_userid)) {
		debugf("MQTT Successfully connected!");
	} else {
		debugf("MQTT Connection failed!");
	}

	/* Subscribe to client command topic for general commands */
	if (mqtt.subscribe(AppSettings.mqtt_topic_cmd)) {
		debugf("MQTT General Command Subscription successful!");
	} else {
		debugf("MQTT General Command Subscription failed!");
	}

	/* Publish LWT message */
	mqtt.publishWithQoS(AppSettings.mqtt_topic_lwt, WifiStation.getIP().toString(), 1, true, onMessageDelivered);

	/* If relay is attached and enabled in settings */
	if (AppSettings.relay) {
		/* Publish current relay state to mqtt */
		mqtt.publish(AppSettings.relay_topic_pub, String(relay.get()), true);
		//TODO: Add relay config key, key_pin, relay_pin, ...
		/* Attach interrupt if enabled in config */
		//attachInterrupt(KEY_INPUT_PIN, keyIRQHandler, CHANGE);
		/* Check if set topics are at least 1 byte long */
		if ((AppSettings.relay_topic_cmd.length() >= 1) && (AppSettings.relay_topic_pub.length() >= 1)) {
			/* Check if topic backup is set */
			//if (AppSettings.relay_topic_cmd_old.length() > 0)
			//	mqtt->unsubscribe(AppSettings.relay_topic_cmd_old); /* Unsubscribe backup relay topic, for changes in webinterface */
			mqtt.subscribe(AppSettings.relay_topic_cmd); /* Resubscribe relay topic */
			/* Set backup relay topic to current relay topic */
			AppSettings.relay_topic_cmd_old = AppSettings.relay_topic_cmd;
		} else {
			// TODO: show error for to short relay topics with ajax
		}
	}

	if (AppSettings.rcswitch) {
		//TODO: Check if topic prefix got already trailing / and/or #
		mqtt.subscribe(AppSettings.rcswitch_topic_prefix + "#");
		rcSwitch.enableTransmit(AppSettings.rcswitch_pin);
	} else {
		//mqtt->unsubscribe(AppSettings.rcswitch_topic_prefix + "#");
		rcSwitch.disableTransmit();
	}

	if (AppSettings.max7219) {
		mqtt.subscribe(AppSettings.max7219_topic_prefix + "#");
		/*mqtt->subscribe(AppSettings.max7219_topic_prefix + AppSettings.max7219_topic_enable);
		mqtt->subscribe(AppSettings.max7219_topic_prefix + AppSettings.max7219_topic_text);
		mqtt->subscribe(AppSettings.max7219_topic_prefix + AppSettings.max7219_topic_charwidth);
		mqtt->subscribe(AppSettings.max7219_topic_prefix + AppSettings.max7219_topic_speed);
		mqtt->subscribe(AppSettings.max7219_topic_prefix + AppSettings.max7219_topic_scroll);
		mqtt->subscribe(AppSettings.max7219_topic_prefix + AppSettings.max7219_topic_intensity);*/
	}
}

/* Start used services for connected peripherals */
void startServices()
{
	if (AppSettings.udp) {
		/* Close any udp connections */
		udp.close();
		/* Start UDP Server */
		if (AppSettings.udp_port > 0)
			udp.listen(AppSettings.udp_port);
		//TODO UDPserver: else error wrong/no udp port assigned
	}
}

void checkMQTTConnection(TcpClient& client, bool flag)
{
	/* Check if MQTT Connection is still alive */
	if (flag == true)
		debugf("MQTT Connection died! Reconnecting! %d", mqtt.getConnectionState());
		startMqttClient(); /* Auto reconnect */
}

void checkMQTTDisconnect()
{
	if(mqtt.getConnectionState() != eTCS_Connected) {
			startMqttClient(); // Auto reconnect
	}
}

/* Start mDNS Server using ESP8266 SDK functions */
void startmDNS(IPAddress ip)
{
    struct mdns_info *info = (struct mdns_info *)os_zalloc(sizeof(struct mdns_info));
    info->host_name = (char *) DEFAULT_MDNS_HOSTNAME; // You can replace test with your own host name
    info->ipAddr = ip;
    info->server_name = (char *) DEFAULT_MDNS_SERVERNAME;
    info->server_port = DEFAULT_MDNS_SERVERPORT;
    info->txt_data[0] = (char *) "version = now";
    espconn_mdns_init(info);
}

// Stop mDNS Server
void stopmDNS()
{
	espconn_mdns_close();
}


/* Sets Wifi Accesspoint Settings and start it */
void startAP()
{
	/* Put on Setup LED Indicator to indicate running ap */
	statusLed(true);

	//disable watchdog
	WDT.enable(false);

	/* Start AP for configuration */
	//WifiAccessPoint.enable(true);
	WifiAccessPoint.config(NINHOME_AP_NAME, "", AUTH_OPEN);
}

/* Stop Wifi Accesspoint if running */
void stopAP()
{
	/* If Wifi Accesspoint is enabled then disable it */
	if (WifiAccessPoint.isEnabled())
		WifiAccessPoint.enable(false);

	/* Put off Setup LED Indicator */
	statusLed(false);

	//TODO: disable mDNS server on stopAP()
	/* Stop mDNS server */
	//stopmDNS();
}

void debounceKey()
{
	if (key_pressed && (millis() - lastKeyPress) >= 500) { // check if key is pressed and last key press is ddd milliseconds away (need config value)
		if (AppSettings.keyinput_invert) {
			if (digitalRead(AppSettings.keyinput_pin) == LOW)
				key_pressed = true;
			else
				key_pressed = false;
		} else {
			if (digitalRead(AppSettings.keyinput_pin) == HIGH)
				key_pressed = true;
			else
				key_pressed = false;
		}

		if (key_pressed) {
			relay.set(!relay.get()); // toggle relay
			mqtt.publish(AppSettings.relay_topic_pub, String(relay.get()), true); // send actual state to broker
			lastKeyPress = millis(); // reset last key press millis
			key_pressed = false; // reset key_pressed value
			interrupts(); // re-enable the interrupts
		}
	}
}

/* Callback when wifi network scan is finished */
void networkScanCompleted(bool succeeded, BssList list)
{
	/* if the network scan was successful */
	if (succeeded)
	{
		/* populate the networks list with the found networks */
			for (int i = 0; i < list.count(); i++)
					if (!list[i].hidden && list[i].ssid.length() > 0) {
							wNetworks.add(list[i]);
							debugf("Network %d = %s", i, list[i].ssid.c_str());
						}
		}
	/* sort the found networks */
	wNetworks.sort([](const BssInfo& a, const BssInfo& b){ return b.rssi - a.rssi; } );
}

void ledMatrixCb()
{
	ledMatrixTimer.stop();

	for (uint8_t z = 0; z < AppSettings.max7219_zones; z++) {
		if (AppSettings.max7219_display[z].enabled) {
			led->displayAnimate();
			if (AppSettings.max7219_display[z].anim && led->getZoneStatus(z)) {
				if (AppSettings.max7219_display[z].scroll) {
					led->displayZoneText(z, (char*)AppSettings.max7219_display[z].text.c_str(), AppSettings.max7219_display[z].alignment, led->getSpeed(z), led->getPause(z), AppSettings.max7219_display[z].effect_in, AppSettings.max7219_display[z].effect_out);
					AppSettings.max7219_display[z].anim = true;
				}
			}
		} else {
			led->displayClear(z);
		}
	}

	/*if (displayEnable) {
		if (displayAnim && led->displayAnimate()) {
			if (displayScroll) {
				led->displayText((char*)AppSettings.max7219_text.c_str(), AppSettings.max7219_alignment, led->getSpeed(), led->getPause(), AppSettings.max7219_effect_in, AppSettings.max7219_effect_out);
				displayAnim = true;
			}
		}
	} else {
		led->displayClear();
	}*/

	ledMatrixTimer.restart();
}

/* Callback when WiFi station was connected to AP and got IP */
void connectOk(IPAddress ip, IPAddress mask, IPAddress gateway)
{
	/* Debug output of IP */
	debugf("I'm CONNECTED");
	Serial.println(ip.toString());

	if (AppSettings.existMQTT()) {
		AppSettings.loadMQTT();

		/* Start MQTT client and publish/subscribe used extensions */
		debugf("Starting MQTT Client");
		startMqttClient();

		/* Load peripheral settings */
		if (AppSettings.existPeriph()) {
			AppSettings.loadPeriph();

			/* Start Services for Peripherals */
			startServices();

			/* Start Timer for publishing attached sensor values, interval from settings */
			/* Add every sensor who want to publish something here */
			if (AppSettings.adc) {
				sensorPublishTimer.initializeMs(AppSettings.timer_delay, sensorPublish).start();
			}

			if (AppSettings.motion) {
				debugf("Starting Motion Sensing Timer");
				motionCheckTimer.initializeMs(AppSettings.motion_interval, motionSensorCheck).start();
			}

			if (AppSettings.relay && AppSettings.keyinput) {
				debounceTimer.initializeMs(100, debounceKey).start();
			}

			if (AppSettings.max7219) {
				ledMatrixTimer.initializeMs(5, ledMatrixCb).start();
			}

			/* Start timer which checks the connection to MQTT */
			checkConnectionTimer.initializeMs(3000, checkMQTTDisconnect).start();

			//stopAP(); TODO: When to stop AP? Or manually triggered in webinterface?
		}
	} else {
		debugf("No MQTT Settings found. Not starting MQTT client");
	}
}

// Callback when WiFi station timeout was reached
void connectFail(String ssid, uint8_t ssidLength, uint8_t *bssid, uint8_t reason)
{
	//debugf("Disconnected from %s. Reason: %d", ssid.c_str(), reason);
	//if (reason >= 200) {//|| reason == 2) {
	//	WifiStation.disconnect();
	//	debugf("Falling Back to Setup Mode");
	//	startAP();
	//} else {
	//	debugf("Trying to reconnect to Wifi");
	//	WifiStation.connect();
	//}

	if (AppSettings.existGlobal()) {
		if (ssid.equals(AppSettings.ssid)) {
			debugf("Wifi Disconnected! Trying to reconnect...");
			WifiStation.connect();
		}
	}
}

void statusLed(bool state)
{
	if (AppSettings.status_led_pin > -1) {
		pinMode(AppSettings.status_led_pin, OUTPUT);
		if (AppSettings.status_led_inv)
			state = !state;
		digitalWrite(AppSettings.status_led_pin, state);
	}
}

bool getStatusLed()
{
	if (AppSettings.status_led_pin > -1) {
		if (AppSettings.status_led_inv)
			return !digitalRead(AppSettings.status_led_pin);
		else
			return digitalRead(AppSettings.status_led_pin);
	} else {
		return false;
	}
}

void motionSensorCheck()
{
	if (digitalRead(AppSettings.motion_pin) == AppSettings.motion_invert) {
		if (motionState != true) {
			motionState = true;
			//if (mqtt->getConnectionState() != eTCS_Connected) //TODO remove autoconnect cause of connCheck timer?
			//			startMqttClient(); // Auto reconnect
			mqtt.publishWithQoS(AppSettings.motion_topic, "1", 1, true, onMessageDelivered);
			debugf("Motion ON (%s)", AppSettings.motion_topic.c_str());
		}
		//digitalWrite(14, 1);
	} else {
		if (motionState != false) {
			motionState = false;
			//if (mqtt->getConnectionState() != eTCS_Connected)
			//			startMqttClient(); // Auto reconnect
			mqtt.publishWithQoS(AppSettings.motion_topic, "0", 1, true, onMessageDelivered);
			debugf("Motion OFF (%s)", AppSettings.motion_topic.c_str());
		}
		//digitalWrite(14, 0);
	}
}

void serialCb(Stream& stream, char arrivedChar, unsigned short availableCharsCount) {

	if (arrivedChar == '\n') {
		char str[availableCharsCount];
		for (int i = 0; i < availableCharsCount; i++) {
			str[i] = stream.read();
			if (str[i] == '\r' || str[i] == '\n') {
				str[i] = '\0';
			}
		}

		if (!strcmp(str, "ip")) {
			Serial.printf("ip: %s mac: %s\r\n", WifiStation.getIP().toString().c_str(), WifiStation.getMAC().c_str());
		/*} else if (!strcmp(str, "ota")) {
			otaUpdate();
		*/} else if (!strcmp(str, "restart")) {
			System.restart();
		} else if (!strcmp(str, "ls")) {
			Vector<String> files = fileList();
			Serial.printf("filecount %d\r\n", files.count());
			for (unsigned int i = 0; i < files.count(); i++) {
				Serial.println(files[i]);
			}
		} else if (!strcmp(str, "cat")) {
			Vector<String> files = fileList();
			if (files.count() > 0) {
				Serial.printf("dumping file %s:\r\n", files[0].c_str());
				Serial.println(fileGetContent(files[0]));
			} else {
				Serial.println("Empty spiffs!");
			}
		} else if (!strcmp(str, "info")) {
			serialShowInfo();
		} else if (!strcmp(str, "help")) {
			Serial.println();
			Serial.println("available commands:");
			Serial.println("  help - display this message");
			Serial.println("  ip - show current ip address");
			//Serial.println("  connect - connect to wifi");
			Serial.println("  restart - restart the esp8266");
			//Serial.println("  ota - perform ota update, switch rom and reboot");
			Serial.println("  info - show esp8266 info");
#ifndef DISABLE_SPIFFS
			Serial.println("  ls - list files in spiffs");
			Serial.println("  cat - show first file in spiffs");
#endif
			Serial.println();
		} else {
			Serial.println("unknown command");
		}
	}
}

int splitString(String &what, int delim,  String *splits)
{
  what.trim();
  const char *chars = what.c_str();
  int splitCount = 0; //1;
  for (int i = 0; i < what.length(); i++) {
    if (chars[i] == delim) splitCount++;
  }
  if (splitCount == 0) {
    splits[0] = what.c_str();
    return 1;
  }

  int pieceCount = splitCount + 1;

  int splitIndex = 0;
  int startIndex = 0;
  for (int i = 0; i < what.length(); i++) {
    if (chars[i] == delim) {
      splits[splitIndex] = what.substring(startIndex, i).c_str();
      splitIndex++;
      startIndex = i + 1;
    }
  }
  splits[splitIndex] = what.substring(startIndex, what.length()).c_str();

  return pieceCount;
}

// Will be called when WiFi hardware and software initialization was finished
// And system initialization was completed
void ready()
{
	debugf("READY!");

	//TODO: activate mDNS on startAP()
	/* Start mDNS server on Wifi Accesspoint IP */
	//startmDNS(WifiAccessPoint.getIP());

	startWebinterface();

	// If AP is enabled:
	debugf("AP. ip: %s mac: %s", WifiAccessPoint.getIP().toString().c_str(), WifiAccessPoint.getMAC().c_str());
}

void init()
{
	/* Mount file system, in order to work with files */
	spiffs_mount();
	//int slot = rboot_get_current_rom();

	/* Start Serial Debug Terminal */
	Serial.begin(115200); // 115200 by default

	//TODO: if debug disabled it get into bootloop, inspect stacktrace
	/* Enable debug output to serial */
	Serial.systemDebugOutput(true);

	// initialize the LCD
	/*lcd.begin(16,2);
	lcd.setBacklightPin(3, POSITIVE);
	lcd.setBacklight(HIGH);
	// Turn on the blacklight and print a message.
	lcd.home ();                   // go home
	lcd.print("Hello, ARDUINO ");
	lcd.setCursor ( 0, 1 );        // go to the next line
	lcd.print (" FORUM - fm   ");
*/

	Serial.setCallback(serialCb);

/*
#ifndef DISABLE_SPIFFS
	if (slot == 0) {
#ifdef RBOOT_SPIFFS_0
		debugf("trying to mount spiffs at 0x%08x, length %d", RBOOT_SPIFFS_0, SPIFF_SIZE);
		spiffs_mount_manual(RBOOT_SPIFFS_0, SPIFF_SIZE);
#else
		debugf("trying to mount spiffs at 0x%08x, length %d", 0x100000, SPIFF_SIZE);
		spiffs_mount_manual(0x100000, SPIFF_SIZE);
#endif
	} else {
#ifdef RBOOT_SPIFFS_1
		debugf("trying to mount spiffs at 0x%08x, length %d", RBOOT_SPIFFS_1, SPIFF_SIZE);
		spiffs_mount_manual(RBOOT_SPIFFS_1, SPIFF_SIZE);
#else
		debugf("trying to mount spiffs at 0x%08x, length %d", 0x300000, SPIFF_SIZE);
		spiffs_mount_manual(0x300000, SPIFF_SIZE);
#endif
	}
#else
	debugf("spiffs disabled");
#endif

	Serial.printf("\r\nCurrently running rom %d.\r\n", slot);
	Serial.println();
*/
	/* Start initialization of ninHOME node */

	/* Load Last Modified Timestamp from the Webcontent */
	if(fileExist(".lastModified")) {
		// The last modification
		lastModified = fileGetContent(".lastModified");
		lastModified.trim();
	}

	/* Enable Wifi Client, even if none is set. Just to scan out existing networks */
    WifiStation.enable(true, false);
    WifiAccessPoint.enable(false, false);

	/* Enable Wifi Scanning and process in "networkScanCompleted" callback function */
  	WifiStation.startScan(networkScanCompleted);

	/* Check if there is already an existing configuration for wifi network */
	if (AppSettings.existNetwork())
	{
		/* Check if all config files are present */
		if (AppSettings.existGlobal()) {
			debugf("All Config files found!\r\n");
			/* Load other configuration files */
			AppSettings.loadAll();
		} else { /* Create missing config files */
			debugf("Some config files are missing. Creating templates");
			if (!AppSettings.existMQTT()) AppSettings.saveMQTT();
			if (!AppSettings.existPeriph()) AppSettings.savePeriph();
		}

		statusLed(false);

		/* Blink Setup LED 3 Times to indicate starting */
		for (uint8_t i = 1; i <= 6; i++) {
			statusLed(!getStatusLed());
			delay(200);
		}

		/* If a motion sensor is attached, enable the input gpio */
		if (AppSettings.motion) {
			debugf("Motion Sensor activated! Setting pinmode!");
			pinMode(AppSettings.motion_pin, INPUT_PULLUP); /* config setting for pullup required? */
		}

		/* If a relay attached and enabled in settings we init it here */
		if (AppSettings.relay) {
			relay.init(AppSettings.relay_pin, AppSettings.relay_status_pin, AppSettings.relay_invert, AppSettings.relay_status_invert, false);
			if (AppSettings.keyinput) {
				if (AppSettings.keyinput_pin > -1) {
					pinMode(AppSettings.keyinput_pin, INPUT);/* config setting for pullup required? */
					//debugf("Relay->Keyinput activated for pin %d. Attaching IRQ", AppSettings.keyinput_pin);
					if (AppSettings.keyinput_invert)
						attachInterrupt(AppSettings.keyinput_pin, keyIRQHandler, FALLING);
					else
						attachInterrupt(AppSettings.keyinput_pin, keyIRQHandler, RISING);
				}
			}
		}

		if (AppSettings.max7219) {
			debugf("Initialize MAX7219 LED Matrix x%d - SS Pin %d - %d Zones", AppSettings.max7219_count, AppSettings.max7219_ss_pin, AppSettings.max7219_zones);
			//led->init(AppSettings.max7219_count, AppSettings.max7219_ss_pin);
			//TODO: Set SS Pin and Count to runtime
			//led(AppSettings.max7219_ss_pin, AppSettings.max7219_count);
			//led = new MD_Parola(4, AppSettings.max7219_count);

			ledPtr.Construct(MD_MAX72XX::FC16_HW, AppSettings.max7219_ss_pin, AppSettings.max7219_count);
			led = &ledPtr.value;

			led->begin(AppSettings.max7219_zones);

			int8_t lastModule = -1;
			for (uint8_t z = 0; z < AppSettings.max7219_zones; z++) {
				led->setZone(z, lastModule+1, AppSettings.max7219_display[z].size+lastModule);
				debugf("MAX7219 Zone %d: %d Modules (%d -> %d)", z, AppSettings.max7219_display[z].size, lastModule+1, AppSettings.max7219_display[z].size+lastModule);
				lastModule = AppSettings.max7219_display[z].size-1;
			}

			//uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Deg C
			//led->addChar('$', degC);


			for (uint8_t z = 0; z < AppSettings.max7219_zones; z++)
			#if ENA_SPRITE
			for (uint8_t z = 0; z < AppSettings.max7219_zones; z++)
				led->setSpriteData(z, sprite_in, W_SPRITE1, F_SPRITE1, sprite_out, W_SPRITE2, F_SPRITE2);
			#endif

			for (uint8_t z = 0; z < AppSettings.max7219_zones; z++) {
				led->displayClear(z);
				led->setSpeed(z, AppSettings.max7219_display[z].speed);
				led->displayZoneText(z, (char*)"", AppSettings.max7219_display[z].alignment, led->getSpeed(z), led->getPause(z), AppSettings.max7219_display[z].effect_in, AppSettings.max7219_display[z].effect_out);
			}
			//while(led->displayAnimate());

			//ledMatrixTimer.initializeMs(200, ledMatrixCb).start();
		}

		/* If there is any ssid set then assume there is a network set */
		if (AppSettings.ssid.length() > 0) {
			debugf("Settings found. Starting Station Mode");
			/* Stopping Accesspoint mode if it is running */
			stopAP();

			//TODO: nosave option wont work correctly
			/* Set the config values to the corrosponding wifistation settings and disable saving */
			WifiStation.config(AppSettings.ssid, AppSettings.password, true, false);

			/* If dhcp is disabled in settings then enable set fixed ip from settings */
			if (!AppSettings.dhcp && !AppSettings.ip.isNull())
				WifiStation.setIP(AppSettings.ip, AppSettings.netmask, AppSettings.gateway);

			/* register callbacks for GotIP and Disconnect wifi events */
			WifiEvents.onStationGotIP(connectOk);
			/* set callback that should be triggered if we are disconnected or connection attempt failed */
			WifiEvents.onStationDisconnect(connectFail);
		} else {
			/* No valid settings found, so start access point mode */
			debugf("Settings found but SSID is empty! Starting Setup Mode");
			startAP();
		}
	} else {
		/* No settings file found, so start access point mode */
		debugf("No Settings found. Starting Setup Mode");
		startAP();
	}

	/* Register onReady callback to run services on system ready */
	System.onReady(ready);


}
