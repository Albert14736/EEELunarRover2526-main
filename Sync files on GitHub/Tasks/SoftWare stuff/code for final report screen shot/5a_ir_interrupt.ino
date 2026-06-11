// IR is a stream of FAST pulses, so we use a hardware INTERRUPT to never miss one.
const int IR_PIN = 9;
volatile unsigned long pulseCnt = 0;     // 'volatile': it is changed inside the interrupt
unsigned long lastIRTime = 0;
float IRpulseRate = 0;

void countPulse() { pulseCnt++; }        // the ISR: just +1 per pulse (kept tiny on purpose)

void setup() {
  // every rising pulse on IR_PIN automatically calls countPulse()
  attachInterrupt(digitalPinToInterrupt(IR_PIN), countPulse, RISING);
}

void loop() {
  // Every ~500ms, turn the pulse COUNT into a RATE (pulses per second)
  unsigned long elapsed = millis() - lastIRTime;
  if (elapsed >= 500) {
    noInterrupts();                       // pause interrupts to read the shared counter safely
    unsigned long cnt = pulseCnt; pulseCnt = 0;
    interrupts();
    lastIRTime = millis();
    IRpulseRate = (float)cnt * 1000.0 / (float)elapsed;   // rate = count / real elapsed time
  }
}

// The rate maps to a tier that the rock-type check uses:
String irStatus() {
  if (IRpulseRate > 400)       return "547";
  else if (IRpulseRate >= 240) return "312";
  else                         return "No IR";
}
