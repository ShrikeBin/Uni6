#include "Wheels.h"
#include "LiquidCrystal_I2C.h"

uint8_t arrowRight[8] =
{
    0b01000,
    0b01100,
    0b00110,
    0b11111,
    0b11111,
    0b00110,
    0b01100,
    0b01000
};

byte LCDAddress = 0x27;
LiquidCrystal_I2C lcd(LCDAddress, 16, 2);
Wheels w;

void setup() {
  // RF, RB, RS
  // LF, LB, LS
  
  w.attach(10,11,5,12,13,6);
  
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, arrowRight);
}


unsigned long T = 0;
unsigned long T_LCD = 0;
unsigned long T_WHEEL = 0;
unsigned long DELAY_LCD = 100;
unsigned long DELAY_WHEELS = 50;

unsigned long DISTANCE = 50;
volatile char IS_DONE = 0;

void loop() 
{
  T = millis();
  
  if(T_LCD + DELAY_LCD <= T){
      T_LCD = T;
      if(IS_DONE == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(DISTANCE);
      DISTANCE = DISTANCE - 1;
      }
  }
  
  if(T_WHEEL + DELAY_WHEELS <= T){
    T_WHEEL = T;
    w.setSpeed(200);
    w.forward();
    if(DISTANCE >= 999999)
    {
      w.stop();
      IS_DONE = 1;
      lcd.clear();
      lcd.print("DONE");
    }
  }
}
