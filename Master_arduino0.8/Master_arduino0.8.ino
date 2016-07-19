//Edit
//include libraries
#include <UTFT_Buttons.h>
#include <UTFT.h>
#include <UTouch.h>
#include <fonts.c>
#include <Wire.h>
#include <Relay.h>

//Initiera skärm, knappar och relän
UTFT    myGLCD(CPLD, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 32, 3, 2);
Relay myRelays(1500);
UTFT_Buttons myButtons(&myGLCD, &myTouch, &myRelays);


//Inkludera fonter och färger
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
word colorlist[] = {VGA_WHITE, VGA_BLACK, VGA_RED, VGA_BLUE, VGA_GREEN, VGA_FUCHSIA, VGA_YELLOW, VGA_AQUA};

//interrupts, BUTT_PIN läser knapp, INTERRUPT_PIN läser interrupt
#define BUTT_PIN A0
#define INTERRUPT_PIN 5

int code = 0;

//////////////////Nödvändigt för att skapa knappar och chasear/////////////////////////////
// Buttons page 1
int but1;
int but2;
int but3;
int but4;
int but5;
int but6;
int but7;

//Buttons page 2
int but8;
int but9;
int but10;
int but11;

//Pagebuttons
int incPage;
int decPage;

//Chases RGB
int cha1;
int cha2;
int cha3;
int cha4;
int cha5;
int cha6;
int cha7;
int cha8;
int cha9;
int cha10;
int cha11;
int cha12;

//Chases Laser
int cha13;
int cha14;
int cha15;
int cha16;
int cha17;
int cha18;
int cha19;
int cha20;
int cha21;
int cha22;
int cha23;
int cha24;

//Chase Boll
int cha25;
int cha26;
int cha27;
int cha28;
int cha29;
int cha30;
////////////////////////////////////////////////////////////////////////////////////////////


//Relays
int strobbLiten;
int strobbStor;
int rinnande;
int UVTak;
int FX;
int RGBSpot;
int miniLaser;
int UVBar;

//Måste ändras om antal chase ändras
#define MAX_CHASES 30
#define MAX_CHASE_IN_GROUP 12
#define CHASE_GROUPS 3

//Antal knappar per sida
#define BUTTONS_PER_PAGE 50
#define PAGES 2

int active_chases[CHASE_GROUPS] = {}; //Sparar aktuell chase för varje grupp
int chase_number[CHASE_GROUPS] = {}; //Vilken chase som är aktuellt per grupp för att skicka

int chase_setup_place = 0; //Variabel för att öka index i chases
int master_setup_place = 0; //variabel för att öka index i chase_master

int chase_groups[CHASE_GROUPS][MAX_CHASE_IN_GROUP] = {}; //Håller varje grupp med chasear på var sin plats
int chase_per_group[CHASE_GROUPS] = {}; //Antal chase per grupp

boolean prev_drawn; //Om vi ritat ut chaseas tidigare


int current_chase_group = 0; //Vilken chase grupp som är aktuell
int prev_chase_group; //Föregående aktuell chase grupp
int chase_master[CHASE_GROUPS] = {}; //sparar knappnummer på lämpligt index

int has_dmx = 0; //Vilken som skickar på reglarna

volatile int pressed = -1; //Aktuell nertryck touch-knapp
volatile int phys_button = 0; //Akutell nertryckt fysisk knapp

volatile boolean light_mode[CHASE_GROUPS]; //Om det är DMX eller chase

int current_page;
int next_page;
int page_buttons[PAGES][BUTTONS_PER_PAGE] = {};
int total_page_buttons[PAGES] = {};
int prev_chase_group_p1 = 0;
int prev_chase_group_p2 = 0;


// the setup routine runs once when you press reset:
void setup() {

  //pinMode(BUTT_PIN, INPUT); //Sätter pin för att läsa fysiska knappar till input
  //attachInterrupt(INTERRUPT_PIN, buttRead, LOW); //Ställer in interrupt för fysiska knappar
  //Initierar LCD och touch
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  //Sätter färger osv
  myGLCD.setColor(VGA_WHITE);
  myButtons.setTextFont(BigFont);
  myButtons.setButtonColors(VGA_BLACK, VGA_GRAY, VGA_WHITE, VGA_BLACK, VGA_WHITE);

  //Startar kommunikation
  Wire.begin();
  Serial.begin(9600);



}

//Funktion för att fixa chase-listor
int popChaseList(int xa, int ya, int xb, int yb, char label[], int place, int group) {
  int tempButton;
  tempButton = myButtons.addButton(xa, ya, xb, yb, label);
  myButtons.disableButtonDraw(tempButton);
  chase_groups[group][place] = tempButton;
  chase_per_group[group] += 1;
  chase_setup_place += 1;
  return tempButton;
}

