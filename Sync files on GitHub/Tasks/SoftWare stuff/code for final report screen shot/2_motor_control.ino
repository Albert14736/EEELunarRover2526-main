// --- Motor pins (each motor has one PWM/speed pin and one direction pin) ---
const int EN_LEFT  = 3, DIR_LEFT  = 2;
const int EN_RIGHT = 4, DIR_RIGHT = 6;
int SPEED = 200;                              // PWM magnitude (0-255), set by the speed slider

// --- Watchdog: stop the motors if no command arrives for a while (e.g. WiFi drops) ---
const unsigned long WATCHDOG_TIMEOUT = 500;   // ms
unsigned long lastCmdTime = 0;

// Drive ONE wheel from a single signed number:
//   the sign -> direction pin (forward / reverse)
//   the size -> PWM duty on the enable pin (how fast it spins)
void setMotor(int dirPin, int enPin, int speed) {
  if (speed == 0) {
    analogWrite(enPin, 0);                    // stop
  } else if (speed > 0) {
    digitalWrite(dirPin, HIGH);               // forward
    analogWrite(enPin, speed);
  } else {
    digitalWrite(dirPin, LOW);                // reverse
    analogWrite(enPin, -speed);               // the PWM value is always positive
  }
}

void stopBoth() {
  setMotor(DIR_LEFT, EN_LEFT, 0);
  setMotor(DIR_RIGHT, EN_RIGHT, 0);
}

// Every direction is just TWO of these numbers:
void moveForward() {                          // both wheels forward -> drive straight
  setMotor(DIR_LEFT,  EN_LEFT,  SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis();                     // reset the watchdog timer
  replyAPI("Fwd");
}
void turnLeft() {                             // left back + right forward -> spin on the spot
  setMotor(DIR_LEFT,  EN_LEFT,  -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT,  SPEED);
  lastCmdTime = millis();
  replyAPI("Left");
}
// ... backward, right and the 4 diagonals follow the same idea: just pick two numbers

void loop() {
  server.handleClient();                                       // run any drive command from the browser
  if (millis() - lastCmdTime > WATCHDOG_TIMEOUT) stopBoth();   // safety: stop if no command lately
}
