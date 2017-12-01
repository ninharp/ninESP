#include <user_config.h>
#include <app_config.h>

#include <SmingCore/SmingCore.h>
#include <AppSettings.h>
#include <Libraries/RCSwitch/RCSwitch.h>
#include <RelaySwitch.h>
#include <ninMQTTClient.h>
/*
 *
 * if (mqtt.getConnectionState() != eTCS_Connected)
				startMqttClient(); // Auto reconnect
 *
 */

HttpServer server;
FTPServer ftp;

BssList networks;
String network, password;
Timer connectionTimer;

String lastModified;

RCSwitch rcSwitch = RCSwitch();
RelaySwitch relay = RelaySwitch();

Timer sensorPublishTimer;

// Forward declarations
void startMqttClient();
void onMessageReceived(String topic, String message);
void connectOk(IPAddress ip, IPAddress mask, IPAddress gateway);
void connectFail(String ssid, uint8_t ssidLength, uint8_t *bssid, uint8_t reason);

// MQTT client
// For quickly check you can use: http://www.hivemq.com/demos/websocket-client/ (Connection= test.mosquitto.org:8080)
ninMqttClient mqtt(DEFAULT_MQTT_SERVER, DEFAULT_MQTT_PORT, onMessageReceived);

// Callback for messages, arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	if (AppSettings.relay && AppSettings.relay_topic.equals(topic)) {
		debugf("Relay Command received!");
		if (message.equals("on") || message.equals("1")) { // Relay ON
			relay.set(true);
		} else if (message.equals("off") || message.equals("0")) { // Relay OFF
			relay.set(false);
		}
	} else {
		Serial.print(topic);
		Serial.print(":\r\n\t"); // Prettify alignment for printing
		Serial.println(message);
	}
}

void sensorPublish()
{
	debugf("sensorPublish()");
	if (AppSettings.adc) { // if ADC is enabled in webinterface
		int a = system_adc_read();
		if (AppSettings.adc_pub) { // Publish to MQTT if enabled in webinterface
			if (mqtt.getConnectionState() != eTCS_Connected)
							startMqttClient(); // Auto reconnect

			mqtt.publish(AppSettings.adc_topic, String(a), true);
		}
	}
}

// Run MQTT client
void startMqttClient()
{
	if(!mqtt.setWill(AppSettings.mqtt_topic_lwt, "offline", 1, true)) {
		debugf("Unable to set the last will and testament. Most probably there is not enough memory on the device.");
	}
	mqtt.setHost(AppSettings.mqtt_server);
	mqtt.setPort(AppSettings.mqtt_port);
	mqtt.connect(AppSettings.mqtt_userid, AppSettings.mqtt_login, AppSettings.mqtt_password);
	mqtt.subscribe(AppSettings.mqtt_topic_cmd);
	//mqtt.publish(AppSettings.mqtt_topic_pub, "online");
	mqtt.publishWithQoS(AppSettings.mqtt_topic_lwt, "online", 1, true);
}

void onIndex(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();
	response.sendTemplate(tmpl); // will be automatically deleted
}

void onReboot(HttpRequest &request, HttpResponse &response)
{
	debugf("Restarting...");
	WDT.enable(true);
	System.restart();
}

void onIpConfig(HttpRequest &request, HttpResponse &response)
{
	if (request.method == HTTP_POST)
	{
		AppSettings.dhcp = request.getPostParameter("dhcp") == "1";
		AppSettings.ip = request.getPostParameter("ip");
		AppSettings.netmask = request.getPostParameter("netmask");
		AppSettings.gateway = request.getPostParameter("gateway");
		debugf("Updating IP settings: %d", AppSettings.ip.isNull());
		AppSettings.saveGlobal();
	}

	TemplateFileStream *tmpl = new TemplateFileStream("settings.html");
	auto &vars = tmpl->variables();

	bool dhcp = WifiStation.isEnabledDHCP();
	vars["dhcpon"] = dhcp ? "checked='checked'" : "";
	vars["dhcpoff"] = !dhcp ? "checked='checked'" : "";

	if (!WifiStation.getIP().isNull())
	{
		vars["ip"] = WifiStation.getIP().toString();
		vars["netmask"] = WifiStation.getNetworkMask().toString();
		vars["gateway"] = WifiStation.getNetworkGateway().toString();
	}
	else
	{
		vars["ip"] = "192.168.1.77";
		vars["netmask"] = "255.255.255.0";
		vars["gateway"] = "192.168.1.1";
	}

	response.sendTemplate(tmpl); // will be automatically deleted
}

