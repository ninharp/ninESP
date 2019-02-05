/**
 * Project: ninHOME_Node
 * @file webinterface.cpp
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * Webinterface dependend functions
 *
 */

#include "application.h"
#include "webinterface.h"

/* Web and FTP Server instance */
HttpServer server;
FTPServer ftp;

/* Timer for make connection ajax callback */
Timer connectionTimer;

const char* checked_str = "checked='checked'";

/* Contains list of available wifi networks after populating */
BssList wNetworks;

/* Current network and password for wifi connection, used in ajax callback */
String wNetwork, wPassword;

/* Last modified string for webinterface */
String lastModified;

//extern MqttClient *mqtt;

void startWebServer();
void startFTP();

/* Index target for webserver */
void onIndex(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();
	vars["lastedit"] = lastModified;

	vars["status"] = getStatusString();
	vars["version"] = app_version;

	response.sendTemplate(tmpl); // will be automatically deleted
}

/* Update target for webserver */
void onUpdate(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("update.html");
	//rBootUpdateItem add;
	//add.targetOffset = RBOOT_SPIFFS_0;
	//add.size = 0;
	//rboot_write_status rBootWriteStatus;
	//rBootWriteStatus = rboot_write_init( add.targetOffset );


	//if(!rboot_write_flash(&rBootWriteStatus, (uint8_t *)data, size)) {
	//	debugf("rboot_write_flash: Failed. Size: %d", size);
	//}

	auto &vars = tmpl->variables();
	vars["lastedit"] = lastModified;

	vars["status"] = getStatusString();
	vars["version"] = app_version;

	response.sendTemplate(tmpl); // will be automatically deleted
}

/* On Reboot target for webserver */
void onReboot(HttpRequest &request, HttpResponse &response)
{
	debugf("Restarting...");
	/* Disable Watchdog */
	//WDT.enable(true);
	/* Restart System */
	System.restart();
}

/* Network config target for webserver */
void onConfig(HttpRequest &request, HttpResponse &response)
{
	/* If values are passed from browser we will set here */
	if (request.method == HTTP_POST)
	{
		AppSettings.status_led_pin = request.getPostParameter("statuspin").toInt();
		AppSettings.status_led_inv = request.getPostParameter("statusinv").equals("on") ? true : false;

		AppSettings.dhcp = request.getPostParameter("dhcp") == "1";
		AppSettings.ip = request.getPostParameter("ip");
		AppSettings.netmask = request.getPostParameter("netmask");
		AppSettings.gateway = request.getPostParameter("gateway");

		AppSettings.udp = request.getPostParameter("udp").equals("on") ? true : false;;
		AppSettings.udp_port = request.getPostParameter("udpport").toInt();

		//debugf("Updating IP settings: %d", AppSettings.ip.isNull());
		AppSettings.saveNetwork();
	}

	AppSettings.loadNetwork();

	TemplateFileStream *tmpl = new TemplateFileStream("settings.html");
	auto &vars = tmpl->variables();

	vars["statuspin"] = String(AppSettings.status_led_pin);
	vars["statusinv"] = AppSettings.status_led_inv ? checked_str : "";

	bool dhcp = WifiStation.isEnabledDHCP();
	vars["dhcpon"] = dhcp ? checked_str : "";
	vars["dhcpoff"] = !dhcp ? checked_str : "";

	vars["udpon"] = AppSettings.udp ? checked_str : "";
	vars["udpport"] = String(AppSettings.udp_port);

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

	vars["lastedit"] = lastModified;

	vars["status"] = getStatusString();
	vars["version"] = app_version;

	response.sendTemplate(tmpl); // will be automatically deleted
}

