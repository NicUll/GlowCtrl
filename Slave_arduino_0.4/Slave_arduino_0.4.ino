//DMX bibliotek
#include <Rdm_Defines.h> 
#include <Rdm_Uid.h>
#include <Conceptinetics.h>

//Bibliotek för kommunikation mellan Arduino
#include <Wire.h>

//Sliders bibliotek
#include "SlidePot.h"

//Antal aktiva DMX kanaler, öka om fler läggs till
#define DMX_MASTER_CHANNELS   26 
#define DMX_MASTER_SLIDES     5
//Pinne på Arduino som kommunicerar med DMX-shield, ändra ej.
#define RXEN_PIN                2

//Pin miken är kopplad på
const int soundPin = A0;
//Mikens känslighet
const int tresh = 650;
//Global variabel för att läsa av ljudet
int soundLevel = 0;

//Vilket ljusläge, Chase eller Sliders
volatile int slide_mode = 0;
//Global variabel för att hålla kolla på aktuell chase
volatile int laserChase = 0;
volatile int rgbChase = 0;
volatile int ballChase = 0;

//Skapar ett SlidePot objekt
SlidePot mySlides;

//Allokerar plats för slides i minnet
int slide1;
int slide2;
int slide3;
int slide4;
int slide5;
int slide6;

//Global variabel för att hålla koll på aktuell kanal att skriva till
volatile int channel = 1;
volatile int channel1 = 1;
volatile int channel2 = 1;
volatile int channel3 = 1;
volatile int channel4 = 1;

volatile int channel_preset = 0;

int active_chase = 0;

volatile int phys_button = 0;

//Globala variabler för att hålla koll på värden i chasear
int turn = 0;
int direc = 1;
int delayTime;
int cycle = 1;

//Håller koll på hur många kanaler som är RGB-slingor
int RGB_CHANNELS = 12;
int RGB_UNITS = RGB_CHANNELS/3;

//Ser till att fade-chasearna avbryts om vi byter
volatile boolean stopChase = false;

//Skapar ett DMX-objekt
DMX_Master        dmx_master ( DMX_MASTER_CHANNELS, RXEN_PIN );


void setup() //Körs en gång i början
{
  pinMode(soundPin, INPUT); //Sätter ljudingången till en INGÅNG
  dmx_master.enable (); //Startar DMX 

  slide1 = mySlides.addSlide(1, A1, true); //Skapar tre slide-objekt,
  slide2 = mySlides.addSlide(2, A2, true); //Tredje parametern avgör om det ska vara en DMX slide eller inte, standard = false
  slide3 = mySlides.addSlide(3, A3, true); //Är det en dmx slide så görs värdena om från 1023 till 255 osv.
  slide4 = mySlides.addSlide(4, A4, true);
  slide5 = mySlides.addSlide(5, A5, true);
  slide6 = mySlides.addSlide(6, A5);
  
  Wire.begin(4);                // join i2c bus with address #4, ger Arduinon enhetsvärde fyra, så den lyssnar enbart då det skickas till #4 från master
  Wire.onReceive(receiveEvent); // register event, vart den ska gå om den får data från mastern

  pinMode(A15, INPUT);
  attachInterrupt(4, butRead, FALLING);
  
}



boolean soundTrigger(){
  soundLevel = analogRead(soundPin); //Läser av ljudnivån, kan med fördel bara göras en gång istället för 
  if(soundLevel > tresh){
    return true; 
  } else{
    return false;
  }
}

void changePreset(int newPreset){
  switch(newPreset){
    case 8:
      channel1 = 1;
      channel2 = 5;
      channel3 = 7;
      channel4 = 11;
      channel_preset = 8;
    break;
    case 12:
      channel1 = 1;
      channel2 = 5;
      channel3 = 7;
      channel4 = 11;
      channel_preset = 12;
    break;
  }
}

void flushChannels(int newChase){
  for(int c=1; c<RGB_CHANNELS+1; c++){
    dmx_master.setChannelValue(c, 0);
  }
  channel = 1;
  active_chase = newChase;
}