int popChaseMaster(int xa, int ya, int xb, int yb, char label[]) {
  int tempButton;
  tempButton = myButtons.addButton(xa, ya, xb, yb, label);
  chase_master[master_setup_place] = tempButton;
  light_mode[master_setup_place] = 0;
  chase_per_group[master_setup_place] = 0;
  master_setup_place += 1;
  return tempButton;

}


void buttonSetup() {

  //page 0
  current_page = 0;
  //add chase masters
  but1 = popChaseMaster(10, 7, 200, 110, "RGB TAK");
  page_buttons[current_page][0] = but1;
  but2 = popChaseMaster(10, 124, 200, 110, "LASER S");
  page_buttons[current_page][1] = but2;
  but3 = popChaseMaster(10, 241, 200, 110, "BOLLJEVEL");
  page_buttons[current_page][2] = but3;

  //add normal buttons
  but4 = myButtons.addButton(10, 358, 200, 110, "RINNANDE");
  page_buttons[current_page][3] = but4;

  but5 = myButtons.addButton(590, 7, 200, 110, "STROBB S");
  page_buttons[current_page][4] = but5;

  but6 = myButtons.addButton(590, 124, 200, 110, "STROBB L");
  page_buttons[current_page][5] = but6;

  but7 = myButtons.addButton(590, 241, 200, 110, "MINILASER");
  page_buttons[current_page][6] = but7;

  incPage = myButtons.addButton(590, 358, 100, 100, "NEXT");
  page_buttons[current_page][7] = incPage;


  total_page_buttons[current_page] = 8;

  //page 1
  current_page = 1;


  but8 = myButtons.addButton(10, 7, 200, 110, "RGB SPOT");
  page_buttons[current_page][0] = but8;

  but9 = myButtons.addButton(10, 124, 200, 110, "UV BAR");
  page_buttons[current_page][1] = but9;

  but10 = myButtons.addButton(10, 241, 200, 110, "UV TAK");
  page_buttons[current_page][2] = but10;

  decPage = myButtons.addButton(10, 358, 100, 100, "PREV");
  page_buttons[current_page][3] = decPage;

  but11 = myButtons.addButton(590, 7, 200, 110, "FX");
  page_buttons[current_page][4] = but11;




  total_page_buttons[current_page] = 5;
}

void chaseButtonSetup() {
  //Chasegroup1
  cha1 = popChaseList(252, 17, 100, 100, "RINN", 0, 0);
  cha2 = popChaseList(362, 17, 100, 100, "FADE S", 1, 0);
  cha3 = popChaseList(472, 17, 100, 100, "ALL STEP", 2, 0);
  cha4 = popChaseList(252, 127, 100, 100, "FADE F", 3, 0);
  cha5 = popChaseList(362, 127, 100, 100, "SINGLE STEP", 4, 0);
  cha6 = popChaseList(472, 127, 100, 100, "CHRIFTIAN", 5, 0);
  cha7 = popChaseList(252, 237, 100, 100, "RGB1", 6, 0);
  cha8 = popChaseList(362, 237, 100, 100, "RGB2", 7, 0);
  cha9 = popChaseList(472, 237, 100, 100, "RGB3", 8, 0);
  cha10 = popChaseList(252, 347, 100, 100, "RGB4", 9, 0);
  cha11 = popChaseList(362, 347, 100, 100, "RGB5", 10, 0);
  cha12 = popChaseList(472, 347, 100, 100, "RGB6", 11, 0);


  //Chasegroup2
  cha13 = popChaseList(252, 17, 100, 100, "SCAN", 0, 1);
  cha14 = popChaseList(362, 17, 100, 100, "SCAN2", 1, 1);
  cha15 = popChaseList(472, 17, 100, 100, "SCAN3", 2, 1);
  cha16 = popChaseList(252, 127, 100, 100, "SCAN4", 3, 1);
  cha17 = popChaseList(362, 127, 100, 100, "SCAN5", 4, 1);
  cha18 = popChaseList(472, 127, 100, 100, "SCAN6", 5, 1);
  cha19 = popChaseList(252, 237, 100, 100, "SCAN7", 6, 1);
  cha20 = popChaseList(362, 237, 100, 100, "SCAN8", 7, 1);
  cha21 = popChaseList(472, 237, 100, 100, "SCAN9", 8, 1);
  cha22 = popChaseList(252, 347, 100, 100, "SCAN10", 9, 1);
  cha23 = popChaseList(362, 347, 100, 100, "SCAN11", 10, 1);
  cha24 = popChaseList(472, 347, 100, 100, "SCAN12", 11, 1);

  //Chasegroup3
  cha25 = popChaseList(252, 17, 100, 100, "BOLL1", 0, 2);
  cha26 = popChaseList(362, 17, 100, 100, "BOLL2", 1, 2);
  cha27 = popChaseList(472, 17, 100, 100, "BOLL33", 2, 2);
  cha28 = popChaseList(252, 127, 100, 100, "BOLL4", 3, 2);
  cha29 = popChaseList(362, 127, 100, 100, "BOLL5", 4, 2);
  cha30 = popChaseList(472, 127, 100, 100, "BOLL6", 5, 2);
}

