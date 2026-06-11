void setup() {
  pinMode(8, INPUT);
  Serial.begin(9600);
}

void loop() {
  int ultra = digitalRead(8);
  if(ultra == 1){
    Serial.println("Ultrasound detected");
  }  
  else{
    Serial.println("No ultrasound detected");
  }
  delay(200);
}