void rgbChase1(){
  if(active_chase != 1){ 
      flushChannels(1);
  }
  dmx_master.setChannelValue(channel, 255);
  delay(100);
  dmx_master.setChannelValue(channel, 0);
  channel += 1;
  if(channel > RGB_CHANNELS){
    channel = 1;
  }
  return;
}


void rgbChase2(){
  if(active_chase != 2){ //Ser till att chasen avbryts om vi byter chase
      flushChannels(2);
  }
  for(int x=0; x<255;x++){
    if(stopChase == true){ //Ser till att chasen avbryts om vi byter chase
      break;
    }
      
    for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, x);
    }
    delay(delayTime);
  }
  for(int x=254; x>0;x--){
    if(stopChase == true){
      stopChase = false;
      for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, 0);
    }
      break;
    }
    for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, x);
    }
    delay(delayTime);
  }
  
  
}


void rgbChase3(){
  if(active_chase != 3){ 
      flushChannels(3);
  }
  int stepSize = 25; //Hur långt den ska hoppa i DMX-värde för varje steg,
  
  for(int c=1; c<RGB_CHANNELS+1; c++){
    dmx_master.setChannelValue(c, turn);
  }
  if(turn == 0){
    direc = 1;
  }
  if(turn == 250){
    direc = -1;
  }
  turn += stepSize*direc;
}


void rgbChase5(){
  if(active_chase != 5){ 
      flushChannels(5);
  }
  int previousChannel = channel;
  channel += 1;
  if(channel == RGB_CHANNELS + 1){
    channel = 1;
  }
  dmx_master.setChannelValue(channel, 255);
  dmx_master.setChannelValue(previousChannel, 0);
  delay(50);
}


void rgbChase6(){
  if(active_chase != 6){ 
      flushChannels(6);
  }
  int previousChannel = channel;
  channel += 3;
  if(channel > RGB_CHANNELS){
    if(cycle < 3){
      cycle += 1;
    } else {
      cycle = 1;
    }
    channel = cycle;
  }
  dmx_master.setChannelValue(channel, 255);
  dmx_master.setChannelValue(previousChannel, 0);
  delay(100);
}


void rgbChase7(){
  if(active_chase != 7){ 
      flushChannels(7);
  }
  int previousChannel = channel;
  delayTime = mySlides.getSlideValue(slide6)*-1+1023;
  if(channel < 3){
    channel ++;
  } else {
    channel = 1;
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(channel+(x*3), 255);
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(previousChannel+(x*3), 0);
  }
  delay(delayTime);
}


void rgbChase8(){
  if(active_chase != 8){ 
      flushChannels(8);
  }
  delayTime = mySlides.getSlideValue(slide6)*-1+1023;
  int previousChannel1 = channel1;
  int previousChannel2 = channel2;
  int previousChannel3 = channel3;
  int previousChannel4 = channel4;
  if(channel_preset != 8){
    changePreset(8);
  }
  if(channel1 < 3){
    channel1 += 1;
  } else {
    channel1 = 1;
  }
  if(channel2 < 6){
    channel2 += 1;
  } else {
    channel2 = 4;
  }
  if(channel3 < 9){
    channel3 += 1;
  } else {
    channel3 = 7;
  }
  if(channel4 < 12){
    channel4 += 1;
  } else {
    channel4 = 10;
  }
  dmx_master.setChannelValue(channel1, 255);
  dmx_master.setChannelValue(previousChannel1, 0);
  dmx_master.setChannelValue(channel2, 255);
  dmx_master.setChannelValue(previousChannel2, 0);
  dmx_master.setChannelValue(channel3, 255);
  dmx_master.setChannelValue(previousChannel3, 0);
  dmx_master.setChannelValue(channel4, 255);
  dmx_master.setChannelValue(previousChannel4, 0);
  delay(delayTime);
}

//Strob
void rgbChase9(){
  if(active_chase != 9){ 
      flushChannels(9);
  }
  delayTime = mySlides.getSlideValue(slide6)*-1+1023;
  for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, 255);
  }
  delay(delayTime);
  for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, 0);
  }
  delay(delayTime);
}