void onPeriphConfig(HttpRequest &request, HttpResponse &response)
{
	if (request.method == HTTP_POST)
	{
		AppSettings.timer_delay = atoi(request.getPostParameter("timer_delay").c_str());

		AppSettings.relay = request.getPostParameter("relay").equals("on") ? true : false;
		AppSettings.adc = request.getPostParameter("sensor_adc").equals("on") ? true : false;
		AppSettings.temp_dht11 = request.getPostParameter("temp_dht11").equals("on") ? true : false;
		AppSettings.temp_ds18b20 = request.getPostParameter("temp_ds18b20").equals("on") ? true : false;
		AppSettings.temp_lm75 = request.getPostParameter("temp_lm75").equals("on") ? true : false;

		AppSettings.adc_pub = request.getPostParameter("sensor_adc_pub").equals("1") ? true : false;
		AppSettings.temp_dht11_pub = request.getPostParameter("temp_dht11_pub").equals("1") ? true : false;
		AppSettings.temp_ds18b20_pub = request.getPostParameter("temp_ds18b20_pub").equals("1") ? true : false;
		AppSettings.temp_lm75_pub = request.getPostParameter("temp_lm75_pub").equals("1") ? true : false;

		AppSettings.relay_topic = request.getPostParameter("topic_relay_pub");
		AppSettings.adc_topic = request.getPostParameter("topic_adc_pub");
		AppSettings.temp_dht11_temp_topic = request.getPostParameter("topic_dht11temp_pub");
		AppSettings.temp_dht11_humi_topic = request.getPostParameter("topic_dht11humi_pub");
		AppSettings.temp_ds18b20_topic = request.getPostParameter("topic_ds18b20_pub");
		AppSettings.temp_lm75_topic = request.getPostParameter("topic_lm75_pub");

		debugf("Updating Peripheral settings");
		AppSettings.savePeriph();

		if (AppSettings.relay) {
			if (AppSettings.relay_topic.length() >= 1) {
				relay.init(14, false);
				if (mqtt.getConnectionState() != eTCS_Connected)
						startMqttClient(); // Auto reconnect

				mqtt.unsubscribe(AppSettings.relay_topic_old);
				mqtt.subscribe(AppSettings.relay_topic);
				AppSettings.relay_topic_old = AppSettings.relay_topic;
			} else {
				// TODO: show error with ajax
			}
		} else {
			if (AppSettings.relay_topic.length() >= 1) {
				mqtt.unsubscribe(AppSettings.relay_topic_old);
			}
		}

		sensorPublishTimer.setIntervalMs(AppSettings.timer_delay);
	} else {
		AppSettings.loadPeriph();
	}

	TemplateFileStream *tmpl = new TemplateFileStream("periph.html");
	auto &vars = tmpl->variables();

	vars["timer_delay"] = String(AppSettings.timer_delay);

	vars["relay_on"] = AppSettings.relay ? "checked='checked'" : "";
	vars["sensor_adc_on"] = AppSettings.adc ? "checked='checked'" : "";
	vars["temp_dht11_on"] = AppSettings.temp_dht11 ? "checked='checked'" : "";
	vars["temp_ds18b20_on"] = AppSettings.temp_ds18b20 ? "checked='checked'" : "";
	vars["temp_lm75_on"] = AppSettings.temp_lm75 ? "checked='checked'" : "";

	vars["adc_pub_on"] = AppSettings.adc_pub ? "checked='checked'" : "";
	vars["adc_pub_off"] = !AppSettings.adc_pub ? "checked='checked'" : "";

	vars["dht11_pub_on"] = AppSettings.temp_dht11_pub ? "checked='checked'" : "";
	vars["dht11_pub_off"] = !AppSettings.temp_dht11_pub ? "checked='checked'" : "";

	vars["ds18_pub_on"] = AppSettings.temp_ds18b20_pub ? "checked='checked'" : "";
	vars["ds18_pub_off"] = !AppSettings.temp_ds18b20_pub ? "checked='checked'" : "";

	vars["lm75_pub_on"] = AppSettings.temp_lm75_pub ? "checked='checked'" : "";
	vars["lm75_pub_off"] = !AppSettings.temp_lm75_pub ? "checked='checked'" : "";

	vars["topic_relay"] = AppSettings.relay_topic;
	vars["topic_adc"] = AppSettings.adc_topic;
	vars["topic_dht11t"] = AppSettings.temp_dht11_temp_topic;
	vars["topic_dht11h"] = AppSettings.temp_dht11_humi_topic;
	vars["topic_ds18b20"] = AppSettings.temp_ds18b20_topic;
	vars["topic_lm75"] = AppSettings.temp_lm75_topic;

	response.sendTemplate(tmpl); // will be automatically deleted
}

