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

// ================= ENUMS =================
enum MovementType { STOP, FORWARD, BACKWARD, LEFT, RIGHT };
enum RobotState   { IDLE, MANUAL, CRASH, SPRING };

// ================= GLOBALS =================
uint32_t      lastCode        = 0xE31CFF00;
unsigned long lastIRReadTime  = 0;
unsigned long lastRepeatRead  = 0;

const unsigned long IR_READ_COOLDOWN              = 300;
const unsigned long MANUAL_STEERING_CUTOFF_PERIOD = 400;

byte LCDAddress = 0x27;
LiquidCrystal_I2C lcd(LCDAddress, 16, 2);
Wheels w;
Servo servo;

volatile int  dist_countL      = 0;
volatile int  dist_countR      = 0;
volatile int  RECEIVING_REPEAT = 0;

unsigned long GLOBAL_T       = 0;
unsigned long DISTANCE_SONAR = 1000;

volatile MovementType MOVEMENT_TYPE            = STOP;
volatile bool         BLOCK_SONAR              = false;
int                   CRASH_DETECTION_DISTANCE = 30;


int                   SPRING_TOLERANCE  = 5;
unsigned volatile int SPRING_DISTANCE   = 0;
volatile bool         isSPRING          = false;

void wheel_fun();
void lcd_fun();
void movement_fun();
void led_fun();
void sonar_fun();
void crash_fun();
void spring_fun();

unsigned long PERIOD_LCD              = 200;
unsigned long PERIOD_WHEELS           = 50;
unsigned long PERIOD_MOVEMENT         = 100;
unsigned long PERIOD_LED              = 100;
unsigned long PERIOD_SONAR            = 50;
unsigned long PERIOD_CRASH_PREVENTION = 25;
unsigned long PERIOD_SPRING           = 50;

// ================ TICKERS =================

Ticker WHEELS           = Ticker(PERIOD_WHEELS,           *wheel_fun);
Ticker LCD_T            = Ticker(PERIOD_LCD,              *lcd_fun);
Ticker MOVEMENT         = Ticker(PERIOD_MOVEMENT,         *movement_fun);
Ticker LED_T            = Ticker(PERIOD_LED,              *led_fun);
Ticker SONAR            = Ticker(PERIOD_SONAR,            *sonar_fun);
Ticker CRASH_PREVENTION = Ticker(PERIOD_CRASH_PREVENTION, *crash_fun);
Ticker SPRING_T         = Ticker(PERIOD_SPRING,           *spring_fun);

// ================= STATE MACHINE =================
RobotState currentState = IDLE;
RobotState previousState = IDLE;

void enterState(RobotState next) {
  previousState = currentState;
  currentState  = next;

  switch (currentState) {
    case IDLE:
      MOVEMENT_TYPE = STOP;
      w.stop();
      break;

    case MANUAL:
      // nothing needed
      break;

    case CRASH:
      BLOCK_SONAR    = true;
      MOVEMENT_TYPE  = STOP;
      w.stop();
      break;

    case SPRING:
      MOVEMENT_TYPE = STOP;
      w.stop();
      // Fetches current sonar distance as spring oscilation radious
      SPRING_DISTANCE = DISTANCE_SONAR;
      isSPRING = true;
      break;
  }
}

// ================= SETUP =================
void setup() {
  w.attach(9, 10, 5, 11, 12, 6);

  pinMode(INTINPUT0,    INPUT);
  pinMode(INTINPUT1,    INPUT);
  pinMode(LED_BUILTIN,  OUTPUT);
  pinMode(TRIG,         OUTPUT);
  pinMode(ECHO,         INPUT);

  attachPCINT(digitalPinToPCINT(INTINPUT0), increment, CHANGE);
  attachPCINT(digitalPinToPCINT(INTINPUT1), increment, CHANGE);

  lcd.init();
  lcd.backlight();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  servo.attach(SERVO);
  servo.write(90);

  enterState(IDLE);
}

