// Safe pins avoiding WiFi Shield conflict (5, 7, 10, 11, 12, 13)
const int DIR_LEFT  = 2;  
const int EN_LEFT   = 3;  
const int DIR_RIGHT = 4;  
const int EN_RIGHT  = 6;

// Speed for tests (0-255). 200 is a good starting point - high enough  
// to overcome stiction, low enough to be safe on the bench.  
const int TEST_SPEED = 200;

void setMotor(int dirPin, int enPin, int speed) {  
  // speed: -255 to +255. Positive = forward, negative = reverse, 0 = stop.  
  if (speed >= 0) {  
    digitalWrite(dirPin, HIGH);  
    analogWrite(enPin, speed);  
  } else {  
    digitalWrite(dirPin, LOW);  
    analogWrite(enPin, -speed);  
  }  
}

void stopBoth() {  
  setMotor(DIR_LEFT,  EN_LEFT,  0);  
  setMotor(DIR_RIGHT, EN_RIGHT, 0);  
}

void setup() {  
  Serial.begin(9600);  
  while (!Serial && millis() < 3000) { ; }  // wait briefly for serial monitor

  pinMode(DIR_LEFT,  OUTPUT);  
  pinMode(EN_LEFT,   OUTPUT);  
  pinMode(DIR_RIGHT, OUTPUT);  
  pinMode(EN_RIGHT,  OUTPUT);

  stopBoth();

  Serial.println("Motor diagnostic - no WiFi");  
  Serial.println("Watch each motor in turn.");  
  delay(1000);  
}

void loop() {  
  // --- Right motor alone ---  
  Serial.println("RIGHT motor: forward");  
  setMotor(DIR_RIGHT, EN_RIGHT, TEST_SPEED);  
  delay(2000);

  Serial.println("RIGHT motor: stop");  
  setMotor(DIR_RIGHT, EN_RIGHT, 0);  
  delay(1000);

  Serial.println("RIGHT motor: reverse");  
  setMotor(DIR_RIGHT, EN_RIGHT, -TEST_SPEED);  
  delay(2000);

  Serial.println("RIGHT motor: stop");  
  setMotor(DIR_RIGHT, EN_RIGHT, 0);  
  delay(1500);

  // --- Left motor alone ---  
  Serial.println("LEFT motor: forward");  
  setMotor(DIR_LEFT, EN_LEFT, TEST_SPEED);  
  delay(2000);

  Serial.println("LEFT motor: stop");  
  setMotor(DIR_LEFT, EN_LEFT, 0);  
  delay(1000);

  Serial.println("LEFT motor: reverse");  
  setMotor(DIR_LEFT, EN_LEFT, -TEST_SPEED);  
  delay(2000);

  Serial.println("LEFT motor: stop");  
  setMotor(DIR_LEFT, EN_LEFT, 0);  
  delay(1500);

  // --- Both motors together ---  
  Serial.println("BOTH motors: forward");  
  setMotor(DIR_LEFT,  EN_LEFT,  TEST_SPEED);  
  setMotor(DIR_RIGHT, EN_RIGHT, TEST_SPEED);  
  delay(2000);

  Serial.println("BOTH motors: stop");  
  stopBoth();  
  delay(2000);  
}