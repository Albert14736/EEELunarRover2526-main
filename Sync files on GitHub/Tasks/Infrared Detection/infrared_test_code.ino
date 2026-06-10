int pulseCnt = 0;
long lastIRTime = 0;
float IRpulseRate;

void countPulse() {
  pulseCnt++;
}

void setup() {
  pinMode(2, INPUT);
  attachInterrupt(2, countPulse, RISING);
  Serial.begin(9600);
}

void loop() {
  long elapsedTime = millis() - lastIRTime;
  if (elapsedTime > 500) {
    lastIRTime = millis();
    IRpulseRate = (float) pulseCnt * 1000 / (float) elapsedTime;
    pulseCnt = 0;
  }
  if(IRpulseRate > 400){
    Serial.println("547s^-1: Basaltoid OR Lunarite");
  }
  if((IRpulseRate > 240 ) && (IRpulseRate < 370)){
    Serial.println("312s^-1: Gravion OR Regolix");
  }
  if(IRpulseRate < 240){
    Serial.println("No strong IR detected");
  }
  Serial.println(IRpulseRate);
}