void onMQTTConfig(HttpRequest &request, HttpResponse &response)
{
	if (request.method == HTTP_POST)
	{
		AppSettings.mqtt_server = request.getPostParameter("server");
		AppSettings.mqtt_port = request.getPostParameter("port");
		AppSettings.mqtt_userid = request.getPostParameter("userid");
		AppSettings.mqtt_login = request.getPostParameter("login");
		AppSettings.mqtt_password = request.getPostParameter("password");
		AppSettings.mqtt_topic_lwt = request.getPostParameter("topic_lwt");
		AppSettings.mqtt_topic_cmd = request.getPostParameter("topic_cmd");
		AppSettings.mqtt_topic_pub = request.getPostParameter("topic_pub");
		debugf("Updating MQTT settings: %d", AppSettings.mqtt_server.length());
		AppSettings.saveMQTT();
	}

	TemplateFileStream *tmpl = new TemplateFileStream("mqtt.html");
	auto &vars = tmpl->variables();

	if (AppSettings.mqtt_server.length() <= 0) { vars["server"] = DEFAULT_MQTT_SERVER; } else { vars["server"] = AppSettings.mqtt_server; }
	if (AppSettings.mqtt_port.length() <= 0) { vars["port"] = DEFAULT_MQTT_PORT; } else { vars["port"] = AppSettings.mqtt_port; }
	if (AppSettings.mqtt_login.length() <= 0) { vars["login"] = DEFAULT_MQTT_LOGIN; } else { vars["login"] = AppSettings.mqtt_login; }
	if (AppSettings.mqtt_userid.length() <= 0) { vars["userid"] = DEFAULT_MQTT_USERID; } else { vars["userid"] = AppSettings.mqtt_userid; }
	if (AppSettings.mqtt_password.length() <= 0) { vars["password"] = DEFAULT_MQTT_PASS; } else { vars["password"] = AppSettings.mqtt_password; }
	if (AppSettings.mqtt_topic_lwt.length() <= 0) { vars["topic_lwt"] = DEFAULT_MQTT_LWT; } else { vars["topic_lwt"] = AppSettings.mqtt_topic_lwt; }
	if (AppSettings.mqtt_topic_cmd.length() <= 0) { vars["topic_cmd"] = DEFAULT_MQTT_CMD; } else { vars["topic_cmd"] = AppSettings.mqtt_topic_cmd; }
	if (AppSettings.mqtt_topic_pub.length() <= 0) { vars["topic_pub"] = DEFAULT_MQTT_PUB; } else { vars["topic_pub"] = AppSettings.mqtt_topic_pub; }

	response.sendTemplate(tmpl); // will be automatically deleted
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	if (lastModified.length() >0 && request.getHeader("If-Modified-Since").equals(lastModified)) {
		response.code = HTTP_STATUS_NOT_MODIFIED;
		return;
	}

	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		if(lastModified.length() > 0) {
			response.setHeader("Last-Modified", lastModified);
		}

		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}

