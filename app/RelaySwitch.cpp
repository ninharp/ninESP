/**
 * Project: ninHOME_Node
 * @file RelaySwitch.cpp
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 * @date 30.11.2017
 *
 * Class file for RelaySwitch
 *
 */

#include "RelaySwitch.h"
#include <SmingCore/SmingCore.h>

RelaySwitch::RelaySwitch()
{
	state = false;
	pin = -1;
	status_pin = -1;
}

void RelaySwitch::init(int8_t pin, int8_t status_pin, bool inverted, bool status_inverted, bool defState)
{
	pinMode(pin, OUTPUT);
	this->status_pin = status_pin;
	this->status_inverted = status_inverted;
	if (status_pin > -1) {
		pinMode(status_pin, OUTPUT);
	}
	this->pin = pin;
	this->inverted = inverted;
	if (inverted)
		digitalWrite(pin, !defState);
	else
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
		this->state = true;
		if (inverted)
			digitalWrite(pin, 0);
		else
			digitalWrite(pin, 1);

		if (status_pin > -1) {
			if (status_inverted)
				digitalWrite(status_pin, 0);
			else
				digitalWrite(status_pin, 1);
		}
	} else {
		debugf("Relay OFF");
		this->state = false;
		if (inverted)
			digitalWrite(pin, 1);
		else
			digitalWrite(pin, 0);

		if (status_pin > -1) {
			if (status_inverted)
				digitalWrite(status_pin, 1);
			else
				digitalWrite(status_pin, 0);
		}
	}
}

void RelaySwitch::toggle(void)
{
	digitalWrite(pin, !state);
	if (status_pin > -1) {
		digitalWrite(status_pin, !state);
	}
}

