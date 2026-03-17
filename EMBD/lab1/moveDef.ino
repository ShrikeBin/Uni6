
#define LEFT_BACKWARD 13
#define LEFT_FORWARD 12
#define RIGHT_BACKWARD 11
#define RIGHT_FORWARD 10

#define LEFT_SPEED 6
#define RIGHT_SPEED 5

void setup() {
  pinMode(LEFT_BACKWARD, OUTPUT); // GRAY
  pinMode(LEFT_FORWARD, OUTPUT); // MAGENTA
  pinMode(RIGHT_BACKWARD, OUTPUT); // DARK BLUE
  pinMode(RIGHT_FORWARD, OUTPUT); // GREEN
  pinMode(WHITE, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  
  analogWrite(LEFT_SPEED, 255); // WHITE
  analogWrite(RIGHT_SPEED, 0); // YELLOW
}

void loop() {
  digitalWrite(RIGHT_BACKWARD, HIGH);
  digitalWrite(LEFT_BACKWARD, HIGH);
  delay(1000);
  digitalWrite(RIGHT_BACKWARD, LOW);
  digitalWrite(LEFT_BACKWARD, LOW);
  delay(1000);
}