void onDumpConfig(HttpRequest &request, HttpResponse &response)
{
	int size = fileGetSize(APP_PERIPH_SETTINGS_FILE);
	char* cfgString = new char[size + 1];
	fileGetContent(APP_PERIPH_SETTINGS_FILE, cfgString, size + 1);

	response.sendString(cfgString);

	delete[] cfgString;
}

void onAjaxNetworkList(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	json["status"] = (bool)true;

	bool connected = WifiStation.isConnected();
	json["connected"] = connected;
	if (connected)
	{
		// Copy full string to JSON buffer memory
		json["network"]= WifiStation.getSSID();
	}

	JsonArray& netlist = json.createNestedArray("available");
	for (int i = 0; i < networks.count(); i++)
	{
		if (networks[i].hidden) continue;
		JsonObject &item = netlist.createNestedObject();
		item["id"] = (int)networks[i].getHashId();
		// Copy full string to JSON buffer memory
		item["title"] = networks[i].ssid;
		item["signal"] = networks[i].rssi;
		item["encryption"] = networks[i].getAuthorizationMethodName();
	}

	response.setAllowCrossDomainOrigin("*");
	response.sendDataStream(stream, MIME_JSON);
}

void makeConnection()
{
	WifiStation.enable(true);
	WifiStation.config(network, password, true, false);
	// Run our method when station was connected to AP (or not connected)
	// Set callback that should be triggered when we have assigned IP
	WifiEvents.onStationGotIP(connectOk);

	// Set callback that should be triggered if we are disconnected or connection attempt failed
	WifiEvents.onStationDisconnect(connectFail);

	WifiStation.connect();

	AppSettings.ssid = network;
	AppSettings.password = password;
	AppSettings.saveGlobal();

	debugf("Making Connection");
	delay(3000);

	network = ""; // task completed
}

void onAjaxConnect(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	String curNet = request.getPostParameter("network");
	String curPass = request.getPostParameter("password");

	bool updating = curNet.length() > 0 && (WifiStation.getSSID() != curNet || WifiStation.getPassword() != curPass);
	bool connectingNow = WifiStation.getConnectionStatus() == eSCS_Connecting || network.length() > 0;

	if (updating && connectingNow)
	{
		debugf("wrong action: %s %s, (updating: %d, connectingNow: %d)", network.c_str(), password.c_str(), updating, connectingNow);
		json["status"] = (bool)false;
		json["connected"] = (bool)false;
	}
	else
	{
		json["status"] = (bool)true;
		if (updating)
		{
			network = curNet;
			password = curPass;
			debugf("CONNECT TO: %s %s", network.c_str(), password.c_str());
			json["connected"] = false;
			connectionTimer.initializeMs(1200, makeConnection).startOnce();
		}
		else
		{
			json["connected"] = WifiStation.isConnected();
			debugf("Network already selected. Current status: %s", WifiStation.getConnectionStatusName());
		}
	}

	if (!updating && !connectingNow && WifiStation.isConnectionFailed())
		json["error"] = WifiStation.getConnectionStatusName();

	response.setAllowCrossDomainOrigin("*");
	response.sendDataStream(stream, MIME_JSON);
}

void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/reboot", onReboot);
	server.addPath("/ipconfig", onIpConfig);
	server.addPath("/periph", onPeriphConfig);
	server.addPath("/dumpcfg", onDumpConfig);
	server.addPath("/mqttconfig", onMQTTConfig);
	server.addPath("/ajax/get-networks", onAjaxNetworkList);
	server.addPath("/ajax/connect", onAjaxConnect);
	server.setDefaultHandler(onFile);
}