void relaySetup() {
  strobbStor = myRelays.addRelay(A1, but5, TOGGLE);
  strobbLiten = myRelays.addRelay(A2, but6, TOGGLE);
  UVTak = myRelays.addRelay(A3, but10, FULL_ON);
  FX = myRelays.addRelay(A4, but11, FLASH);
  RGBSpot = myRelays.addRelay(A5, but8, TOGGLE);
  miniLaser = myRelays.addRelay(A6, but7, TOGGLE);
  rinnande = myRelays.addRelay(A7, but4, TOGGLE);
  UVBar = myRelays.addRelay(A8, but9, FULL_ON);

}




void pageOnePhys() {
  if (phys_button > 498) {
    pressed = incPage;
  }

  else if (phys_button > 335) {
    pressed = myButtons.checkButtons(true, but7);
  }

  else if (phys_button > 250) {
    pressed = myButtons.checkButtons(true, but6);
  }

  else if (phys_button > 198) {
    pressed = myButtons.checkButtons(true, but5);
  }

  else if (phys_button > 164) {
    pressed = myButtons.checkButtons(true, but4);
  }

  else if (phys_button > 139) {
    pressed = but3;
  }
  else if (phys_button > 120) {
    pressed = but2;
  }
  else if (phys_button > 100) {
    pressed = but1;
  }
  phys_button = 0;
  return;
}

void pageTwoPhys() {
  /*if(phys_button > 498){
    if(code !=0){
      code = 0;
    }
    else{
      code = 1;
    }
  }
  else if (phys_button > 335){
    if(code != 2){
      code = 0;
    }
    else{
      myButtons.enableButton(but11, true);
      code = 0;
    }
  }
  else if(phys_button > 250){
    if(code != 1){
      code = 0;
    }
    else{
      code = 2;
    }
  } */
  if (phys_button > 250) {

  }
  else if (phys_button > 198) {
    if (myButtons.buttonEnabled(but11)) {
      pressed = myButtons.checkButtons(true, but11);
      myButtons.disableButton(but11, true);
    }
  }
  else if (phys_button > 164) {
    pressed = decPage;
  }
  else if (phys_button > 139) {
    pressed = myButtons.checkButtons(true, but10);
  }
  else if (phys_button > 120) {
    pressed = myButtons.checkButtons(true, but9);
  }
  else if (phys_button > 100) {
    pressed = myButtons.checkButtons(true, but8);
  }
  phys_button = 0;
  //Serial.println(code);
  return;
}

