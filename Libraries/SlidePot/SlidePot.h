#ifndef SlidePot_h
#define SlidePot_h

#include "Arduino.h"

#define MAX_SLIDES 20

typedef struct
{
  int pin;
  boolean active;
  int value;
  boolean DMX;
} slide_type;

class SlidePot
{
  public:
	SlidePot();
	
	int addSlide(int slide, int pin, boolean DMX=false);
	void deleteSlide(int slide);
	boolean isSlideActive(int slide);
	int getSlideValue(int slide);
  private:
	slide_type slides[MAX_SLIDES];
};
#endif

