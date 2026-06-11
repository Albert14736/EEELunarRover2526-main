// Magnetism: a Hall sensor on an analogue pin. We just threshold the reading.
const int MAG_PIN = A4;
bool magIsNorth(int v) { return v > 620; }
bool magIsSouth(int v) { return v < 600; }     // 600-620 is a dead zone -> "None"

String magStatus() {
  int v = analogRead(MAG_PIN);
  if (magIsNorth(v)) return "UP (North)";
  if (magIsSouth(v)) return "DOWN (South)";
  return "None";
}

// Ultrasound: a digital pin. HIGH = 40kHz detected, LOW = nothing.
const int US_PIN = 12;
String usStatus() { return digitalRead(US_PIN) == HIGH ? "Detected" : "None"; }