void enableChaseButtons() {
  boolean currentDrawn;
  if (prev_chase_group != current_chase_group) {
    myGLCD.setColor(VGA_BLACK);
    myGLCD.drawRoundRect(10, 7 + (117 * prev_chase_group), 210, 117 + (117 * prev_chase_group));
    myGLCD.drawRoundRect(9, 6 + (117 * prev_chase_group), 211, 118 + (117 * prev_chase_group));
    myGLCD.setColor(VGA_RED);
    myGLCD.drawRoundRect(10, 7 + (117 * current_chase_group), 210, 117 + (117 * current_chase_group));
    myGLCD.drawRoundRect(9, 6 + (117 * current_chase_group), 211, 118 + (117 * current_chase_group));
  }
  if (light_mode[current_chase_group] == 0) {
    if (prev_chase_group != current_chase_group) {
      myGLCD.setColor(VGA_BLACK);
      myGLCD.fillRoundRect(243, 8, 581, 456);
      for (int i = 0; i < chase_per_group[prev_chase_group]; i++) {
        myButtons.disableButtonDraw(chase_groups[prev_chase_group][i]);
      }
    }
    myGLCD.setColor(VGA_WHITE);
    myGLCD.drawRoundRect(242, 7, 582, 457);

    for (int i = 0; i < chase_per_group[current_chase_group]; i++) {
      myButtons.enableButtonDraw(chase_groups[current_chase_group][i]);
    }
    myGLCD.setColor(VGA_BLACK);
    myGLCD.fillRect(212, 35 + (107 * current_chase_group), 220, 40 + (107 * current_chase_group));
    myGLCD.setColor(VGA_WHITE);
    myGLCD.print("C", 212, 35 + (107 * current_chase_group));
    updateChaseButtons();
    currentDrawn = true;
  }
  else {
    myGLCD.setColor(VGA_BLACK);
    myGLCD.fillRoundRect(241, 6, 583, 458);
    for (int i = 0; i < chase_per_group[prev_chase_group]; i++) {
      myButtons.disableButtonDraw(chase_groups[prev_chase_group][i]);
    }
    myGLCD.setColor(VGA_BLACK);
    myGLCD.fillRect(212, 35 + (107 * current_chase_group), 220, 40 + (107 * current_chase_group));
    myGLCD.setColor(VGA_WHITE);
    myGLCD.print("M", 212, 35 + (107 * current_chase_group));
    updateChaseButtons();
    currentDrawn = false;
  }

  if (prev_drawn == true) {
    myGLCD.setColor(VGA_BLACK);
    myGLCD.fillRect(211 , 50 + (107 * prev_chase_group), 241, 56 + (107 * prev_chase_group));
  }
  if (currentDrawn == true) {
    myGLCD.setColor(VGA_WHITE);
    myGLCD.fillRect(211 , 50 + (107 * current_chase_group), 241, 56 + (107 * current_chase_group));
  }

  prev_drawn = currentDrawn;
}


void updateChaseButtons() {
  myButtons.setTextFont(SmallFont);
  for (int i = 0; i < chase_per_group[current_chase_group]; i++) {
    if (chase_groups[current_chase_group][i] == active_chases[current_chase_group]) {
      myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GREEN);
      myButtons.drawButton(chase_groups[current_chase_group][i]);
      myButtons.setButtonColors(VGA_BLACK, VGA_GRAY, VGA_WHITE, VGA_BLACK, VGA_WHITE);
    }
    else {
      myButtons.drawButton(chase_groups[current_chase_group][i]);
    }
  }
  myButtons.setTextFont(BigFont);
}

void mainCheck() {
  //noInterrupts();
  //detachInterrupt(INTERRUPT_PIN);
  Serial.println("mainCheck");
  for (int i = 0; i < CHASE_GROUPS; i++) {
    Serial.println("itteration");
    if (pressed == chase_master[i]) {
      Serial.println("1");
      current_chase_group = i;
      if (current_chase_group == prev_chase_group) {
        Serial.println("2");
        if (light_mode[i] % 2 == 0) {
          Serial.println("3");
          chase_number[i] = 0;
          has_dmx = i + 1;
        }
        else {
          Serial.println("4");
          has_dmx = 0;
        }
        Serial.println("5");
        light_mode[i] = !light_mode[i];
      }
      else {
        Serial.println("6");
        if (light_mode[i] % 2 == 0) {
          Serial.println("7");
          has_dmx = 0;
        }
        else {
          Serial.println("8");
          has_dmx = i + 1;
        }
      }

      Serial.println("Innan wire");
      Wire.beginTransmission(4);
      Wire.write(has_dmx);
      Serial.println("I wire");
      for (int x = 0; x < CHASE_GROUPS; x++) {
        Wire.write(chase_number[x]);
      }
      Wire.endTransmission();
      Serial.println("Efter wire");

      enableChaseButtons();
      prev_chase_group = current_chase_group;
      break;
    }


    else if (light_mode[current_chase_group] == 0) {
      for (int i = 0; i < chase_per_group[current_chase_group]; i++) {
        int check = chase_groups[current_chase_group][i];
        if (pressed == check) {
          chase_number[current_chase_group] = i + 1;
          Wire.beginTransmission(4);
          Wire.write(has_dmx);
          for (int i = 0; i < CHASE_GROUPS; i++) {
            Wire.write(chase_number[i]);
            Serial.println(chase_number[i]);
          }
          Wire.endTransmission();
          active_chases[current_chase_group] = pressed;

          updateChaseButtons();
          break;
        }
      }
    }
  }
  if (pressed == incPage) {
    Serial.println("ny sida start 1");
    prev_chase_group_p1 = current_chase_group;
    current_chase_group = prev_chase_group_p2;
    next_page = 1;
    pageSwitch();
    //enableChaseButtons();
    Serial.println("ny sida klar 1");
  }
  else if (pressed == decPage) {
    Serial.println("ny sida start 2");
    prev_chase_group_p2 = current_chase_group;
    current_chase_group = prev_chase_group_p1;
    next_page = 0;
    pageSwitch();
    myGLCD.setColor(VGA_RED);
    myGLCD.drawRoundRect(10, 7 + (117 * current_chase_group), 210, 117 + (117 * current_chase_group));
    myGLCD.drawRoundRect(9, 6 + (117 * current_chase_group), 211, 118 + (117 * current_chase_group));
    enableChaseButtons();
    Serial.println("ny sida klar 2");
  }
  Serial.println("heejee");
  pressed = -1;
  /*while (analogRead(BUTT_PIN) != 0) {
  }*/
  //attachInterrupt(INTERRUPT_PIN, buttRead, LOW);
  Serial.println("main chekkus KLAAR");
  return;
  //interrupts();

}