//Fade 1 färg i taget
void rgbChase10(){
  if(active_chase != 10){ 
      flushChannels(10);
  }
  delayTime = mySlides.getSlideValue(slide6)*-1+1023;
  for(int x=0; x<255;x++){
    if(stopChase == true){ //Ser till att chasen avbryts om vi byter chase
      stopChase = false;
      for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, 0);
    }
      break;
    }
    for(int c=turn; c<RGB_CHANNELS+1; c+=3){
      dmx_master.setChannelValue(c, x);
    }
    delay(5);
  }
  for(int x=254; x>-1;x--){
    if(stopChase == true){
      stopChase = false;
      for(int c=1; c<RGB_CHANNELS+1; c++){
      dmx_master.setChannelValue(c, 0);
    }
      break;
    }
    for(int c=turn; c<RGB_CHANNELS+1; c+=3){
      dmx_master.setChannelValue(c, x);
    }
    delay(5);
  }
  if(turn < 3){
    turn++;
  } else{
    turn = 1;
  }
  delay(delayTime);
}

//samma färg på alla, soundtrigger
void rgbChase11(){
  if(active_chase != 11){ 
      flushChannels(11);
  }
  int previousChannel = channel;
  if(channel < 3){
    channel ++;
  } else {
    channel = 1;
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(channel+(x*3), 255);
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(previousChannel+(x*3), 0);
  }
  delay(100);
}


void rgbChase12(){
  if(active_chase != 12){ 
      flushChannels(12);
  }
  delayTime = mySlides.getSlideValue(slide6)*-1+1023;
  int previousChannel1 = channel1;
  int previousChannel2 = channel2;
  int previousChannel3 = channel3;
  int previousChannel4 = channel4;
  if(channel_preset != 12){
    changePreset(12);
  }
  if(channel1 < 3){
    channel1 += 1;
  } else {
    channel1 = 1;
  }
  if(channel2 < 6){
    channel2 += 1;
  } else {
    channel2 = 5;
  }
  if(channel3 < 9){
    channel3 += 1;
  } else {
    channel3 = 7;
  }
  if(channel4 < 12){
    channel4 += 1;
  } else {
    channel4 = 11;
  }
  dmx_master.setChannelValue(channel1, 255);
  dmx_master.setChannelValue(previousChannel1, 0);
  dmx_master.setChannelValue(channel2, 255);
  dmx_master.setChannelValue(previousChannel2, 0);
  dmx_master.setChannelValue(channel3, 255);
  dmx_master.setChannelValue(previousChannel3, 0);
  dmx_master.setChannelValue(channel4, 255);
  dmx_master.setChannelValue(previousChannel4, 0);
  delay(delayTime);
}  

void laserChase1(){
  dmx_master.setChannelValue(13, 0);
}

void laserChase2(){
  dmx_master.setChannelValue(13, 155);
  dmx_master.setChannelValue(14, 22);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 65);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 255);
  dmx_master.setChannelValue(19, 10);
}

void laserChase3(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 135);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 60);
  dmx_master.setChannelValue(17, 100);
  dmx_master.setChannelValue(18, 155);
  dmx_master.setChannelValue(19, 10);
}

void laserChase4(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 96);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 168);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}

void laserChase5(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 158);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 215);
  dmx_master.setChannelValue(17, 205);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}

void laserChase6(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 217);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 10);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}
void laserChase7(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 226);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 10);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}
void laserChase8(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 52);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 230);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}
void laserChase9(){
  dmx_master.setChannelValue(13, 155);
  dmx_master.setChannelValue(14, 255);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 255);
  dmx_master.setChannelValue(17, 255);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}
void laserChase10(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 237);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 175);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}
void laserChase11(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 82);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 160);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}
void laserChase12(){
  dmx_master.setChannelValue(13, 205);
  dmx_master.setChannelValue(14, 127);
  dmx_master.setChannelValue(15, 255);
  dmx_master.setChannelValue(16, 150);
  dmx_master.setChannelValue(17, 0);
  dmx_master.setChannelValue(18, 0);
  dmx_master.setChannelValue(19, 0);
}


