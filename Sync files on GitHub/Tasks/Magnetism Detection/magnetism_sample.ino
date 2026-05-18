/*
  Magnetism Detection Sample Code
  Hardware: SS49E Hall Effect Sensor connected to A0
*/

void setup() {  
  Serial.begin(9600);  
}

void loop() {  
  int sensorValue = analogRead(A0);  
    
  // 512 is the middle point for 3.3V logic. 
  // We add a buffer of +/- 50 to avoid noise.  
  if (sensorValue > 560) {  
    Serial.println("Magnetic Field: UP (North)");  
  } else if (sensorValue < 460) {  
    Serial.println("Magnetic Field: DOWN (South)");  
  } else {  
    Serial.println("No strong magnetic field detected.");  
  }  
    
  delay(200);  
}
