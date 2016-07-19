#include "Arduino.h"
#include "Relay.h"




Relay::Relay(unsigned long setToggleTime)
{
	for (int i = 0; i < MAX_RELAYS; i++){
		relays[i].active = false;
		relays[i].turned_on = false;
	}
	toggleTime = setToggleTime;
}

int Relay::addRelay(int pin, int button, int function, word standardcolor, word altcolor)
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH);

	relays[button].pin = pin;
	relays[button].active = true;
	relays[button].function = function;
	relays[button].turned_on = false;
	relays[button].altcolor = altcolor;
	relays[button].standardcolor = standardcolor;

	return button;
}

void Relay::deleteRelay(int button)
{
	relays[button].active = false;
}

void Relay::relaySwitch(int button)
{
	relays[button].turned_on = !(relays[button].turned_on);
	digitalWrite(relays[button].pin, !relays[button].turned_on);
}


boolean Relay::isRelayActive(int button)
{
	return relays[button].active;
}

boolean Relay::isRelayTurnedOn(int button)
{
	return relays[button].turned_on;
}

int Relay::relayFunction(int button)
{
	return relays[button].function;
}

word Relay::getAltColor(int button)
{
	return relays[button].altcolor;
}

word Relay::getStaColor(int button)
{
	return relays[button].standardcolor;
}