void startFTP()
{
	if (!fileExist("index.html"))
		fileSetContent("index.html", "<h3>No SPIFFS found! Contact supplier!</h3>");

	// Start FTP server
	ftp.listen(DEFAULT_FTP_PORT);
	ftp.addUser(DEFAULT_FTP_USER, DEFAULT_FTP_PASS); // FTP account
}

// Will be called when system initialization was completed
void startServers()
{
	startFTP();
	startWebServer();
}

void startAP()
{
	digitalWrite(LED_SETUP_PIN, 1); // Put off Setup LED Indicator
	// Start AP for configuration
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config(NINHOME_AP_NAME, "", AUTH_OPEN);
}

void stopAP()
{
	WifiAccessPoint.enable(false);
	digitalWrite(LED_SETUP_PIN, 0); // Put off Setup LED Indicator
}

void networkScanCompleted(bool succeeded, BssList list)
{
	if (succeeded)
	{
		for (int i = 0; i < list.count(); i++)
			if (!list[i].hidden && list[i].ssid.length() > 0) {
				networks.add(list[i]);
				debugf("Network %d = %s", i, list[i].ssid.c_str());
			}
	}
	networks.sort([](const BssInfo& a, const BssInfo& b){ return b.rssi - a.rssi; } );
}

// Will be called when WiFi station was connected to AP
void connectOk(IPAddress ip, IPAddress mask, IPAddress gateway)
{
	debugf("I'm CONNECTED");
	Serial.println(ip.toString());

	// Run MQTT client
	debugf("Starting MQTT Client");
	startMqttClient();

	// Start Timer
	sensorPublishTimer.initializeMs(AppSettings.timer_delay, sensorPublish).start();

	//stopAP(); TODO: When to stop AP? Or manually triggered in webinterface?
}

