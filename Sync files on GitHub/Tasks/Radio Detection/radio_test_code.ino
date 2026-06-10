void setup() {
  //Initialise pin modes and serial
  Serial.begin(9600); 
  Serial1.begin(600); //bit rate //serial 1 is the uart port
}

void loop() {
  while (Serial1.available()) {
    char c = Serial1.read();
    if(c == '#'){
      Serial.println();
      Serial.println("Age of rock: ");
    }
    Serial.print(c);
    Serial.print(" ");
  }
}