/* Peripheral config target for webserver */
void onPeriphConfig(HttpRequest &request, HttpResponse &response)
{
	/* If values are passed from browser we will set here */
	if (request.method == HTTP_POST)
	{
		AppSettings.timer_delay = String(request.getPostParameter("timer_delay")).toInt();

		/* Relay Settings */
		AppSettings.relay = request.getPostParameter("relay").equals("on") ? true : false;
		AppSettings.relay_pin = String(request.getPostParameter("relay_pin")).toInt();
		AppSettings.relay_invert = request.getPostParameter("relay_invert").equals("on") ? true : false;
		AppSettings.relay_status_pin = String(request.getPostParameter("status_pin")).toInt();
		AppSettings.relay_status_invert = request.getPostParameter("status_invert").equals("on") ? true : false;
		AppSettings.relay_topic_pub = request.getPostParameter("topic_relay_pub");
		AppSettings.relay_topic_cmd = request.getPostParameter("topic_relay_cmd");
		AppSettings.keyinput = request.getPostParameter("keyinput").equals("on") ? true : false;
		AppSettings.keyinput_invert = request.getPostParameter("keyinput_invert").equals("on") ? true : false;
		AppSettings.keyinput_pin = String(request.getPostParameter("keyinput_pin")).toInt();
		AppSettings.keyinput_debounce = String(request.getPostParameter("keyinput_debounce")).toInt();

		/* ADC Settings */
		AppSettings.adc = request.getPostParameter("adc").equals("on") ? true : false;
		AppSettings.adc_pub = request.getPostParameter("sensor_adc_pub").equals("1") ? true : false;
		AppSettings.adc_topic = request.getPostParameter("topic_adc_pub");

		/* RCSwitch Settings */
		AppSettings.rcswitch = request.getPostParameter("rcswitch").equals("on") ? true : false;
		AppSettings.rcswitch_topic_prefix = request.getPostParameter("topic_rcswitch");
		AppSettings.rcswitch_pin = String(request.getPostParameter("rcswitch_pin")).toInt();
		//TODO RCSWitch Save devices aswell

		/* MAX7219 Display Settings */
		AppSettings.max7219 = request.getPostParameter("max7219").equals("on") ? true : false;
		AppSettings.max7219_count = String(request.getPostParameter("max7219_count")).toInt();
		AppSettings.max7219_ss_pin = String(request.getPostParameter("max7219_pin")).toInt();
		AppSettings.max7219_topic_prefix = request.getPostParameter("max7219_prefix");
		AppSettings.max7219_topic_enable = request.getPostParameter("max7219_enable");
		AppSettings.max7219_topic_text = request.getPostParameter("max7219_text");
		AppSettings.max7219_topic_scroll = request.getPostParameter("max7219_scroll");
		AppSettings.max7219_topic_speed = request.getPostParameter("max7219_speed");
		AppSettings.max7219_topic_charwidth = request.getPostParameter("max7219_char");
		AppSettings.max7219_topic_intensity = request.getPostParameter("max7219_int");
		AppSettings.max7219_topic_invert = request.getPostParameter("max7219_inv");
		AppSettings.max7219_topic_alignment = request.getPostParameter("max7219_align");
		AppSettings.max7219_topic_pause = request.getPostParameter("max7219_pause");
		AppSettings.max7219_topic_effect_in = request.getPostParameter("max7219_ein");
		AppSettings.max7219_topic_effect_out = request.getPostParameter("max7219_eout");
		AppSettings.max7219_topic_reset = request.getPostParameter("max7219_reset");

		/* Motion Sensor Settings */
		AppSettings.motion = request.getPostParameter("motion").equals("on") ? true : false;
		AppSettings.motion_invert = request.getPostParameter("motion_invert").equals("on") ? true : false;
		AppSettings.motion_pin = request.getPostParameter("motion_pin").toInt();
		AppSettings.motion_interval = request.getPostParameter("motion_interval").toInt();
		AppSettings.motion_topic = request.getPostParameter("topic_motion");

		//debugf("Updating Peripheral settings");

		/* Save peripheral config */
		AppSettings.savePeriph();

		/* Start services again */
		startServices();

		/* Set interval of sensor Publishing if timer was started before */
		/*if (sensorPublishTimer.isStarted()) {
			sensorPublishTimer.setIntervalMs(AppSettings.timer_delay);
		} else {
			if (AppSettings.adc) {
				sensorPublishTimer.setIntervalMs(AppSettings.timer_delay);
				sensorPublishTimer.start();
			}
		}*/

		/* Set interval of motion sensing sensor check if timer was started before */
		/*if (motionCheckTimer.isStarted()) {
			motionCheckTimer.setIntervalMs(AppSettings.motion_interval);
		} else {
			if (AppSettings.motion) {
				motionCheckTimer.initializeMs(AppSettings.motion_interval, motionSensorCheck).start();
			}
		}*/
	}

	AppSettings.loadPeriph();

	TemplateFileStream *tmpl = new TemplateFileStream("periph.html");
	auto &vars = tmpl->variables();

	vars["timer_delay"] = String(AppSettings.timer_delay);

	/* Relay Settings */
	vars["relay_on"] = AppSettings.relay ? checked_str : "";
	vars["relay_pin"] = AppSettings.relay_pin;
	vars["relay_invert"] = AppSettings.relay_invert ? checked_str : "";
	vars["status_pin"] = AppSettings.relay_status_pin;
	vars["status_invert"] = AppSettings.relay_status_invert ? checked_str : "";
	vars["topic_relay_cmd"] = AppSettings.relay_topic_cmd;
	vars["topic_relay_pub"] = AppSettings.relay_topic_pub;
	vars["keyinput_on"] = AppSettings.keyinput ? checked_str : "";
	vars["keyinput_pin"] = AppSettings.keyinput_pin;
	vars["key_debounce"] = AppSettings.keyinput_debounce;
	vars["keyinput_invert"] = AppSettings.keyinput_invert ? checked_str : "";

	/* ADC Settings */
	vars["adc_on"] = AppSettings.adc ? checked_str : "";
	vars["topic_adc"] = AppSettings.adc_topic;
	vars["adc_pub_on"] = AppSettings.adc_pub ? checked_str : "";
	vars["adc_pub_off"] = !AppSettings.adc_pub ? checked_str : "";

	/* RCSwitch Settings */
	vars["rcswitch_on"] = AppSettings.rcswitch ? checked_str : "";
	vars["topic_rcswitch"] = AppSettings.rcswitch_topic_prefix;
	vars["rcswitch_pin"] = AppSettings.rcswitch_pin;

	/* MAX7219 Settings */
	vars["max7219_on"] = AppSettings.max7219 ? checked_str : "";
	vars["max7219_count"] = AppSettings.max7219_count;
	vars["max7219_pin"] = AppSettings.max7219_ss_pin;
	vars["max7219_prefix"] = AppSettings.max7219_topic_prefix;
	vars["max7219_enable"] = AppSettings.max7219_topic_enable;
	vars["max7219_text"] = AppSettings.max7219_topic_text;
	vars["max7219_scroll"] = AppSettings.max7219_topic_scroll;
	vars["max7219_speed"] = AppSettings.max7219_topic_speed;
	vars["max7219_char"] = AppSettings.max7219_topic_charwidth;
	vars["max7219_int"] = AppSettings.max7219_topic_intensity;
	vars["max7219_inv"] = AppSettings.max7219_topic_invert;
	vars["max7219_align"] = AppSettings.max7219_topic_alignment;
	vars["max7219_pause"] = AppSettings.max7219_topic_pause;
	vars["max7219_ein"] = AppSettings.max7219_topic_effect_in;
	vars["max7219_eout"] = AppSettings.max7219_topic_effect_out;
	vars["max7219_reset"] = AppSettings.max7219_topic_reset;

	/* Motion Sensor Settings */
	vars["motion_on"] = AppSettings.motion ? checked_str : "";
	vars["motion_pin"] = AppSettings.motion_pin;
	vars["motion_invert"] = AppSettings.motion_invert ? checked_str : "";
	vars["topic_motion"] = AppSettings.motion_topic;
	vars["motion_interval"] = AppSettings.motion_interval;

	vars["lastedit"] = lastModified;

	vars["status"] = getStatusString();
	vars["version"] = app_version;

	response.sendTemplate(tmpl); // will be automatically deleted
}

