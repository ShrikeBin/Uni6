#include "Wheels.h"
#include "Ticker.h"
#include "LiquidCrystal_I2C.h"
#include "PinChangeInterrupt.h"

#define INTINPUT0 A0
#define INTINPUT1 A1

byte LCDAddress = 0x3F;
LiquidCrystal_I2C lcd(LCDAddress, 16, 2);
Wheels w;

volatile int cnt0;
volatile int cnt1;

void setup() {
  // RF, RB, RS
  // LF, LB, LS
  
  w.attach(9,10,5,11,12,6);

  pinMode(INTINPUT0, INPUT);
  pinMode(INTINPUT1, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  cnt0=0;
  cnt1=0;
  
  attachPCINT(digitalPinToPCINT(INTINPUT0), increment, CHANGE);
  attachPCINT(digitalPinToPCINT(INTINPUT1), increment, CHANGE);

  lcd.init();
  lcd.backlight();
}

unsigned long PERIOD_LCD = 200;
unsigned long PERIOD_WHEELS = 50;
unsigned long PERIOD_MOVEMENT = 10;
unsigned long PERIOD_LED = 10;

unsigned long GLOBAL_T = 0;
unsigned long DISTANCE = 0;
volatile char IS_DONE = 0;
volatile char MOVEMENT_TYPE = 1; // 0 - stop, 1 - forward, 2 - backward

Ticker WHEELS = Ticker(50, *wheel_fun);
Ticker LCD = Ticker(PERIOD_LCD, *lcd_fun);
Ticker MOVEMENT = Ticker(PERIOD_MOVEMENT, *movement_fun);
Ticker LED = Ticker(PERIOD_LED, *led_fun);

void loop() 
{
  GLOBAL_T = millis();
  WHEELS.check();
  LCD.check();
  MOVEMENT.check();
  LED.check();
}

void lcd_fun(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(cnt0);
      lcd.print(" [L][P] ");
      lcd.print(cnt1);
      lcd.setCursor(0,1);
      if(MOVEMENT_TYPE == 0){
        lcd.print("STOPPED");
      }
      else if(MOVEMENT_TYPE == 1){
        lcd.print("FORWARD");
      }
      else if(MOVEMENT_TYPE == 2){
        lcd.print("BACKWARD");
      }
      lcd.setCursor(0,0); 
}

void wheel_fun(){
    w.setSpeed(200);
    if(MOVEMENT_TYPE == 0){
        w.stop();
    }
    else if(MOVEMENT_TYPE == 1){
      w.forward();
      DISTANCE = DISTANCE + 1; 
    }
    else if(MOVEMENT_TYPE == 2){
      w.back();
      DISTANCE = DISTANCE + 1; 
    }
}


unsigned long last_switch = 0;
  
void movement_fun(){
  
  if (GLOBAL_T - last_switch >= 5000)
  {
      MOVEMENT_TYPE = (MOVEMENT_TYPE + 1) % 3;
      last_switch = GLOBAL_T;
  }
}

unsigned long last_led = 0;
  
void led_fun(){
    digitalWrite(LED_BUILTIN, LOW);
    if(MOVEMENT_TYPE == 2){
      if (GLOBAL_T - last_led >= 1000){
          digitalWrite(LED_BUILTIN, HIGH);
          last_led = GLOBAL_T;
      }
      else{
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
}


void increment() {
  if(digitalRead(INTINPUT0))
    cnt0++;
  if(digitalRead(INTINPUT1))
    cnt1++;
}
