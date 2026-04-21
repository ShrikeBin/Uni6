#include "Wheels.h"
#include "Ticker.h"
#include "LiquidCrystal_I2C.h"
#include "PinChangeInterrupt.h"
#include "Servo.h"
#include "IRremote.h"

#define INTINPUT0 A0
#define INTINPUT1 A1
#define TRIG A2
#define ECHO A3
#define SERVO 3
#define IR_PIN 2

uint32_t lastCode = 0xE31CFF00;
unsigned long lastIRReadTime = 0;
unsigned long lastRepeatRead = 0;
const unsigned long IR_READ_COOLDOWN = 300; // milliseconds
const unsigned long MOVEMENT_READ_COOLDOWN = 200; // milliseconds

byte LCDAddress = 0x27;
LiquidCrystal_I2C lcd(LCDAddress, 16, 2);
Wheels w;
Servo serwo;

volatile int cnt0;
volatile int cnt1;
volatile int RECEIVE_REPEAT = 0;

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

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  
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
volatile char MOVEMENT_TYPE = 0; // 0 - stop, 1 - forward, 2 - backward, 3 - left, 4 - right

Ticker WHEELS = Ticker(50, *wheel_fun);
Ticker LCD = Ticker(PERIOD_LCD, *lcd_fun);
Ticker MOVEMENT = Ticker(PERIOD_MOVEMENT, *movement_fun);
Ticker LED = Ticker(PERIOD_LED, *led_fun);
Ticker SONAR = Ticker(PERIOD_SONAR, *sonar_fun);

void loop() 
{
  ir_fun(); // bo niby musi być częściej idk
  SONAR.check();
  WHEELS.check();
  LCD.check();
  MOVEMENT.check();
  LED.check();
}

void ir_fun() {
  if (millis() - lastRepeatRead > MOVEMENT_READ_COOLDOWN) {
    RECEIVE_REPEAT = 0;
  }
  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;

      switch(code) {
      case 0xE718FF00: // UP
        MOVEMENT_TYPE = 1;
        break;

      case 0xAD52FF00: // DOWN
        MOVEMENT_TYPE = 2;
        break;

      case 0xE31CFF00: // OK
        MOVEMENT_TYPE = 0;
        break;

      case 0xF708FF00: // LEFT
        MOVEMENT_TYPE = 3;
        break;

      case 0xA55AFF00: // RIGHT
        MOVEMENT_TYPE = 4;
        break;

      case 0x0: // REPEAT
        RECEIVE_REPEAT = 1;
        lastRepeatRead = millis();
        break;

      default:
        break;
    }

    if (millis() - lastIRReadTime < IR_READ_COOLDOWN) {
      IrReceiver.resume();
      return;
    }

    lastIRReadTime = millis();
    if(code != 0x0)
    {
      lastCode = code;
    }

    IrReceiver.resume();
  }
}

void lcd_fun(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("[D] ");
      lcd.print((cnt1 + cnt0 )/ 2);
      lcd.print(" ");
      lcd.print(decodeIR(lastCode));
      lcd.print(RECEIVE_REPEAT);
      if(RECEIVE_REPEAT == 0)
      {
        MOVEMENT_TYPE = 0;
      }
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
    else if(MOVEMENT_TYPE == 3){
      w.rotateRight();
    }
    else if(MOVEMENT_TYPE == 4){
      w.rotateLeft();
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
    delay(500);
    w.stop();

    serwo.write(135);
    int left1 = getDistance();
    delay(300);

    serwo.write(45);
    int right1 = getDistance();
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
    MOVEMENT_TYPE = 0;
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

const char* decodeIR(uint32_t code) {

  // Code: 0xBA45FF00 // 1
  // Code: 0xB946FF00 // 2
  // Code: 0xB847FF00 // 3
  // Code: 0xBB44FF00 // 4
  // Code: 0xBF40FF00 // 5
  // Code: 0xBC43FF00 // 6
  // Code: 0xF807FF00 // 7
  // Code: 0xEA15FF00 // 8
  // Code: 0xF609FF00 // 9
  // Code: 0xE916FF00 // *
  // Code: 0xE619FF00 // 0
  // Code: 0xF20DFF00 // #
  // Code: 0xE718FF00 // UP
  // Code: 0xF708FF00 // LEFT
  // Code: 0xE31CFF00 // OK
  // Code: 0xA55AFF00 // RIGHT
  // Code: 0xAD52FF00 // DOWN

  switch(code) {
    case 0xBA45FF00: return "1";
    case 0xB946FF00: return "2";
    case 0xB847FF00: return "3";
    case 0xBB44FF00: return "4";
    case 0xBF40FF00: return "5";
    case 0xBC43FF00: return "6";
    case 0xF807FF00: return "7";
    case 0xEA15FF00: return "8";
    case 0xF609FF00: return "9";
    case 0xE619FF00: return "0";

    case 0xE916FF00: return "*";
    case 0xF20DFF00: return "#";

    case 0xE718FF00: return "UP";
    case 0xAD52FF00: return "DOWN";
    case 0xF708FF00: return "LEFT";
    case 0xA55AFF00: return "RIGHT";
    case 0xE31CFF00: return "OK";

    case 0x0: return "REPEAT";

    default: return "UNKNOWN";
  }
}