/* MQTT Config target for Webserver */
void onMQTTConfig(HttpRequest &request, HttpResponse &response)
{
	//TODO: Message in webinterface, mqtt will enabled after first save
	/* If values are passed from browser we will set here */
	if (request.method == HTTP_POST)
	{
		AppSettings.mqtt_server = request.getPostParameter("server");
		AppSettings.mqtt_port = request.getPostParameter("port").toInt();
		AppSettings.mqtt_userid = request.getPostParameter("userid");
		AppSettings.mqtt_login = request.getPostParameter("login");
		AppSettings.mqtt_password = request.getPostParameter("password");
		AppSettings.mqtt_topic_lwt = request.getPostParameter("topic_lwt");
		AppSettings.mqtt_topic_cmd = request.getPostParameter("topic_cmd");
		AppSettings.mqtt_topic_pub = request.getPostParameter("topic_pub");
		//debugf("Updating MQTT settings: %d", AppSettings.mqtt_server.length());
		AppSettings.saveMQTT();
	}

	AppSettings.loadMQTT();

	TemplateFileStream *tmpl = new TemplateFileStream("mqtt.html");
	auto &vars = tmpl->variables();

	if (AppSettings.mqtt_server.length() <= 0) { vars["server"] = DEFAULT_MQTT_SERVER; } else { vars["server"] = AppSettings.mqtt_server; }
	if (AppSettings.mqtt_port > 1024) { vars["port"] = DEFAULT_MQTT_PORT; } else { vars["port"] = AppSettings.mqtt_port; }
	if (AppSettings.mqtt_login.length() <= 0) { vars["login"] = DEFAULT_MQTT_LOGIN; } else { vars["login"] = AppSettings.mqtt_login; }
	if (AppSettings.mqtt_userid.length() <= 0) { vars["userid"] = DEFAULT_MQTT_USERID; } else { vars["userid"] = AppSettings.mqtt_userid; }
	if (AppSettings.mqtt_password.length() <= 0) { vars["password"] = DEFAULT_MQTT_PASS; } else { vars["password"] = AppSettings.mqtt_password; }
	if (AppSettings.mqtt_topic_lwt.length() <= 0) { vars["topic_lwt"] = DEFAULT_MQTT_LWT; } else { vars["topic_lwt"] = AppSettings.mqtt_topic_lwt; }
	if (AppSettings.mqtt_topic_cmd.length() <= 0) { vars["topic_cmd"] = DEFAULT_MQTT_CMD; } else { vars["topic_cmd"] = AppSettings.mqtt_topic_cmd; }
	if (AppSettings.mqtt_topic_pub.length() <= 0) { vars["topic_pub"] = DEFAULT_MQTT_PUB; } else { vars["topic_pub"] = AppSettings.mqtt_topic_pub; }

	vars["lastedit"] = lastModified;

	vars["status"] = getStatusString();
	vars["version"] = app_version;

	response.sendTemplate(tmpl); // will be automatically deleted
}

