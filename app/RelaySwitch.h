/*
 * RelaySwitch.h
 *
 *  Created on: 30.11.2017
 *      Author: Michael Sauer
 */

#include <SmingCore/SmingCore.h>

#ifndef APP_RELAYSWITCH_H_
#define APP_RELAYSWITCH_H_

class RelaySwitch {
public:
	RelaySwitch();
	void set(bool state);
	void toggle(void);
	bool get(void);
	void init(int8_t pin, int8_t status_pin, bool inverted, bool defState);

protected:
	bool state;
	bool inverted;
private:
	int8_t pin;
	int8_t status_pin;
};

#endif /* APP_RELAYSWITCH_H_ */