void ballChase1(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, 255);
  dmx_master.setChannelValue(22, 255);
  dmx_master.setChannelValue(23, 255);
  dmx_master.setChannelValue(24, 10);
  dmx_master.setChannelValue(25, 250);
  dmx_master.setChannelValue(26, 0);
}
void ballChase2(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, 255);
  dmx_master.setChannelValue(22, 255);
  dmx_master.setChannelValue(23, 255);
  dmx_master.setChannelValue(24, 0);
  dmx_master.setChannelValue(25, 0);
  dmx_master.setChannelValue(26, 100);
}
void ballChase3(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, 255);
  dmx_master.setChannelValue(22, 255);
  dmx_master.setChannelValue(23, 255);
  dmx_master.setChannelValue(24, 0);
  dmx_master.setChannelValue(25, 0);
  dmx_master.setChannelValue(26, 130);
}
void ballChase4(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, 255);
  dmx_master.setChannelValue(22, 255);
  dmx_master.setChannelValue(23, 255);
  dmx_master.setChannelValue(24, 0);
  dmx_master.setChannelValue(25, 0);
  dmx_master.setChannelValue(26, 150);
}
void ballChase5(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, 255);
  dmx_master.setChannelValue(22, 255);
  dmx_master.setChannelValue(23, 255);
  dmx_master.setChannelValue(24, 0);
  dmx_master.setChannelValue(25, 0);
  dmx_master.setChannelValue(26, 190);
}
void ballChase6(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, 255);
  dmx_master.setChannelValue(22, 255);
  dmx_master.setChannelValue(23, 255);
  dmx_master.setChannelValue(24, 0);
  dmx_master.setChannelValue(25, 0);
  dmx_master.setChannelValue(26, 210);
}

void RGBLights(){ //Funktion för att ställa värden med slides
  if(active_chase != 0){ 
      flushChannels(0);
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(channel+(x*3), mySlides.getSlideValue(slide1));
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(channel+1+(x*3), mySlides.getSlideValue(slide2));
  }
  for(int x=0; x<RGB_UNITS; x++){
    dmx_master.setChannelValue(channel+2+(x*3), mySlides.getSlideValue(slide3));
  }
  if(phys_button >160){
    for(int x=0; x<RGB_UNITS; x++){
      dmx_master.setChannelValue(channel+(x*3), 255);
    }
    phys_button = 0;
  } else if(phys_button >200){
    for(int x=0; x<RGB_UNITS; x++){
      dmx_master.setChannelValue(channel+1+(x*3), 255);
    }
    phys_button = 0;
  } else if(phys_button >250){
    for(int x=0; x<RGB_UNITS; x++){
      dmx_master.setChannelValue(channel+2+(x*3), 255);
    }
    phys_button = 0;
  }
  if(phys_button == 0){
    attachInterrupt(4, butRead, FALLING);
  }
}

void laserSlides(){
  dmx_master.setChannelValue(13, mySlides.getSlideValue(slide1));
  dmx_master.setChannelValue(14, mySlides.getSlideValue(slide2));
  dmx_master.setChannelValue(15, mySlides.getSlideValue(slide3));
  dmx_master.setChannelValue(16, mySlides.getSlideValue(slide4));
  dmx_master.setChannelValue(17, mySlides.getSlideValue(slide5));
  if(phys_button >160){
    dmx_master.setChannelValue(13, 255);
    phys_button = 0;
  } else if(phys_button >200){
    dmx_master.setChannelValue(14, 255);
    phys_button = 0;
  } else if(phys_button >250){
    dmx_master.setChannelValue(15, 255);
    phys_button = 0;
  } else if(phys_button >345){
    dmx_master.setChannelValue(16, 255);
    phys_button = 0;
  } else if(phys_button >500){
    dmx_master.setChannelValue(17, 255);
    phys_button = 0;
  }
  if(phys_button == 0){
    attachInterrupt(4, butRead, FALLING);
  }
}

void ballSlides(){
  dmx_master.setChannelValue(20, 255);
  dmx_master.setChannelValue(21, mySlides.getSlideValue(slide1));
  dmx_master.setChannelValue(22, mySlides.getSlideValue(slide2));
  dmx_master.setChannelValue(23, mySlides.getSlideValue(slide3));
  dmx_master.setChannelValue(24, mySlides.getSlideValue(slide4));
  dmx_master.setChannelValue(25, mySlides.getSlideValue(slide5));
  if(phys_button >160){
    dmx_master.setChannelValue(21, 255);
    phys_button = 0;
  } else if(phys_button >200){
    dmx_master.setChannelValue(22, 255);
    phys_button = 0;
  } else if(phys_button >250){
    dmx_master.setChannelValue(23, 255);
    phys_button = 0;
  } else if(phys_button >345){
    dmx_master.setChannelValue(24, 255);
    phys_button = 0;
  } else if(phys_button >500){
    dmx_master.setChannelValue(25, 255);
    phys_button = 0;
  }
  if(phys_button == 0){
    attachInterrupt(4, butRead, FALLING);
  }
}