/* Default target for Webserver */
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

/* Debugging configuration values, dumps out the whole JSON config file which is currently active */
/* Syntax: /dumpcfg?type=<type>   where type is the type of the config file (network, mqtt, periph) */
void onDumpConfig(HttpRequest &request, HttpResponse &response)
{
	if (request.method == HTTP_GET) {
		String type = request.getQueryParameter("type");
		if (type.equals("network")) {
			int size = fileGetSize(APP_GLOBAL_SETTINGS_FILE);
			char* cfgString = new char[size + 1];
			fileGetContent(APP_GLOBAL_SETTINGS_FILE, cfgString, size + 1);

			response.sendString(cfgString);
		} else if (type.equals("mqtt")) {
			int size = fileGetSize(APP_MQTT_SETTINGS_FILE);
			char* cfgString = new char[size + 1];
			fileGetContent(APP_MQTT_SETTINGS_FILE, cfgString, size + 1);

			response.sendString(cfgString);
			delete[] cfgString;
		} else if (type.equals("periph")) {
			int size = fileGetSize(APP_PERIPH_SETTINGS_FILE);
			char* cfgString = new char[size + 1];
			fileGetContent(APP_PERIPH_SETTINGS_FILE, cfgString, size + 1);

			response.sendString(cfgString);
			delete[] cfgString;
		}
	} else {
		int size1 = fileGetSize(APP_GLOBAL_SETTINGS_FILE);
		int size2 = fileGetSize(APP_MQTT_SETTINGS_FILE);
		int size3 = fileGetSize(APP_PERIPH_SETTINGS_FILE);
		char* cfgString1 = new char[size1 + 1];
		char* cfgString2 = new char[size2 + 1];
		char* cfgString3 = new char[size3 + 1];
		String delimiter("\r\n\r\n-\r\n\r\n");
		fileGetContent(APP_GLOBAL_SETTINGS_FILE, cfgString1, size1 + 1);
		fileGetContent(APP_MQTT_SETTINGS_FILE, cfgString2, size2 + 1);
		fileGetContent(APP_PERIPH_SETTINGS_FILE, cfgString3, size3 + 1);
		String temp = String(cfgString1) + delimiter + String(cfgString2) + delimiter + String(cfgString3);
		response.sendString(temp);
		delete[] cfgString1;
		delete[] cfgString2;
		delete[] cfgString3;
	}
}

