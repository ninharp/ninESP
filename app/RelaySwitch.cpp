/*
 * RelaySwitch.cpp
 *
 *  Created on: 30.11.2017
 *      Author: Michael Sauer
 */

#include "RelaySwitch.h"
#include <SmingCore/SmingCore.h>

RelaySwitch::RelaySwitch()
{
	state = false;
	pin = -1;
}

void RelaySwitch::init(int8_t pin, bool defState = false)
{
	pinMode(pin, OUTPUT);
	this->pin = pin;
	digitalWrite(pin, defState);
}

bool RelaySwitch::get(void)
{
	return state;
}

void RelaySwitch::set(bool state)
{
	if (state) {
		debugf("Relay ON");
		state = true;
		digitalWrite(pin, 0);
	} else {
		debugf("Relay OFF");
		state = false;
		digitalWrite(pin, 1);
	}
}

void RelaySwitch::toggle(void)
{
	digitalWrite(pin, !state);
	//mqtt.publish(AppSettings.relay_topic_pub, "on", true);
}

