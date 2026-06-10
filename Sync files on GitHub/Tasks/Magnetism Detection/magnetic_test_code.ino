void setup() {
  pinMode(A4, INPUT);
  Serial.begin(9600);
}

void loop() {
  int MagVal = analogRead(A4);
  
  if (MagVal > 620) {
    Serial.println("Magnetic Field: UP (North)");
  } else if (MagVal < 600) {
    Serial.println("Magnetic Field: DOWN (South)");
  } else {
    Serial.println("No strong magnetic field detected.");
  }
  Serial.println(MagVal);
  delay(200);
}