/* Ajax target on Network List receive */
void onAjaxNetworkList(HttpRequest &request, HttpResponse &response)
{
	/* Create JSON stream */
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	json["status"] = (bool)true;

	/* Get connection state of wifi station */
	bool connected = WifiStation.isConnected();
	json["connected"] = connected;
	if (connected)
	{
		/* Copy full string to JSON buffer memory */
		json["network"]= WifiStation.getSSID();
	}

	JsonArray& netlist = json.createNestedArray("available");
	for (int i = 0; i < wNetworks.count(); i++)
	{
		if (wNetworks[i].hidden) continue;
		JsonObject &item = netlist.createNestedObject();
		item["id"] = (int)wNetworks[i].getHashId();
		/* Copy full string to JSON buffer memory */
		item["title"] = wNetworks[i].ssid;
		item["signal"] = wNetworks[i].rssi;
		item["encryption"] = wNetworks[i].getAuthorizationMethodName();
	}

	response.setAllowCrossDomainOrigin("*");

	/* Send back JSON created network list to our ajax script */
	response.sendDataStream(stream, MIME_JSON);
}


//Start connection triggered from ajax service
void makeConnection()
{
	// Enable wifistation if not already enabled
	WifiStation.enable(true);

	// Set config for wifistation from values
	WifiStation.config(wNetwork, wPassword, true, false);
	// Run our method when station was connected to AP (or not connected)
	// Set callback that should be triggered when we have assigned IP
	WifiEvents.onStationGotIP(connectOk);

	// Set callback that should be triggered if we are disconnected or connection attempt failed
	WifiEvents.onStationDisconnect(connectFail);

	// Trigger wifistation connection manual
	WifiStation.connect();

	// Write values from connection to appsettings and save to config file
	AppSettings.ssid = wNetwork;
	AppSettings.password = wPassword;
	// Save all config files from values of AppSettings
	AppSettings.saveNetwork();
	//debugf("Making Connection");

	delay(3000);

	// Set network value back to "" for task completion indication
	wNetwork = "";
}

