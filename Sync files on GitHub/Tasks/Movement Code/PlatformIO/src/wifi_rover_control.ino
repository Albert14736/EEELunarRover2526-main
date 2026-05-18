#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#include <WiFiWebServer.h>

// ==========================================
// 1. Hardware Pin Definitions (功能对调，适配物理接线)
// ==========================================
// 之前测试发现：物理上的 PWM 孔连到了 Metro 的 D2/D4，DIR 孔连到了 D3/D6
// 所以我们在软件里把定义互换
const int EN_LEFT   = 2;  // 现在 D2 负责 PWM
const int DIR_LEFT  = 3;  // 现在 D3 负责 DIR
const int EN_RIGHT  = 4;  // 现在 D4 负责 PWM
const int DIR_RIGHT = 6;  // 现在 D6 负责 DIR

const int SPEED = 200; 

// ==========================================
// 2. WiFi Configuration
// ==========================================
const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 10; 

WiFiWebServer server(80);

// ==========================================
// 3. Web UI Frontend 
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
    .btn { width: 90px; height: 90px; font-size: 20px; font-weight: bold; border-radius: 15px; border: none; background: #4CAF50; color: white; touch-action: manipulation; }
    .btn:active { background: #45a049; }
    .btn.active-state { background: #45a049; }
    .stop { background: #f44336; }
    #status-box { margin-top: 40px; padding: 15px; background: #111; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; font-family: monospace; color: #0f0; word-wrap: break-word;}
    #history-box { margin-top: 15px; padding: 15px; background: #1a1a1a; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; font-family: monospace; color: #aaa; height: 120px; overflow-y: auto; text-align: left; box-sizing: border-box; position: relative; }
    .expand-btn { position: absolute; top: 10px; right: 10px; background: #333; color: #fff; border: 1px solid #555; padding: 3px 8px; border-radius: 5px; cursor: pointer; font-size: 10px; }
    .dashboard { margin-top: 20px; padding: 15px; background: #111; border-radius: 10px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; text-align: left; border: 1px solid #444; }
    .dash-item { margin: 10px 0; font-size: 13px; display: flex; justify-content: space-between; border-bottom: 1px dashed #333; padding-bottom: 5px; }
    .dash-label { color: #ccc; }
    .dash-val { color: #f44336; font-family: monospace; }
  </style>
</head>
<body oncontextmenu="return false;">
  <h2>Lunar Rover PRO V5</h2>
  <div id="connection-indicator" style="font-size:18px; font-weight:bold; color:#f44336; margin-bottom:15px;">❌ Disconnected</div>

  <div class="grid">
    <button id="btn-fl" class="btn" onmousedown="startMove('/forward_left')" ontouchstart="startMove('/forward_left')">&#8598;</button>
    <button id="btn-fwd" class="btn" onmousedown="startMove('/forward')" ontouchstart="startMove('/forward')">&#9650;</button>
    <button id="btn-fr" class="btn" onmousedown="startMove('/forward_right')" ontouchstart="startMove('/forward_right')">&#8599;</button>
    <button id="btn-lft" class="btn" onmousedown="startMove('/left')" ontouchstart="startMove('/left')">&#9664;</button>
    <button id="btn-stp" class="btn stop" onmousedown="stopMove()" ontouchstart="stopMove()">STOP</button>
    <button id="btn-rgt" class="btn" onmousedown="startMove('/right')" ontouchstart="startMove('/right')">&#9654;</button>
    <button id="btn-bl" class="btn" onmousedown="startMove('/backward_left')" ontouchstart="startMove('/backward_left')">&#8601;</button>
    <button id="btn-bwd" class="btn" onmousedown="startMove('/backward')" ontouchstart="startMove('/backward')">&#9660;</button>
    <button id="btn-br" class="btn" onmousedown="startMove('/backward_right')" ontouchstart="startMove('/backward_right')">&#8600;</button>
  </div>

  <div class="dashboard">
    <div style="color: #fff; text-align: center; margin-bottom: 10px; font-weight: bold;">Sensor Dashboard</div>
    <div class="dash-item"><span class="dash-label">📡 Radio</span> <span class="dash-val">Active</span></div>
    <div class="dash-item"><span class="dash-label">🧲 Magnetic</span> <span class="dash-val">Scanning...</span></div>
  </div>

  <div id="status-box">Status: Ready...</div>
  <div id="history-box">
    <div style="color: #fff; border-bottom: 1px solid #444; padding-bottom: 5px; margin-bottom: 5px; text-align: center; font-size: 12px;">Command History</div>
    <div id="history-content" style="font-size: 12px;"></div>
  </div>

  <script>
    var timer;
    var currentAction = "";
    var historyContent = document.getElementById('history-content');

    function sendCmd(cmd) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById('status-box').innerHTML = "Status: " + this.responseText;
        }
      };
      var url = cmd + "?t=" + new Date().getTime();
      xhttp.open('GET', url, true);
      xhttp.send();
    }

    setInterval(function() {
      var pxhttp = new XMLHttpRequest(); pxhttp.open('GET', '/ping?t=' + new Date().getTime(), true); pxhttp.timeout = 500;
      pxhttp.onload = function() { 
        document.getElementById('connection-indicator').innerHTML = '✅ Connected'; 
        document.getElementById('connection-indicator').style.color = '#4CAF50'; 
      };
      pxhttp.onerror = function() {
        document.getElementById('connection-indicator').innerHTML = '❌ Disconnected'; 
        document.getElementById('connection-indicator').style.color = '#f44336'; 
      };
      pxhttp.send();
    }, 1000);

    function startMove(cmd) { 
      if (currentAction !== cmd) {
        var log = document.createElement('div');
        log.innerHTML = "> " + cmd;
        historyContent.insertBefore(log, historyContent.firstChild);
        currentAction = cmd;
      }
      document.getElementById('status-box').innerHTML = "Sending: " + cmd;
      sendCmd(cmd); 
      clearInterval(timer); 
      timer = setInterval(function() { sendCmd(cmd); }, 200); 
    }

    function stopMove() { 
      clearInterval(timer); 
      currentAction = "";
      sendCmd('/stop'); 
    }

    // WASD & Arrow Keys Support
    var keyState = { w:0, a:0, s:0, d:0, arrowup:0, arrowleft:0, arrowdown:0, arrowright:0 };
    function processKeys() {
      var fwd = keyState['w'] || keyState['arrowup'];
      var bwd = keyState['s'] || keyState['arrowdown'];
      var lft = keyState['a'] || keyState['arrowleft'];
      var rgt = keyState['d'] || keyState['arrowright'];
      
      var cmd = "/stop";
      if (fwd && lft) cmd = "/forward_left";
      else if (fwd && rgt) cmd = "/forward_right";
      else if (bwd && lft) cmd = "/backward_left";
      else if (bwd && rgt) cmd = "/backward_right";
      else if (fwd) cmd = "/forward";
      else if (bwd) cmd = "/backward";
      else if (lft) cmd = "/left";
      else if (rgt) cmd = "/right";

      if (cmd === "/stop") stopMove(); else startMove(cmd);
    }

    document.addEventListener('keydown', function(e) {
      var key = e.key.toLowerCase();
      if (keyState[key] !== undefined && !keyState[key]) {
        keyState[key] = 1; processKeys();
      }
    });
    document.addEventListener('keyup', function(e) {
      var key = e.key.toLowerCase();
      if (keyState[key] !== undefined) {
        keyState[key] = 0; processKeys();
      }
    });

    document.addEventListener('mouseup', stopMove);
    document.addEventListener('touchend', stopMove);
  </script>
</body>
</html>
)rawliteral";

// ==========================================
// 4. Core Motor Functions
// ==========================================
unsigned long lastCmdTime = 0;
const unsigned long WATCHDOG_TIMEOUT = 500; 

void replyAPI(const String& msg) {
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*")); 
  server.send(200, F("text/plain"), msg);
}

void setMotor(int dirPin, int enPin, int speed) {  
  if (speed == 0) {
    analogWrite(enPin, 0); 
  } else if (speed > 0) {  
    digitalWrite(dirPin, HIGH); // 调整逻辑：现在 HIGH 对应前进
    analogWrite(enPin, speed);  
  } else {  
    digitalWrite(dirPin, LOW);  // 调整逻辑：现在 LOW 对应后退
    analogWrite(enPin, -speed); 
  }  
}

void stopBoth() { setMotor(DIR_LEFT, EN_LEFT, 0); setMotor(DIR_RIGHT, EN_RIGHT, 0); }
void moveForward() { 
  Serial.println("Cmd: Forward");
  setMotor(DIR_LEFT, EN_LEFT, SPEED); setMotor(DIR_RIGHT, EN_RIGHT, SPEED); 
  lastCmdTime = millis(); replyAPI(F("Fwd")); 
}
void moveBackward() { 
  Serial.println("Cmd: Backward");
  setMotor(DIR_LEFT, EN_LEFT, -SPEED); setMotor(DIR_RIGHT, EN_RIGHT, -SPEED); 
  lastCmdTime = millis(); replyAPI(F("Rev")); 
}
void turnLeft() { 
  Serial.println("Cmd: Left");
  setMotor(DIR_LEFT, EN_LEFT, -SPEED); setMotor(DIR_RIGHT, EN_RIGHT, SPEED); 
  lastCmdTime = millis(); replyAPI(F("Left")); 
}
void turnRight() { 
  Serial.println("Cmd: Right");
  setMotor(DIR_LEFT, EN_LEFT, SPEED); setMotor(DIR_RIGHT, EN_RIGHT, -SPEED); 
  lastCmdTime = millis(); replyAPI(F("Right")); 
}
void curveForwardLeft() { 
  int inner = (SPEED * 4) / 10;
  Serial.print("Cmd: Curve Fwd Left (L:"); Serial.print(inner); Serial.println(", R:200)");
  setMotor(DIR_LEFT, EN_LEFT, inner); setMotor(DIR_RIGHT, EN_RIGHT, SPEED); 
  lastCmdTime = millis(); replyAPI("C-FL (" + String(inner) + "/200)"); 
}
void curveForwardRight() { 
  int inner = (SPEED * 4) / 10;
  Serial.print("Cmd: Curve Fwd Right (L:200, R:"); Serial.print(inner); Serial.println(")");
  setMotor(DIR_LEFT, EN_LEFT, SPEED); setMotor(DIR_RIGHT, EN_RIGHT, inner); 
  lastCmdTime = millis(); replyAPI("C-FR (200/" + String(inner) + ")"); 
}
void curveBackwardLeft() { 
  int inner = (SPEED * 4) / 10;
  Serial.print("Cmd: Curve Bwd Left (L:"); Serial.print(-inner); Serial.println(", R:-200)");
  setMotor(DIR_LEFT, EN_LEFT, -inner); setMotor(DIR_RIGHT, EN_RIGHT, -SPEED); 
  lastCmdTime = millis(); replyAPI("C-BL (" + String(-inner) + "/-200)"); 
}
void curveBackwardRight() { 
  int inner = (SPEED * 4) / 10;
  Serial.print("Cmd: Curve Bwd Right (L:-200, R:"); Serial.print(-inner); Serial.println(")");
  setMotor(DIR_LEFT, EN_LEFT, -SPEED); setMotor(DIR_RIGHT, EN_RIGHT, -inner); 
  lastCmdTime = millis(); replyAPI("C-BR (-200/" + String(-inner) + ")"); 
}
void stopRover() { 
  Serial.println("Cmd: Stop");
  stopBoth(); replyAPI(F("Stop")); 
}

void handleRoot() { 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");
  int totalLength = sizeof(webpage) - 1;
  int bytesSent = 0;
  const int CHUNK_SIZE = 512;
  char chunkBuffer[CHUNK_SIZE + 1];
  while (bytesSent < totalLength) {
    int currentChunkSize = (totalLength - bytesSent > CHUNK_SIZE) ? CHUNK_SIZE : totalLength - bytesSent;
    memcpy_P(chunkBuffer, webpage + bytesSent, currentChunkSize);
    chunkBuffer[currentChunkSize] = '\0';
    server.sendContent(chunkBuffer);
    bytesSent += currentChunkSize;
    delay(20); 
  }
  server.sendContent("");
}

void handlePing() { server.send(200, F("text/plain"), F("pong")); }

void setup() {
  pinMode(DIR_LEFT, OUTPUT); pinMode(EN_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT); pinMode(EN_RIGHT, OUTPUT);
  stopBoth(); 
  Serial.begin(9600);
  WiFi.config(IPAddress(192, 168, 0, groupNumber + 1));
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) { delay(500); }
  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.on("/forward", moveForward);
  server.on("/backward", moveBackward);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/forward_left", curveForwardLeft);
  server.on("/forward_right", curveForwardRight);
  server.on("/backward_left", curveBackwardLeft);
  server.on("/backward_right", curveBackwardRight);
  server.on("/stop", stopRover);
  server.begin();
}

void loop() {
  server.handleClient(); 
  if (millis() - lastCmdTime > WATCHDOG_TIMEOUT) { stopBoth(); }
}
