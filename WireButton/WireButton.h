/** WireButton library by Nic Ullman,
	to be used with modified version of UTFT_Buttons.
	Version 1.0 */


#ifndef WireButton_h
#define WireButton_h


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

#define CHASE_MASTER 0
#define CHASE_MEMBER 1
#define SLIDE_ONLY 2

#define CHASE false
#define MANUAL true

#define MAX_WIREBUTTONS 48

typedef struct
{
	int		function; //FULL_ON, FLASH or TOGGLE
	int		armNum;    //Armature number to send in wire
	int type; //The type of wirebutton
	int mode;
	int chase_group;
	boolean	turned_on; //If the WireButton is currently on
	boolean active; //If the WireButton is in use
	word standardcolor;
	word altcolor; //The toggle color
}	WireButton_type;		

class WireButton
{
	public:
		WireButton(int wireChannel, unsigned long setToggleTime);
		
		int addWireButton(int button, int armNum, int function, int type, word standardcolor=VGA_WHITE, word altcolor=VGA_GREEN);
		void deleteWireButton(int button);
		void wireButtonSwitch(int pressed);
		boolean isWireButtonActive(int button);
		boolean isWireButtonTurnedOn(int button);
		int getWireButtonFunction(int button);
		int getWireButtonType(int button);
		int getChaseGroup(int button);
		int wireChannel;
		unsigned long toggleTime;
		int currentSelected;
		word getAltColor(int button);
		word getStaColor(int button);
		int chaseGroupAmount;
	private:
		WireButton_type	wireButtons[MAX_WIREBUTTONS];
};
#endif
