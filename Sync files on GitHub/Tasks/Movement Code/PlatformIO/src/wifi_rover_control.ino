#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#include <WiFiWebServer.h>

// ==========================================
// 1. Hardware Pin Definitions
// ==========================================
const int DIR_LEFT  = 2;  
const int EN_LEFT   = 3;  
const int DIR_RIGHT = 4;  // Pin 4 for DIR
const int EN_RIGHT  = 6;  // Pin 6 for PWM

const int SPEED = 200; 

// ==========================================
// 2. WiFi Configuration
// ==========================================
const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 10; 

WiFiWebServer server(80);

// ==========================================
// 3. Web UI Frontend (Stored in Flash memory)
// ==========================================
const char webpage[] PROGMEM = R"rawliteral(
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
    .btn.active-state { background: #45a049; }
    .stop { background: #f44336; }
    .empty { background: transparent; }
    #status-box { margin-top: 40px; padding: 15px; background: #111; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; font-family: monospace; color: #0f0; word-wrap: break-word;}
    #history-box { margin-top: 15px; padding: 15px; background: #1a1a1a; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; font-family: monospace; color: #aaa; height: 120px; overflow-y: auto; text-align: left; box-sizing: border-box; position: relative; transition: all 0.3s ease; }
    #history-box.expanded { position: fixed; top: 5%; left: 5%; right: 5%; bottom: 5%; width: 90%; height: 90%; max-width: none; z-index: 1000; font-size: 16px; box-shadow: 0 0 20px rgba(0,0,0,0.8); }
    .expand-btn { position: absolute; top: 10px; right: 10px; background: #333; color: #fff; border: 1px solid #555; padding: 3px 8px; border-radius: 5px; cursor: pointer; font-size: 12px; }
    .dashboard { margin-top: 20px; padding: 15px; background: #111; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; text-align: left; border: 1px solid #444; }
    .dash-item { margin: 10px 0; font-size: 15px; display: flex; justify-content: space-between; border-bottom: 1px dashed #333; padding-bottom: 5px; }
    .dash-label { color: #ccc; }
    .dash-val { color: #f44336; font-family: monospace; font-size: 12px; }
  </style>
</head>
<body oncontextmenu="return false;">
  <h2>Lunar Rover PRO<br><span style="font-size: 14px; color: #4CAF50;">(Stable Build V2)</span></h2>
  <div id="connection-indicator" style="font-size:18px; font-weight:bold; color:#f44336; margin-bottom:15px;">❌ Disconnected</div>
  <div class="grid">
    <button id="btn-fl" class="btn" onmousedown="startMove('/forward_left')" onmouseup="stopMove()" ontouchstart="startMove('/forward_left')" ontouchend="stopMove()">&#8598;</button>
    <button id="btn-fwd" class="btn" onmousedown="startMove('/forward')" onmouseup="stopMove()" ontouchstart="startMove('/forward')" ontouchend="stopMove()">&#9650;</button>
    <button id="btn-fr" class="btn" onmousedown="startMove('/forward_right')" onmouseup="stopMove()" ontouchstart="startMove('/forward_right')" ontouchend="stopMove()">&#8599;</button>
    <button id="btn-lft" class="btn" onmousedown="startMove('/left')" onmouseup="stopMove()" ontouchstart="startMove('/left')" ontouchend="stopMove()">&#9664;</button>
    <button id="btn-stp" class="btn stop" onmousedown="stopMove()" ontouchstart="stopMove()">STOP</button>
    <button id="btn-rgt" class="btn" onmousedown="startMove('/right')" onmouseup="stopMove()" ontouchstart="startMove('/right')" ontouchend="stopMove()">&#9654;</button>
    <button id="btn-bl" class="btn" onmousedown="startMove('/backward_left')" onmouseup="stopMove()" ontouchstart="startMove('/backward_left')" ontouchend="stopMove()">&#8601;</button>
    <button id="btn-bwd" class="btn" onmousedown="startMove('/backward')" onmouseup="stopMove()" ontouchstart="startMove('/backward')" ontouchend="stopMove()">&#9660;</button>
    <button id="btn-br" class="btn" onmousedown="startMove('/backward_right')" onmouseup="stopMove()" ontouchstart="startMove('/backward_right')" ontouchend="stopMove()">&#8600;</button>
  </div>
  <div class="dashboard">
    <div style="color: #fff; text-align: center; margin-bottom: 10px; font-weight: bold;">Sensor Dashboard</div>
    <div class="dash-item"><span class="dash-label">📡 Radio (Age)</span> <span class="dash-val">Wait for Zifan...</span></div>
    <div class="dash-item"><span class="dash-label">🔥 Infrared</span> <span class="dash-val">Wait for Chris...</span></div>
    <div class="dash-item"><span class="dash-label">🦇 Ultrasound</span> <span class="dash-val">Wait for Wangmo...</span></div>
    <div class="dash-item"><span class="dash-label">🧲 Magnetic</span> <span class="dash-val">Wait for Devesh...</span></div>
  </div>
  <div id="status-box">Status: Ready to connect...</div>
  <div id="history-box">
    <button class="expand-btn" onclick="toggleHistory()">Expand</button>
    <div style="color: #fff; border-bottom: 1px solid #444; padding-bottom: 5px; margin-bottom: 5px; text-align: center;">Command History</div>
    <div id="history-content"></div>
  </div>
  <script>
    var timer; var heartbeatCount = 0; var currentActionName = "";
    var statusBox = document.getElementById('status-box');
    var historyContent = document.getElementById('history-content');
    function getActionName(cmd) {
      if(cmd === '/forward') return 'Forward'; if(cmd === '/backward') return 'Backward';
      if(cmd === '/left') return 'Left'; if(cmd === '/right') return 'Right';
      if(cmd === '/forward_left') return 'Forward Left'; if(cmd === '/forward_right') return 'Forward Right';
      if(cmd === '/backward_left') return 'Backward Left'; if(cmd === '/backward_right') return 'Backward Right';
      return 'Stop';
    }
    function sendCmd(cmd) {
      if (cmd !== '/stop') heartbeatCount++;
      statusBox.innerHTML = "Signal: " + cmd + "<br>Packets: " + heartbeatCount;
      var xhttp = new XMLHttpRequest(); xhttp.open('GET', cmd, true); xhttp.send();
    }
    setInterval(function() {
      var pxhttp = new XMLHttpRequest(); pxhttp.open('GET', '/ping', true); pxhttp.timeout = 500;
      pxhttp.onload = function() {
        var ind = document.getElementById('connection-indicator');
        if (pxhttp.status === 200) { ind.style.color = '#4CAF50'; ind.innerHTML = '✅ Connected'; }
        else { ind.style.color = '#f44336'; ind.innerHTML = '❌ Disconnected'; }
      };
      pxhttp.onerror = pxhttp.ontimeout = function() { document.getElementById('connection-indicator').innerHTML = '❌ Disconnected'; };
      pxhttp.send();
    }, 1000);
    function logCurrentAction() {
      if (currentActionName !== "" && heartbeatCount > 0) {
        var logEntry = document.createElement('div');
        logEntry.innerHTML = "&gt; " + currentActionName + " <span style='color:#4CAF50;'>x" + heartbeatCount + "</span>";
        historyContent.insertBefore(logEntry, historyContent.firstChild);
      }
    }
    function startMove(cmd) {
      var newAction = getActionName(cmd);
      if (currentActionName !== "" && currentActionName !== newAction) logCurrentAction();
      currentActionName = newAction; heartbeatCount = 0;
      sendCmd(cmd); clearInterval(timer);
      timer = setInterval(function() { sendCmd(cmd); }, 200); 
    }
    function stopMove() { clearInterval(timer); logCurrentAction(); currentActionName = ""; sendCmd('/stop'); }
    function toggleHistory() { var box = document.getElementById('history-box'); box.classList.toggle('expanded'); }
    var keyState = { w:0, a:0, s:0, d:0, arrowup:0, arrowleft:0, arrowdown:0, arrowright:0 };
    function processKeys() {
      var fwd = keyState['w'] || keyState['arrowup']; var bwd = keyState['s'] || keyState['arrowdown'];
      var lft = keyState['a'] || keyState['arrowleft']; var rgt = keyState['d'] || keyState['arrowright'];
      var cmd = '/stop';
      if (fwd && lft) cmd = '/forward_left'; else if (fwd && rgt) cmd = '/forward_right';
      else if (fwd) cmd = '/forward'; else if (bwd) cmd = '/backward';
      else if (lft) cmd = '/left'; else if (rgt) cmd = '/right';
      if (cmd === '/stop') stopMove(); else startMove(cmd);
    }
    document.addEventListener('keydown', function(e) { var key = e.key.toLowerCase(); if (keyState[key] !== undefined && !keyState[key]) { keyState[key] = 1; processKeys(); e.preventDefault(); } });
    document.addEventListener('keyup', function(e) { var key = e.key.toLowerCase(); if (keyState[key] !== undefined) { keyState[key] = 0; processKeys(); e.preventDefault(); } });
  </script>
</body>
</html>
)rawliteral";

// ==========================================
// 4. Core Motor Functions (Backend Logic)
// ==========================================
unsigned long lastCmdTime = 0;
const unsigned long WATCHDOG_TIMEOUT = 500; 

void replyAPI(const String& msg) {
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*")); 
  server.send(200, F("text/plain"), msg);
}

void setMotor(int dirPin, int enPin, int speed) {  
  if (speed == 0) {
    digitalWrite(dirPin, LOW);
    analogWrite(enPin, 0); 
  } else if (speed > 0) {  
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

void moveForward() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis(); 
  replyAPI(F("Moving Forward"));
}

void moveBackward() {
  stopBoth(); // 暂时禁用后退
  lastCmdTime = millis(); 
  replyAPI(F("Backward Disabled"));
}

void turnLeft() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis(); 
  replyAPI(F("Turning Left"));
}

void turnRight() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
  lastCmdTime = millis(); 
  replyAPI(F("Turning Right"));
}

void curveForwardLeft() {
  setMotor(DIR_LEFT, EN_LEFT, (SPEED * 3) / 4); 
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);   
  lastCmdTime = millis();
  replyAPI(F("Curve Forward Left"));
}

void curveForwardRight() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, (SPEED * 3) / 4);
  lastCmdTime = millis();
  replyAPI(F("Curve Forward Right"));
}

void stopRover() {
  stopBoth();
  replyAPI(F("Stopped"));
}

void handleRoot() { 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  int totalLength = sizeof(webpage) - 1;
  int bytesSent = 0;
  const int CHUNK_SIZE = 1024;
  char chunkBuffer[CHUNK_SIZE + 1];
  while (bytesSent < totalLength) {
    int currentChunkSize = (totalLength - bytesSent > CHUNK_SIZE) ? CHUNK_SIZE : totalLength - bytesSent;
    memcpy_P(chunkBuffer, webpage + bytesSent, currentChunkSize);
    chunkBuffer[currentChunkSize] = '\0';
    server.sendContent(chunkBuffer);
    bytesSent += currentChunkSize;
    delay(10); 
  }
  server.sendContent("");
}

void handlePing() { server.send(200, F("text/plain"), F("pong")); }

// ==========================================
// 5. Initialization & Network Binding
// ==========================================
void setup() {
  pinMode(DIR_LEFT, OUTPUT); pinMode(EN_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT); pinMode(EN_RIGHT, OUTPUT);
  stopBoth(); 

  Serial.begin(9600);
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true);
  }

  WiFi.config(IPAddress(192, 168, 0, groupNumber + 1));
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(500);
  }

  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.on("/forward", moveForward);
  server.on("/backward", moveBackward);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/forward_left", curveForwardLeft);
  server.on("/forward_right", curveForwardRight);
  server.on("/stop", stopRover);
  
  server.begin();
}

void loop() {
  server.handleClient(); 
  if (millis() - lastCmdTime > WATCHDOG_TIMEOUT) {
    stopBoth(); 
  }
}
