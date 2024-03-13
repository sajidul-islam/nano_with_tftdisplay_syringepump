#include <Arduino.h>
#include <TFT.h>  
#include <SPI.h>
#include <AccelStepper.h>

////////////////////Pin Define/////////////////
#define CS   10
#define DC   9
#define RST  8  
///////////////////// Switches///////////////////////
#define BUTTON_UP   A0
#define BUTTON_DOWN A1
#define BUTTON_SELECT A2


#define home_switch1 2 
#define home_switch2 3 

#define x_step_pin 6   
#define x_dir_pin 5



AccelStepper stepper1(1, x_step_pin, x_dir_pin); 

////////////////////// TFT LIbrary///////////////
TFT screen = TFT(CS, DC, RST);

////////////////Variables to manage menu state/////////////////////
int currentMenuOption = 1;
const int totalMenuOptions = 3;
int currentmode = 0;

bool homing = false;
//////////////////////////////////////Pre defined flow profile/////////////////
const int dataPointsCount = 50;
int sinusoidal[dataPointsCount] = {0, 32, 64, 95, 125, 152, 177, 199, 218, 233, 244, 251, 254, 253, 248, 239, 226, 209, 188, 165, 139, 110, 80, 48, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int dataIndex = 0;
const int ledPin = 12;  
int ledState = LOW; 
unsigned long previousMillis = 0;  
const long interval = 1000; 

/////////functions////////////

void updateMenuDisplay();
void Sinusoidal_function();
void blink();
void executeMenuOption(int option) ;
void go_tohome_position();
void sisusoidal_movement();

////////////////////////////////////////

void updateMenuDisplay() 
{
  screen.background(0, 0, 0); // Clear the screen
  screen.text("Syringe Pump Menu", 40, 5);
  screen.text("________________________________________________________", 0, 10);
  screen.setTextSize(1);
  String menuItems[totalMenuOptions] = {"1. Home Position", "2. Sinusoidal","4. Custome"};
  for (int i = 0; i < totalMenuOptions; i++) {
    if (i == (currentMenuOption - 1)) {
      screen.stroke(255, 0, 0); // Highlight the selected menu item in red
    } else {
      screen.stroke(255, 255, 255); // Other items in white
    }
    screen.text(menuItems[i].c_str(), 40, 20 + (i * 10));
  }
}


void Sinusoidal_function()
 {    
      
      screen.background(255, 255, 255);  
      screen.stroke(0, 0, 0);
      screen.text("Half Sinusoidal", 45, 20);
      screen.stroke(0, 0, 0); 
      screen.line(40, 20, 40, 110); 
      screen.line(35, 100, 150, 100); 
      int prevX = 45;
      int prevY = 90; 
      for (int i = 1; i < dataPointsCount; i++) 
      {
        int x = map(i, 0, dataPointsCount-1, 45, 140);
        int y = 130-map(sinusoidal[i], 0, 255, 40, 100); 
        screen.stroke(255, 0, 0); 
        screen.line(prevX, prevY, x, y); 
        prevX = x;
        prevY = y;
      }

 }

 void blink()
 {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }

 }





  void sisusoidal_movement()
{ 
  homing =  false;
  stepper1.runSpeed();
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= 50) 
  {
    lastMillis = currentMillis;
    dataIndex = (dataIndex + 1) % (sizeof(sinusoidal) / sizeof(sinusoidal[0]));
    stepper1.setSpeed(sinusoidal[dataIndex]*3);
  }  

}


void executeMenuOption(int option) 
{
  currentmode = option;
  screen.background(0, 0, 0);
  switch (option) {
    case 1:
      screen.text("Going to Home Position...", 0, 20);
      break;
    case 2:
      Sinusoidal_function();
      break;
    case 3:
      // option3Function();
      screen.text("Custom", 30, 20);
      break;
  }
}






void setup() {
  // Initialize the display
  screen.begin();
  screen.background(0, 0, 0);
  screen.stroke(255, 255, 255);
  screen.setTextSize(1);

  // Initialize the button pins
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  pinMode(x_step_pin, OUTPUT);
  pinMode(x_dir_pin,OUTPUT);  
  pinMode(home_switch1, INPUT_PULLUP);
  pinMode(home_switch2, INPUT_PULLUP);

  stepper1.setMaxSpeed(3000);
  // Display the initial menu
  updateMenuDisplay();
  Serial.begin(9600);
}

void loop() 
{
  static unsigned long lastButtonPress = 0;
  if (millis() - lastButtonPress > 200) { // Basic debounce protection
    if (digitalRead(BUTTON_UP) == LOW) {
      if (currentMenuOption > 1) {
        currentMenuOption--;
        updateMenuDisplay();
        lastButtonPress = millis();
      }
    } else if (digitalRead(BUTTON_DOWN) == LOW) {
      if (currentMenuOption < totalMenuOptions) {
        currentMenuOption++;
        updateMenuDisplay();
        lastButtonPress = millis();
      }
    } else if (digitalRead(BUTTON_SELECT) == LOW) {
      executeMenuOption(currentMenuOption);
      lastButtonPress = millis();
    }
  }

  if (currentmode ==1)
  {
    go_tohome_position();
  }

  if (currentmode ==2)
  {
    blink();
    sisusoidal_movement();
  }

}
  

void go_tohome_position()
{

    static bool homeDisplayUpdated = false;
   if(digitalRead(home_switch2) == 1 && homing == false)
   {
    stepper1.runSpeed();
    stepper1.setSpeed(-3000);
   }
   if(digitalRead(home_switch2) == 0 && !homeDisplayUpdated)
   {
    homing = true;
    updateMenuDisplay();
    homeDisplayUpdated = true;
   }

  
}