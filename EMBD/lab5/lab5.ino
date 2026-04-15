#include "Wheels.h"
#include "Ticker.h"
#include "LiquidCrystal_I2C.h"
#include "PinChangeInterrupt.h"
#include "Servo.h"

#define INTINPUT0 A0
#define INTINPUT1 A1
#define TRIG A2
#define ECHO A3
#define SERVO 3

byte LCDAddress = 0x27;
LiquidCrystal_I2C lcd(LCDAddress, 16, 2);
Wheels w;
Servo serwo;

volatile int cnt0;
volatile int cnt1;

void setup() {
  // RF, RB, RS
  // LF, LB, LS
  
  w.attach(9,10,5,11,12,6);

  pinMode(INTINPUT0, INPUT);
  pinMode(INTINPUT1, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TRIG, OUTPUT);    // TRIG startuje sonar
  pinMode(ECHO, INPUT);     // ECHO odbiera powracający impuls

  cnt0=0;
  cnt1=0;
  
  attachPCINT(digitalPinToPCINT(INTINPUT0), increment, CHANGE);
  attachPCINT(digitalPinToPCINT(INTINPUT1), increment, CHANGE);

  lcd.init();
  lcd.backlight();
  
  serwo.attach(SERVO);
  serwo.write(90);
}

unsigned long PERIOD_LCD = 200;
unsigned long PERIOD_WHEELS = 50;
unsigned long PERIOD_MOVEMENT = 10;
unsigned long PERIOD_LED = 10;
unsigned long PERIOD_SONAR = 50;

unsigned long GLOBAL_T = 0;
unsigned long DISTANCE = 0;
unsigned long DISTANCE_SONAR = 1000;
volatile char IS_DONE = 0;
volatile char MOVEMENT_TYPE = 1; // 0 - stop, 1 - forward, 2 - backward

Ticker WHEELS = Ticker(50, *wheel_fun);
Ticker LCD = Ticker(PERIOD_LCD, *lcd_fun);
Ticker MOVEMENT = Ticker(PERIOD_MOVEMENT, *movement_fun);
Ticker LED = Ticker(PERIOD_LED, *led_fun);
Ticker SONAR = Ticker(PERIOD_SONAR, *sonar_fun);

void loop() 
{
  GLOBAL_T = millis();
  SONAR.check();
  WHEELS.check();
  LCD.check();
  MOVEMENT.check();
  LED.check();
}

void lcd_fun(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("[D] ");
      lcd.print((cnt1 + cnt0 )/ 2);
      lcd.setCursor(0,1);
      if(MOVEMENT_TYPE == 0){
        lcd.print("[X] ");
      }
      else if(MOVEMENT_TYPE == 1){
        lcd.print("[F] ");
      }
      else if(MOVEMENT_TYPE == 2){
        lcd.print("[B] ");
      }
      lcd.print(" [SONAR] ");
      lcd.print(DISTANCE_SONAR);
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


unsigned volatile int BLOCK_SONAR = 0;

void movement_fun(){
  // Maybe adjust for the serwo drift
  if(DISTANCE_SONAR < 30)
  {
    BLOCK_SONAR = 1;
    MOVEMENT_TYPE = 0;
    w.back();
    delay(400);
    w.stop();

    serwo.write(135);
    int left1 = getDistance();
    delay(300);

    serwo.write(45);
    int right2 = getDistance();
    delay(300);

    serwo.write(135);
    int left2 = getDistance();
    delay(300);

    serwo.write(45);
    int right2 = getDistance();
    delay(300);

    serwo.write(90);

    if(((left1 + left2) / 2) > (((right1 + right2) / 2))){
      w.rotateLeft();
      delay(500);
      w.stop();
    } else {
      w.rotateRight();
      delay(500);
      w.stop();
    }

    
    BLOCK_SONAR = 0;
    MOVEMENT_TYPE = 1;
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

unsigned int getDistance() {
  
  unsigned long tot;
  unsigned int distance;
  
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG, LOW);
  
  tot = pulseIn(ECHO, HIGH);
  
  distance = tot/58;
  return distance;
}

void sonar_fun() {
  if(BLOCK_SONAR == 0)
  { 
    serwo.write(90);
    DISTANCE_SONAR = getDistance();
  }
}