void pageSwitch() {
  for (int i = 0; i < total_page_buttons[current_page]; i++) {
    myButtons.disableButtonDraw(page_buttons[current_page][i]);
  }
  for (int group = 0; group < CHASE_GROUPS; group++) {
    for (int i = 0; i < chase_per_group[group]; i++) {
      myButtons.disableButtonDraw(chase_groups[group][i]);
    }
  }
  for (int i = 0; i < total_page_buttons[next_page]; i++) {
    myButtons.enableButtonDraw(page_buttons[next_page][i]);
  }

  if (next_page == 1) {
    myButtons.disableButton(but11);
  }

  current_page = next_page;
  myGLCD.clrScr();
  myButtons.drawButtons();
  myButtons.drawAllButtonStatus();
  return;
}

void grattis() {

  myGLCD.fillRoundRect(345, 50, 350, 75);
  myGLCD.fillRoundRect(363, 50, 368, 75);
  myGLCD.fillRoundRect(389, 50, 394, 75);
  myGLCD.fillRoundRect(423, 50, 428, 75);
  myGLCD.fillRoundRect(450, 50, 455, 75);

  myGLCD.fillRoundRect(325, 80, 475, 90);
  myGLCD.fillRoundRect(325, 95, 475, 105);
  myGLCD.fillRoundRect(325, 110, 475, 120);
  myGLCD.setColor(VGA_TEAL);
  myGLCD.fillRoundRect(327, 75, 473, 80);
  myGLCD.setColor(VGA_RED);
  myGLCD.fillRoundRect(327, 90, 473, 95);
  myGLCD.setColor(VGA_YELLOW);
  myGLCD.fillRoundRect(327, 105, 473, 110);
  myGLCD.fillCircle(347, 47, 3);
  myGLCD.fillCircle(366, 47, 3);
  myGLCD.fillCircle(391, 47, 3);
  myGLCD.fillCircle(425, 47, 3);
  myGLCD.fillCircle(453, 47, 3);

  myGLCD.setColor(205, 133, 63);
  myGLCD.fillRoundRect(320, 120, 480, 136);

  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(BigFont);
  myGLCD.print("GRATTIS PHILIP!", CENTER, 235);
  delay(1000);
  myGLCD.print("On the fem-years dag...", CENTER, 250);
  myGLCD.setFont(SmallFont);
  delay(1000);
  myGLCD.print("Den kan inte skriva svenska alfabetet helt bra...", CENTER, 300);
  while (myTouch.dataAvailable() != true) {
  }
  myGLCD.clrScr();

}


void loop()
{
  Serial.println("Setup Start");
  //grattis();
  buttonSetup();
  chaseButtonSetup();
  relaySetup();
  current_page = 1;
  next_page = 0;
  pageSwitch();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_RED);
  myGLCD.drawRoundRect(10, 7 + (117 * current_chase_group), 210, 117 + (117 * current_chase_group));
  myGLCD.drawRoundRect(9, 6 + (117 * current_chase_group), 211, 118 + (117 * current_chase_group));
  Serial.println("setup klar");
  while (1) {
    if (myButtons.checkButtons() != -1) {
      Serial.println("trykk");
      pressed = myButtons.checkButtons();
      while (myButtons.checkButtons() != -1) {

      }
    }

    if (pressed != -1) {
      Serial.println("main chekkus");
      mainCheck();
    }

    if (phys_button != 0) {
      switch (current_page) {
        case 0:
          pageOnePhys();
        case 1:
          pageTwoPhys();
      }
    }
    else {
      //attachInterrupt(INTERRUPT_PIN, buttRead, LOW);
    }


  }
}



void buttRead() {
  if (analogRead(BUTT_PIN) > 50) {
    phys_button = analogRead(BUTT_PIN);
  }
  detachInterrupt(INTERRUPT_PIN);
  //Serial.println(phys_button);
}
























