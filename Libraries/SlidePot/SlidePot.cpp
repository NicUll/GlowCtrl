#include "Arduino.h"
#include "SlidePot.h"

SlidePot::SlidePot()
{
  for(int i=0; i<MAX_SLIDES; i++){
    slides[i].active = false;
  }
}

int SlidePot::addSlide(int slide, int pin, boolean DMX){
  slides[slide].active = true;
  slides[slide].pin = pin;
  slides[slide].DMX = DMX;
  return slide;
}

void SlidePot::deleteSlide(int slide){
  slides[slide].active = false;
}

boolean SlidePot::isSlideActive(int slide){
  return slides[slide].active;
}

int SlidePot::getSlideValue(int slide){
	int slideValue = analogRead(slides[slide].pin);
	if(slides[slide].DMX){
		slideValue = slideValue >> 2;
	}
	return slideValue;
}