void loop()
{
  if(slide_mode == 1){
    RGBLights(); //Går till att läsa sliders om den inte ska köra chase
  } else if(slide_mode == 2){
      laserSlides();
  } else if(slide_mode == 3){
      ballSlides();
  }
  if (rgbChase > 0){
    switch (rgbChase){
      case 1:
        if(soundTrigger()){           //att göras i varje chase
          rgbChase1();
        }
      break;
      case 2:
        delayTime = 25;
        rgbChase2();
      break;
      case 3:
        if(soundTrigger()){
          rgbChase3();
        }
      break;
      case 4:
        delayTime = 5;
        rgbChase2();
      break;
      case 5:
        if(soundTrigger()){
          rgbChase5();
        }
      break;
      
      case 6:
        if(soundTrigger()){
          rgbChase6();
        }
      break;
      case 7:
//        if(soundTrigger()){
          rgbChase7();
//        }
      break;
      case 8:
          rgbChase8();
      break;
      case 9:
          rgbChase9();
      break;
      case 10:
          rgbChase10();
      break;
      case 11:
        if(soundTrigger()){
          rgbChase11();
        }
      break;
      case 12:
          rgbChase12();
      break;
    }
  }
  if(laserChase > 0){
    switch (laserChase){
      case 1:
        laserChase1();
      break;
      case 2:
        laserChase2();
      break;
      case 3:
        laserChase3();
      break;
      case 4:
        laserChase4();
      break;
      case 5:
        laserChase5();
      break;
      case 6:
        laserChase6();
      break;
      case 7:
        laserChase7();
      break;
      case 8:
        laserChase8();
      break;
      case 9:
        laserChase9();
      break;
      case 10:
        laserChase10();
      break;
      case 11:
        laserChase11();
      break;
      case 12:
        laserChase12();
      break;
    }
  }

  if(ballChase > 0){
    switch (ballChase){
      case 1:
        ballChase1();
      break;
      case 2:
        ballChase2();
      break;
      case 3:
        ballChase3();
      break;
      case 4:
        ballChase4();
      break;
      case 5:
        ballChase5();
      break;
      case 6:
        ballChase6();
      break;
    }
  }
  /*if(slide_mode == 0){ //Kollar om vi kör ett chase, annars gå vi till att läsa sliders
    if(rgbChase == 1){
      soundLevel = analogRead(soundPin); //Läser av ljudnivån, kan med fördel bara göras en gång istället för 
      if(soundLevel > tresh){           //att göras i varje chase
        rgbChase1();
      }
    }
    if(rgbChase == 2){
      delayTime = 25;
      rgbChase2();
    }
    if(rgbChase == 3){
      soundLevel = analogRead(soundPin);
      if(soundLevel > tresh){
        rgbChase3();
      }
    }
    if(rgbChase == 4){
      delayTime = 5;
      rgbChase2();
    }
    if(rgbChase == 5){
      soundLevel = analogRead(soundPin);
      if(soundLevel > tresh){
        rgbChase5();
      }
    }
    if(rgbChase == 6){
      soundLevel = analogRead(soundPin);
      if(soundLevel > tresh){
        rgbChase6();
      }
    }
    }
    /*else{
      RGBLights(); //Går till att läsa sliders om den inte ska köra chase
    }*/
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) //som det står ovan :D
{ 
    slide_mode = Wire.read();
    rgbChase = Wire.read();
    laserChase = Wire.read();
    ballChase = Wire.read();
     
  /*if(func == 2){
    
    }*/
  stopChase = true; //ser till att nuvarande chase avbryts
  
}

void butRead(){
  if(analogRead(A15) != 0){
    phys_button = analogRead(A15);
  }
  detachInterrupt(4);
}


















