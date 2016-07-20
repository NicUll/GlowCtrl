#include "Arduino.h"
#include "WireButton.h"
#include "Wire.h"




//Added channel parameter for wire
WireButton::WireButton(int setWireChannel, unsigned long setToggleTime) {
	for (int i = 0; i < MAX_WIREBUTTONS; i++) {
		wireButtons[i].active = false;
		wireButtons[i].turned_on = false;
	}
	chaseGroupAmount = 0;
	wireChannel = setWireChannel;
	toggleTime = setToggleTime;
	currentSelected = -1;
	Wire.begin();

}

void WireButton::wireButtonSwitch(int button)
{
	if(wireButtons[button].type == CHASE_MASTER){
		if(currentSelected == wireButtons[button].armNum){
			wireButtons[button].mode = !wireButtons[button].mode;
		}else{
			currentSelected = wireButtons[button].armNum;
		}
		if(wireButtons[button].mode == MANUAL){
			Wire.beginTransmission(wireChannel);
			Wire.write(MANUAL);
			Wire.write(currentSelected);
			Wire.write(0);
			Wire.endTransmission(wireChannel);
		}else{

		}
	}
	else if(wireButtons[button].type == CHASE_MEMBER) {
		Wire.beginTransmission(wireChannel);
		Wire.write(CHASE);
		Wire.write(currentSelected);
		Wire.write(wireButtons[button].armNum);
		Wire.endTransmission(wireChannel);
	}
	else{
			currentSelected = wireButtons[button].armNum;
			Wire.beginTransmission(wireChannel);
			Wire.write(MANUAL);
			Wire.write(currentSelected);
			Wire.write(0);
			Wire.endTransmission(wireChannel);
	}
	wireButtons[button].turned_on = !(wireButtons[button].turned_on);
	//digitalWrite(wireButtons[button].pin, !wireButtons[button].turned_on);
}





int WireButton::addWireButton(int button, int armNum, int function, int type, word standardcolor, word altcolor)
{
	wireButtons[button].armNum = armNum;
	wireButtons[button].active = true;
	wireButtons[button].function = function;
	wireButtons[button].type = type;
	wireButtons[button].turned_on = false;
	wireButtons[button].altcolor = altcolor;
	wireButtons[button].standardcolor = standardcolor;
	wireButtons[button].mode = CHASE;
	if(type == CHASE_MASTER){
		wireButtons[button].chase_group = chaseGroupAmount;
		chaseGroupAmount += 1;
	}
	return button;
}

int WireButton::getChaseGroup(int button){
	return wireButtons[button].chase_group;
}

void WireButton::deleteWireButton(int button)
{
	wireButtons[button].active = false;
}
boolean WireButton::isWireButtonActive(int button)
{
	return wireButtons[button].active;
}

boolean WireButton::isWireButtonTurnedOn(int button)
{
	return wireButtons[button].turned_on;
}

int WireButton::getWireButtonFunction(int button)
{
	return wireButtons[button].function;
}

word WireButton::getAltColor(int button)
{
	return wireButtons[button].altcolor;
}

word WireButton::getStaColor(int button)
{
	return wireButtons[button].standardcolor;
}

int WireButton::getWireButtonType(int button)
{
	if(wireButtons[button].active == true) {
		return wireButtons[button].type;
	}else{
		return -1;
	}
}