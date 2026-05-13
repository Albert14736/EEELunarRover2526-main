#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#include <WiFiWebServer.h>

// ==========================================
// 1. Hardware Pin Definitions
// ==========================================
const int DIR_LEFT  = 2;  // Changed from 12 to 2 to avoid WiFi conflict
const int EN_LEFT   = 3;  // PWM pin for left motor
const int DIR_RIGHT = 9;  // Right motor DIR
const int EN_RIGHT  = 8;  // Right motor PWM

const int SPEED = 200; // Testing speed (0-255), high enough to overcome stiction

// ==========================================
// 2. WiFi Configuration
// ==========================================
const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 0; // Set your group number to fix IP at 192.168.0.(groupNumber+1)

WiFiWebServer server(80);

// ==========================================
// 3. Web UI Frontend 
// ==========================================
const char webpage[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <style>
    body { font-family: sans-serif; text-align: center; background: #222; color: #fff; margin-top: 50px; user-select: none; -webkit-user-select: none; }
    .grid { display: grid; grid-template-columns: repeat(3, 90px); grid-gap: 15px; justify-content: center; margin-top: 30px; }
    .btn { width: 90px; height: 90px; font-size: 28px; font-weight: bold; border-radius: 15px; border: none; background: #4CAF50; color: white; touch-action: manipulation; }
    .btn:active { background: #45a049; }
    .stop { background: #f44336; }
    .empty { background: transparent; }
    #status-box { margin-top: 40px; padding: 15px; background: #111; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; font-family: monospace; color: #0f0; word-wrap: break-word;}
    #history-box { margin-top: 15px; padding: 15px; background: #1a1a1a; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; font-family: monospace; color: #aaa; height: 120px; overflow-y: auto; text-align: left; box-sizing: border-box; }
  </style>
</head>
<body>
  <h2>Lunar Rover PRO</h2>
  <div class="grid">
    <div class="empty"></div>
    <button class="btn" onmousedown="startMove('/forward')" onmouseup="stopMove()" ontouchstart="startMove('/forward')" ontouchend="stopMove()">&#9650;</button>
    <div class="empty"></div>
    <button class="btn" onmousedown="startMove('/left')" onmouseup="stopMove()" ontouchstart="startMove('/left')" ontouchend="stopMove()">&#9664;</button>
    <button class="btn stop" onmousedown="stopMove()" ontouchstart="stopMove()">STOP</button>
    <button class="btn" onmousedown="startMove('/right')" onmouseup="stopMove()" ontouchstart="startMove('/right')" ontouchend="stopMove()">&#9654;</button>
    <div class="empty"></div>
    <button class="btn" onmousedown="startMove('/backward')" onmouseup="stopMove()" ontouchstart="startMove('/backward')" ontouchend="stopMove()">&#9660;</button>
    <div class="empty"></div>
  </div>

  <div id="status-box">Status: Ready to connect...</div>
  <div id="history-box">
    <div style="color: #fff; border-bottom: 1px solid #444; padding-bottom: 5px; margin-bottom: 5px; text-align: center;">Command History</div>
    <div id="history-content"></div>
  </div>

  <script>
    var timer;
    var heartbeatCount = 0;
    var currentActionName = "";
    var statusBox = document.getElementById('status-box');
    var historyContent = document.getElementById('history-content');

    function getActionName(cmd) {
      if(cmd === '/forward') return 'Forward';
      if(cmd === '/backward') return 'Backward';
      if(cmd === '/left') return 'Left';
      if(cmd === '/right') return 'Right';
      return 'Stop';
    }

    function sendCmd(cmd) {
      // Send actual HTTP request to the WiFi module
      var xhttp = new XMLHttpRequest();
      xhttp.open('GET', cmd, true);
      xhttp.send();
      
      // Update the status monitor at the bottom of the page
      if (cmd !== '/stop') heartbeatCount++;
      statusBox.innerHTML = "Transmitting: <strong style='color:#fff; font-size:18px;'>" + cmd + "</strong><br>Packets Sent: " + heartbeatCount;
    }

    function startMove(cmd) {
      currentActionName = getActionName(cmd);
      heartbeatCount = 0;
      sendCmd(cmd); 
      clearInterval(timer);
      timer = setInterval(function() { sendCmd(cmd); }, 200); 
    }

    function stopMove() {
      clearInterval(timer);
      if (currentActionName !== "" && heartbeatCount > 0) {
        var logEntry = document.createElement('div');
        logEntry.innerHTML = "&gt; " + currentActionName + " <span style='color:#4CAF50;'>x" + heartbeatCount + "</span>";
        historyContent.insertBefore(logEntry, historyContent.firstChild);
      }
      currentActionName = "";
      sendCmd('/stop'); 
    }
  </script>
</body>
</html>
)rawliteral";

// ==========================================
// 4. Core Motor Functions (Backend Logic)
// ==========================================
unsigned long lastCmdTime = 0;
const unsigned long WATCHDOG_TIMEOUT = 500; // Force stop if no signal is received for 500ms!

void handleRoot() { server.send(200, "text/html", webpage); }

// Motor control function using positive/negative values for direction
void setMotor(int dirPin, int enPin, int speed) {  
  if (speed >= 0) {  
    digitalWrite(dirPin, HIGH);  
    analogWrite(enPin, speed);  
  } else {  
    digitalWrite(dirPin, LOW);  
    analogWrite(enPin, -speed);  
  }  
}

void stopBoth() {  
  setMotor(DIR_LEFT, EN_LEFT, 0);  
  setMotor(DIR_RIGHT, EN_RIGHT, 0);  
}

// Note: Teammate mentioned "ends connected the other way around"
// If the rover drives backward, just change SPEED to -SPEED below!
void moveForward() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis(); // Feed the watchdog (Reset timeout)
  server.send(200, "text/plain", "Moving Forward");
}

void moveBackward() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
  lastCmdTime = millis(); // Feed the watchdog
  server.send(200, "text/plain", "Moving Backward");
}

void turnLeft() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis(); // Feed the watchdog
  server.send(200, "text/plain", "Turning Left");
}

void turnRight() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
  lastCmdTime = millis(); // Feed the watchdog
  server.send(200, "text/plain", "Turning Right");
}

void stopRover() {
  stopBoth();
  server.send(200, "text/plain", "Stopped");
}

// ==========================================
// 5. Initialization & Network Binding
// ==========================================
void setup() {
  pinMode(DIR_LEFT, OUTPUT); pinMode(EN_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT); pinMode(EN_RIGHT, OUTPUT);
  stopBoth(); // Brake on startup

  Serial.begin(9600);
  // while (!Serial && millis() < 10000); // Can be commented out to speed up startup

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  if (groupNumber) WiFi.config(IPAddress(192, 168, 0, groupNumber + 1));

  Serial.print("Connecting to SSID: "); Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(500); Serial.print('.');
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  // Routing
  server.on("/", handleRoot);
  server.on("/forward", moveForward);
  server.on("/backward", moveBackward);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/stop", stopRover);
  
  server.begin();
}

void loop() {
  server.handleClient(); // Continuously listen for HTTP requests from the webpage
  
  // Ultimate Anti-Crash Watchdog Logic
  if (millis() - lastCmdTime > WATCHDOG_TIMEOUT) {
    stopBoth(); // Force stop if no command received for > 500ms
  }
}