// ================= LOOP =================
void loop() {
  GLOBAL_T = millis();

  ir_fun();
  SONAR.check();

  switch (currentState) {
    case IDLE:
      LCD_T.check();
      break;

    case MANUAL:
      CRASH_PREVENTION.check();
      WHEELS.check();
      LCD_T.check();
      MOVEMENT.check();
      LED_T.check();  // LED is for beepin'
      break;

    case CRASH:
      LCD_T.check();
      // crash_fun() does everything in an override
      break;

    case SPRING:
      LCD_T.check();
      WHEELS.check();
      LED_T.check();
      SPRING_T.check();
      break;
  }
}

// ================= IR =================
void ir_fun() {
  if (millis() - lastRepeatRead > MANUAL_STEERING_CUTOFF_PERIOD) {
    RECEIVING_REPEAT = 0;
  }

  if (!IrReceiver.decode()) return;

  uint32_t code = IrReceiver.decodedIRData.decodedRawData;

  bool gotCommand = true;
  switch (code) 
  {
    case 0xE718FF00: MOVEMENT_TYPE = FORWARD;  RECEIVING_REPEAT = 1; break;  // UP
    case 0xAD52FF00: MOVEMENT_TYPE = BACKWARD; RECEIVING_REPEAT = 1; break;  // DOWN
    case 0xE31CFF00: MOVEMENT_TYPE = STOP;     RECEIVING_REPEAT = 1; break;  // OK
    case 0xF708FF00: MOVEMENT_TYPE = LEFT;     RECEIVING_REPEAT = 1; break;  // LEFT
    case 0xA55AFF00: MOVEMENT_TYPE = RIGHT;    RECEIVING_REPEAT = 1; break;  // RIGHT
    case 0x0:                                          // REPEAT
      RECEIVING_REPEAT = 1;
      lastRepeatRead   = millis();
      gotCommand       = false;
      break;


    case 0xF20DFF00:                                   // '#' as SPRING mode entry
      if(!isSPRING) {
        enterState(SPRING);
      } else {
        isSPRING = false;
        enterState(IDLE);
      }
      break;


    default:
      gotCommand = false;
      break;
  }

  if (currentState == IDLE || currentState == MANUAL) 
  {
    if (gotCommand && MOVEMENT_TYPE != STOP) 
    {
      if (currentState == IDLE){
        enterState(MANUAL);
      } 
    } 
    else if (gotCommand && MOVEMENT_TYPE == STOP)
    {
      if (currentState == MANUAL) {
        enterState(IDLE);
      }
    }
  }

  if (millis() - lastIRReadTime < IR_READ_COOLDOWN) {
    IrReceiver.resume();
    return;
  }

  lastIRReadTime = millis();
  if (code != 0x0) lastCode = code;

  IrReceiver.resume();
}

// ================= TICKER CALLBACKS =================
void movement_fun() {
  if (currentState != MANUAL) return;

  if (RECEIVING_REPEAT == 0) {
    MOVEMENT_TYPE = STOP;
    enterState(IDLE);
  }
}

void lcd_fun() {
  lcd.clear();
  lcd.setCursor(0, 0);

  switch (currentState) {
    case IDLE:
      lcd.print("IDLE");
      lcd.setCursor(0, 1);
      lcd.print("[S] ");
      lcd.print(DISTANCE_SONAR);
      lcd.print("cm");
      break;

    case MANUAL:
      lcd.print(decodeIR(lastCode));
      lcd.print(" ");
      lcd.print(RECEIVING_REPEAT ? "HOLD" : "    ");
      lcd.setCursor(0, 1);
      lcd.print("[D] ");
      lcd.print((dist_countL + dist_countR) / 2);
      lcd.print(" [S] ");
      lcd.print(DISTANCE_SONAR);
      break;

    case CRASH:
      lcd.print("!! CRASH !!");
      lcd.setCursor(0, 1);
      lcd.print("[S] ");
      lcd.print(DISTANCE_SONAR);
      lcd.print("cm");
      break;

    case SPRING:
      lcd.print("SPRING ");
      lcd.print(SPRING_DISTANCE);
      lcd.print("cm");
      lcd.setCursor(0, 1);
      lcd.print("now:");
      lcd.print(DISTANCE_SONAR);
      lcd.print(" mv:");
      switch (MOVEMENT_TYPE) {
        case FORWARD:  lcd.print("FWD"); break;
        case BACKWARD: lcd.print("BCK"); break;
        case STOP:     lcd.print("STP"); break;
        default:       lcd.print("???"); break;
      }
      break;
  }
}