// Will be called when WiFi station timeout was reached
void connectFail(String ssid, uint8_t ssidLength, uint8_t *bssid, uint8_t reason)
{
	// The different reason codes can be found in user_interface.h. in your SDK.
	debugf("Disconnected from %s. Reason: %d", ssid.c_str(), reason);
	if (reason >= 200) {//|| reason == 2) {
		WifiStation.disconnect();
		debugf("Falling Back to Setup Mode");
		startAP();
	}
	/*
		REASON_UNSPECIFIED              = 1,
	REASON_AUTH_EXPIRE              = 2,
	REASON_AUTH_LEAVE               = 3,
	REASON_ASSOC_EXPIRE             = 4,
	REASON_ASSOC_TOOMANY            = 5,
	REASON_NOT_AUTHED               = 6,
	REASON_NOT_ASSOCED              = 7,
	REASON_ASSOC_LEAVE              = 8,
	REASON_ASSOC_NOT_AUTHED         = 9,
	REASON_DISASSOC_PWRCAP_BAD      = 10,  // 11h
	REASON_DISASSOC_SUPCHAN_BAD     = 11,  // 11h
	REASON_IE_INVALID               = 13,  // 11i
	REASON_MIC_FAILURE              = 14,  // 11i
	REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,  // 11i
	REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,  // 11i
	REASON_IE_IN_4WAY_DIFFERS       = 17,  // 11i
	REASON_GROUP_CIPHER_INVALID     = 18,  // 11i
	REASON_PAIRWISE_CIPHER_INVALID  = 19,  // 11i
	REASON_AKMP_INVALID             = 20,  // 11i
	REASON_UNSUPP_RSN_IE_VERSION    = 21,  // 11i
	REASON_INVALID_RSN_IE_CAP       = 22,  // 11i
	REASON_802_1X_AUTH_FAILED       = 23,  // 11i
	REASON_CIPHER_SUITE_REJECTED    = 24,  // 11i

	REASON_BEACON_TIMEOUT           = 200,
	REASON_NO_AP_FOUND              = 201,
	REASON_AUTH_FAIL				= 202,
	REASON_ASSOC_FAIL				= 203,
	REASON_HANDSHAKE_TIMEOUT		= 204,
	*/
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files

	pinMode(LED_SETUP_PIN, OUTPUT);
	pinMode(A0, INPUT);
	pinMode(RESET_PIN, INPUT_PULLUP);

	pinMode(14, OUTPUT); // Relay

	/*rcSwitch.enableTransmit(RCSWITCH_TX_PIN);

	while(1) {
		rcSwitch.switchOn("11111", "00010");
		delay(200);
		rcSwitch.switchOff("11111", "00010");
		delay(200);

		rcSwitch.switchOff("11111", "00011");
		delay(200);
		rcSwitch.switchOn("11111", "00011");
		delay(200);
	}*/

	for (uint8_t i = 1; i <= 6; i++) {
		digitalWrite(LED_SETUP_PIN, !digitalRead(LED_SETUP_PIN));
		delay(200);
	}

	if(fileExist(".lastModified")) {
		// The last modification
		lastModified = fileGetContent(".lastModified");
		lastModified.trim();
	}

	Serial.begin(115200); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	AppSettings.loadAll();

	/*
	AppSettings.rcswitch = true;
	AppSettings.rcswitch_topic = "Testtopic";
	Vector<String>temp;
	temp.addElement("11111");
	temp.addElement("00010");

	AppSettings.rcswitch_dev.add(temp);
	AppSettings.rcswitch_dev.add(temp);
	AppSettings.rcswitch_count = AppSettings.rcswitch_dev.size();
	AppSettings.rcswitch_dev[0][0] = "11111";
	AppSettings.rcswitch_dev[0][1] = "00010";
	AppSettings.rcswitch_dev[1][0] = "11111";
	AppSettings.rcswitch_dev[1][1] = "00001";
	AppSettings.savePeriph();
    */

	WifiStation.enable(true, false);
	WifiStation.startScan(networkScanCompleted);

	if (AppSettings.existGlobal() && (digitalRead(RESET_PIN) == 1))
	{
		if (AppSettings.ssid.length() > 0) {
			debugf("Settings found. Starting Station Mode");

			digitalWrite(LED_SETUP_PIN, 0); // Put off Setup LED Indicator

			WifiStation.config(AppSettings.ssid, AppSettings.password, true, false);
			if (!AppSettings.dhcp && !AppSettings.ip.isNull())
				WifiStation.setIP(AppSettings.ip, AppSettings.netmask, AppSettings.gateway);

			// Run our method when station was connected to AP (or not connected)
			// Set callback that should be triggered when we have assigned IP
			WifiEvents.onStationGotIP(connectOk);

			// Set callback that should be triggered if we are disconnected or connection attempt failed
			WifiEvents.onStationDisconnect(connectFail);
		} else {
			debugf("Settings found but SSID is empty! Starting Setup Mode");
			startAP();
		}
	} else if (AppSettings.existGlobal() && (digitalRead(RESET_PIN) == 0)) {
		debugf("Settings found but reset condition triggered!");
		debugf("Erasing App Settings...");
		if(fileExist(APP_GLOBAL_SETTINGS_FILE)) {
			fileDelete(APP_GLOBAL_SETTINGS_FILE);
			//TODO: Delete appropriate flash 128 bytes where wifi credentials are stored (0x7E000)
			//		ex. ./esptool.py read_flash 0x7E000 128 wifisettings.bin
		}
		if(fileExist(APP_MQTT_SETTINGS_FILE))
			fileDelete(APP_MQTT_SETTINGS_FILE);

		if(fileExist(APP_PERIPH_SETTINGS_FILE))
			fileDelete(APP_PERIPH_SETTINGS_FILE);

		debugf("done");
	} else {
		debugf("No Settings found. Starting Setup Mode");
		startAP();
	}

	// Run WEB server on system ready
	System.onReady(startServers);
}
