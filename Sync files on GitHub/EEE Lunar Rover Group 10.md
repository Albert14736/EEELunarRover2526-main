**EEE LUNAR ROVER GROUP 10**

| Group Members | Role |
| :---- | :---- |
| Mohammed Salem | Movement + User interface |
| Albert Ma |  |
| Devesh Kemani | Magnetism Detection |
| Christopher Koh | Project Manager + Infrared Detection  |
| Wangmo Koo | Ultrasonic Detection + Parts IC |
| Ye Zifan | Age Detection |

**TIMELINE**

28 May - Interim Presentation  
11 June - Report + Reflection Form  
16 June - Demo

Componants to order

| Item  | Cost (pounds) | Min quantity |
| :---- | :---- | :---- |
| SS49E hall sensor | <1 |  |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |

Steering  
Receiving signals:

- Radio (age)  
- Infrared  
- Ultrasonic  
- Magnetic

Weight (max 750g)

HARDWARE

[https://www.keysight.com/used/gb/en/knowledge/guides/voltage-regulators](https://www.keysight.com/used/gb/en/knowledge/guides/voltage-regulators)

Sensing magnetic fields  
Hardware: SS49E hall sensor  
Pin 1 (VCC): Connect to 3.3V on your Metro board.  
Pin 2 (GND): Connect to GND.  
Pin 3 (Output): Connect to an analog pin, like A0.

Metro M0 converts the voltage into a number 0-1023  
When no magneti near by - supply voltage = 1.65ishV and analogRead() will return a value near 512.  
As a North pole approaches, the voltage rises toward 3.3V value moves toward 1023  
As south pole approaches, the voltage drops toward 0V (value moves toward 0).

Magnetic field strength is proportional to r^3 so might only detect small changes. If the signal is too weak use Opamp as a non-onverting amplifier. 

Sample code:  
```cpp
void setup() {  
  Serial.begin(9600);  
}

void loop() {  
  int sensorValue = analogRead(A0);  
    
  // 512 is the middle. We add a buffer of +/- 50 to avoid noise.  
  if (sensorValue > 560) {  
    Serial.println("Magnetic Field: UP (North)");  
  } else if (sensorValue < 460) {  
    Serial.println("Magnetic Field: DOWN (South)");  
  } else {  
    Serial.println("No strong magnetic field detected.");  
  }  
    
  delay(200);  
}  
```
How the robot can be controlled

We should manually control the robot so that it can move towards the rock and gather sensor readings.

We can’t use the pins 5, 7, 10 since they are used by the WifiShield.  
\[Reference to README file :   
The I/O pins pass through the WiFi Shield when it is connected, but the pins labelled CS, IRQ and RST on the WiFi Shield (Arduino pins 5,7 and 10\) are used by WiFi and can't be used for other purposes. \]

Therefore we should use other pins to control the motor.  
THerefore we should use: 

- Right : EN(8), DIR(9)  
- Left : EN(11), DIR(12)

ENABLE wires are coloured orange  
DIRECTION wires are coloured blue.

13/05  
Connected PCB to Metro board and uploaded the code `motor_move_v1` to test if the motors are connected properly.  
*(Code `motor_move_v1` removed. It was a failed test.)*

Test failed motors did not move, DC voltage 6.21 around battery terminal and 0.125A measured current through battery.

I found a wire that had not been placed properly (5V wire connecting board and PCB) fixed the issue and ran the code again.  
Rewrote the code as well.  
RESULT : LEFT motor was working fine but the right motor was not operating  
The issue is that the motor on the right side works, however the pin12 is contested with the signal from the wifi module therefore we will use other pins to deliver the data. Suggested pins : DIR_LEFT = 2, EN_LEFT = 3

**Rewritten code:** *(Extracted to `Tasks/Movement Code/motor_diagnostic_test.ino`)*

The robot has its ends connected in the other way around.  
We can fix this by the software or just connecting the wires to different parts around.