void wheel_fun() {
  if ((currentState != MANUAL) && (currentState != SPRING)) return;

  w.setSpeed(200);
  switch (MOVEMENT_TYPE) {
    case STOP:     w.stop();        break;
    case FORWARD:  w.forward();     break;
    case BACKWARD: w.back();        break;
    case RIGHT:    w.rotateLeft(); break;
    case LEFT:     w.rotateRight();  break;
  }
}

unsigned long last_led = 0;
void led_fun() {
  digitalWrite(LED_BUILTIN, LOW);
  if (MOVEMENT_TYPE == BACKWARD) {
    if (GLOBAL_T - last_led >= 1000) {
      digitalWrite(LED_BUILTIN, HIGH);
      last_led = GLOBAL_T;
    }
  }
}

void sonar_fun() {
  if (!BLOCK_SONAR) {
    servo.write(90);
    DISTANCE_SONAR = getDistance();
  }
}

void crash_fun() {
  if (currentState != CRASH && DISTANCE_SONAR < CRASH_DETECTION_DISTANCE) {
    enterState(CRASH);
    handleCrash();
    enterState(IDLE);
  }
}

// ================= SPRING SIMULATION ==============
void spring_fun(){
  int low = SPRING_DISTANCE - SPRING_TOLERANCE;
  int high = SPRING_DISTANCE + SPRING_TOLERANCE; 

  if(DISTANCE_SONAR > high){
    MOVEMENT_TYPE = FORWARD;
  } else if (DISTANCE_SONAR < low){
    MOVEMENT_TYPE = BACKWARD;
  } else {
    MOVEMENT_TYPE = STOP;
  }
}


// ================= CRASH RECOVERY =================
void handleCrash() {
  w.setSpeed(200);
  lcd_fun();
  w.back();
  delay(500);
  lcd_fun();
  w.stop();

  servo.write(135); int left1  = getDistance(); delay(300);
  servo.write(45);  int right1 = getDistance(); delay(300);
  servo.write(135); int left2  = getDistance(); delay(300);
  servo.write(45);  int right2 = getDistance(); delay(300);
  servo.write(90);

  if (((left1 + left2) / 2) > ((right1 + right2) / 2)) {
    w.rotateLeft();
  } else {
    w.rotateRight();
  }
  delay(400);
  w.stop();
  lcd_fun();

  BLOCK_SONAR   = false;
  MOVEMENT_TYPE = STOP;
}

// ================= HELPERS =================
void increment() {
  if (digitalRead(INTINPUT0)) dist_countL++;
  if (digitalRead(INTINPUT1)) dist_countR++;
}

unsigned int getDistance() {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG, LOW);
  unsigned long tot = pulseIn(ECHO, HIGH);
  return (unsigned int)(tot / 58);
}

const char* stateToStr(RobotState state) {
  switch (state) {
    case IDLE:   return "IDLE";
    case MANUAL: return "MAN";
    case CRASH:  return "CRASH";
    case SPRING: return "SPRING";
    default:     return "?";
  }
}

const char* decodeIR(uint32_t code) {
  switch (code) {
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
    case 0x0:        return "RPT";
    default:         return "UNK";
  }
}