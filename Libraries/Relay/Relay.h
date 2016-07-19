/** Relay library by Nic Ullman,
	to be used with modified version of UTFT_Buttons.
	Version 1.0 */


#ifndef Relay_h
#define Relay_h


#include "Arduino.h"
//#include <UTFT_BUTTON.h>

//Colors
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

#define FLASH	0
#define FULL_ON	1
#define	TOGGLE	2

#define MAX_RELAYS 48

typedef struct
{
	int		function; //FULL_ON, FLASH or TOGGLE
	int		pin;    //Which pin/relay to link to
	boolean	turned_on; //If the relay is currently on
	boolean active; //If the relay is in use
	word standardcolor;
	word altcolor; //The toggle color
}	relay_type;		

class Relay
{
	public:
		Relay(unsigned long setToggleTime);
		
		int addRelay(int pin, int button, int function, word standardcolor=VGA_WHITE, word altcolor=VGA_GREEN);
		void deleteRelay(int button);
		void relaySwitch(int pressed);
		boolean isRelayActive(int button);
		boolean isRelayTurnedOn(int button);
		int relayFunction(int button);
		unsigned long toggleTime;
		word getAltColor(int button);
		word getStaColor(int button);
	private:
		relay_type	relays[MAX_RELAYS];
};
#endif
