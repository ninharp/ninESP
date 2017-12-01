/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/anakod/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Modifications for ninHOME by Michael Sauer, 2017
 *
 ****/

/** @defgroup   mqttclient MQTT client
 *  @brief      Provides MQTT client
 *  @ingroup    tcpclient
 *  @{
 */

#ifndef _SMING_CORE_NETWORK_ninMqttClient_H_
#define _SMING_CORE_NETWORK_ninMqttClient_H_

#define MQTT_MAX_BUFFER_SIZE 1024

#include <SmingCore/Network/TcpClient.h>
#include <SmingCore/Delegate.h>
#include <Wiring/WString.h>
#include <Wiring/WHashMap.h>
#include <Services/libemqtt/libemqtt.h>

//typedef void (*MqttStringSubscriptionCallback)(String topic, String message);
typedef Delegate<void(String topic, String message)> ninMqttStringSubscriptionCallback;
typedef Delegate<void(uint16_t msgId, int type)> ninMqttMessageDeliveredCallback;

class ninMqttClient;
class URL;

class ninMqttClient: protected TcpClient
{
public:
	ninMqttClient(String serverHost, int serverPort, ninMqttStringSubscriptionCallback callback = NULL);
	ninMqttClient(IPAddress serverIp, int serverPort, ninMqttStringSubscriptionCallback callback = NULL);
	virtual ~ninMqttClient();

	void setKeepAlive(int seconds);			//send to broker
	void setPingRepeatTime(int seconds);            //used by client
	// Sets Last Will and Testament
	bool setWill(const String& topic, const String& message, int QoS, bool retained = false);

	bool connect(const String& clientName, boolean useSsl = false, uint32_t sslOptions = 0);
	bool connect(const String& clientName,const String& username, const String& password, boolean useSsl = false, uint32_t sslOptions = 0);

	using TcpClient::setCompleteDelegate;

	__forceinline bool isProcessing()  { return TcpClient::isProcessing(); }
	__forceinline TcpClientState getConnectionState() { return TcpClient::getConnectionState(); }

	bool publish(String topic, String message, bool retained = false);
	bool publishWithQoS(String topic, String message, int QoS, bool retained = false, ninMqttMessageDeliveredCallback onDelivery = NULL);

	bool subscribe(const String& topic);
	bool unsubscribe(const String& topic);

	void setHost(String host);
	void setPort(char* port);
	void setPort(String port);

#ifdef ENABLE_SSL
	using TcpClient::addSslOptions;
	using TcpClient::setSslFingerprint;
	using TcpClient::pinCertificate;
	using TcpClient::setSslClientKeyCert;
	using TcpClient::freeSslClientKeyCert;
	using TcpClient::getSsl;
#endif

protected:
	virtual err_t onReceive(pbuf *buf);
	virtual void onReadyToSendData(TcpConnectionEvent sourceEvent);
	void debugPrintResponseType(int type, int len);
	static int staticSendPacket(void* userInfo, const void* buf, unsigned int count);

private:
	String server;
	IPAddress serverIp;
	int port;
	mqtt_broker_handle_t broker;
	int waitingSize;
	uint8_t buffer[MQTT_MAX_BUFFER_SIZE + 1];
	uint8_t *current;
	int posHeader;
	ninMqttStringSubscriptionCallback callback;
	int keepAlive = 60;
	int PingRepeatTime = 20;
	unsigned long lastMessage = 0;
	HashMap<uint16_t, ninMqttMessageDeliveredCallback> onDeliveryQueue;
};

/** @} */
#endif /* _SMING_CORE_NETWORK_ninMqttClient_H_ */