// Ajax target to connect to wifi network
void onAjaxConnect(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	String curNet = request.getPostParameter("network");
	String curPass = request.getPostParameter("password");

	bool updating = curNet.length() > 0 && (WifiStation.getSSID() != curNet || WifiStation.getPassword() != curPass);
	bool connectingNow = WifiStation.getConnectionStatus() == eSCS_Connecting || wNetwork.length() > 0;

	if (updating && connectingNow)
	{
		debugf("wrong action: %s %s, (updating: %d, connectingNow: %d)", wNetwork.c_str(), wPassword.c_str(), updating, connectingNow);
		json["status"] = (bool)false;
		json["connected"] = (bool)false;
	}
	else
	{
		json["status"] = (bool)true;
		if (updating)
		{
			wNetwork = curNet;
			wPassword = curPass;
			//debugf("CONNECT TO: %s %s", network.c_str(), password.c_str());
			json["connected"] = false;
			connectionTimer.initializeMs(1200, makeConnection).startOnce();
		}
		else
		{
			json["connected"] = WifiStation.isConnected();
			//debugf("Network already selected. Current status: %s", WifiStation.getConnectionStatusName());
		}
	}

	if (!updating && !connectingNow && WifiStation.isConnectionFailed())
		json["error"] = WifiStation.getConnectionStatusName();

	response.setAllowCrossDomainOrigin("*");
	response.sendDataStream(stream, MIME_JSON);
}

/* Will be called when system initialization was completed */
void startWebinterface()
{
	startFTP();
	startWebServer();
}

/* Starts the implemented Webserver and add the used targets */
void startWebServer()
{
	/* Listen on Port */
	//TODO: Webserver port in configuration file?
	if (!server.listen(80)) {
		debugf("Cannot start Webserver");
		return;
	} else {
		debugf("Webserver started!");
	}

	/* Check for existing index.html, else create a empty one with error message */
	if (!fileExist("index.html"))
		fileSetContent("index.html", "<h3>No SPIFFS found! Contact supplier!</h3>");

	/* Add targets to server */
	server.addPath("/", onIndex);
	server.addPath("/reboot", onReboot);
	server.addPath("/config", onConfig);
	server.addPath("/periph", onPeriphConfig);
	server.addPath("/dumpcfg", onDumpConfig);
	server.addPath("/mqttconfig", onMQTTConfig);
	server.addPath("/ajax/get-networks", onAjaxNetworkList);
	server.addPath("/ajax/connect", onAjaxConnect);

	/* Any other targets */
	server.setDefaultHandler(onFile);
}

/* Starts the FTP Server with fixed credentials, just for debugging purposes, had to be disabled in production state */
void startFTP()
{
	/* Start FTP server instance */
	ftp.listen(DEFAULT_FTP_PORT);

	/* Add fixed credention ftp user account */
	ftp.addUser(DEFAULT_FTP_USER, DEFAULT_FTP_PASS); // FTP account
}

String getStatusString()
{
	String status;
	if (WifiAccessPoint.isEnabled())
		status += "Access Point ";
	if (WifiStation.isEnabled()) {
		if (WifiAccessPoint.isEnabled())
			status += "& ";
		status += "Station ";
	}
	status += "Mode - ";
	if (WifiStation.isEnabled() && WifiStation.isConnected()) {
		IPAddress ip = WifiStation.getIP();
		String ssid = WifiStation.getSSID();
		status += "Connected to " + ssid + " with IP " + ip.toString();
	}
	if (WifiAccessPoint.isEnabled()) {
		IPAddress ip = WifiAccessPoint.getIP();
		String ssid = WifiAccessPoint.getSSID();
		if (WifiStation.isEnabled())
			status += " & ";
		status += "Wifi AP '" + ssid + "' on IP " + ip.toString();
	}
	return status;
}




