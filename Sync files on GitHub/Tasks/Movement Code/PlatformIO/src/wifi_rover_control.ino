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

int SPEED = 200;  // 不再 const —— 由 /speed 滑块实时调节

// ==========================================
// IR (Infrared) Sensor — Chris 的红外: 中断计数, 算脉冲率分三档
// ==========================================
const int IR_PIN = 9;                  // D9 (空闲口; 避开电机 D2/3/4/6 与 WiFi D5/7/10)
volatile unsigned long pulseCnt = 0;   // 中断里自增 -> 必须 volatile
unsigned long lastIRTime = 0;
float IRpulseRate = 0;
void countPulse() { pulseCnt++; }      // ISR: 每个上升沿 +1

// ==========================================
// Magnetism Sensor — Devesh 的磁场: 模拟霍尔读 A4, 分上/下/无
// ==========================================
const int MAG_PIN = A4;                // A4 (空闲模拟口; ⚠️ 旧 sample 用 A0, 跟 Devesh 确认实际接线)

// ==========================================
// Radio / Age — Zifan 的年龄: Serial1(D0=RX) 600bps 收 ASCII, '#' 分隔每条读数
// ==========================================
const int AGE_BUF = 40;
char ageBuf[AGE_BUF];                   // 正在累积的一段 (定长 char, 不用 String 防堆碎片)
int  ageLen = 0;
char lastAge[AGE_BUF] = "no signal";    // 最近一条完整读数 (网页 /age 展示)
unsigned long lastAgeCommit = 0;        // 最近一次「收完整一条读数」的时刻 (超时清屏基准)
const unsigned long AGE_TIMEOUT = 2000; // 判定③: 2s 没有新的完整读数(如石头拿开) -> 复位 "no signal"

// ==========================================
// Ultrasound Sensor — Wangmo 的超声: D8 数字读 40kHz 有/无
// ==========================================
const int US_PIN = 8;                   // D8 (空闲数字口; 原 snippet 用 pin2=左电机, 已改 D8) 

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
    .speed-control { margin-top: 25px; width: 80%; max-width: 300px; margin-left: auto; margin-right: auto; }
    .speed-control label { display: block; font-size: 14px; margin-bottom: 8px; color: #ccc; }
    #speed-slider { width: 100%; height: 30px; }
    #speed-num { width: 55px; background: #111; color: #4CAF50; border: 1px solid #555; border-radius: 5px; font-family: monospace; font-weight: bold; font-size: 14px; text-align: center; padding: 3px; }
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

  <div class="speed-control">
    <label>Speed: <input type="number" id="speed-num" min="60" max="255" step="1" value="200"> / 255</label>
    <input type="range" id="speed-slider" min="60" max="255" value="200">
  </div>

  <div class="dashboard">
    <div style="color: #fff; text-align: center; margin-bottom: 10px; font-weight: bold;">Sensor Dashboard</div>
    <div class="dash-item"><span class="dash-label">📡 Radio</span> <span class="dash-val" id="age-val">--</span></div>
    <div class="dash-item"><span class="dash-label">🧲 Magnetic</span> <span class="dash-val" id="mag-val">--</span></div>
    <div class="dash-item"><span class="dash-label">🔥 Infrared</span> <span class="dash-val" id="ir-val">--</span></div>
    <div class="dash-item"><span class="dash-label">🦇 Ultrasound</span> <span class="dash-val" id="us-val">Pending</span></div>
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

    // 轮询传感器 (红外 + 磁场 + 年龄), 更新仪表盘
    setInterval(function() {
      var irx = new XMLHttpRequest(); irx.open('GET', '/ir?t=' + new Date().getTime(), true); irx.timeout = 800;
      irx.onload = function() { if (irx.status == 200) document.getElementById('ir-val').innerHTML = irx.responseText; };
      irx.send();
      var mgx = new XMLHttpRequest(); mgx.open('GET', '/mag?t=' + new Date().getTime(), true); mgx.timeout = 800;
      mgx.onload = function() { if (mgx.status == 200) document.getElementById('mag-val').innerHTML = mgx.responseText; };
      mgx.send();
      var agx = new XMLHttpRequest(); agx.open('GET', '/age?t=' + new Date().getTime(), true); agx.timeout = 800;
      agx.onload = function() { if (agx.status == 200) document.getElementById('age-val').innerHTML = agx.responseText; };
      agx.send();
      var usx = new XMLHttpRequest(); usx.open('GET', '/us?t=' + new Date().getTime(), true); usx.timeout = 800;
      usx.onload = function() { if (usx.status == 200) document.getElementById('us-val').innerHTML = usx.responseText; };
      usx.send();
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
      if (e.target && e.target.tagName === 'INPUT') return; // 在数值框里打字时别触发开车
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

    // 调速: 滑块 + 数值框联动, 数值夹在 60–255 之间
    var speedSlider = document.getElementById('speed-slider');
    var speedNum = document.getElementById('speed-num');
    function clampSpeed(v) {
      v = parseInt(v, 10);
      if (isNaN(v)) v = parseInt(speedSlider.value, 10); // 空着/乱填 -> 回退当前值
      return Math.max(60, Math.min(255, v));             // >255 拉回 255, <60 拉到 60
    }
    function applySpeed(v) {
      speedSlider.value = v;
      speedNum.value = v;
      document.getElementById('status-box').innerHTML = "Speed set: " + v;
      var sx = new XMLHttpRequest();
      sx.open('GET', '/speed?v=' + v + '&t=' + new Date().getTime(), true);
      sx.send();
    }
    speedSlider.addEventListener('input', function() { speedNum.value = this.value; });            // 拖动实时同步数字
    speedSlider.addEventListener('change', function() { applySpeed(clampSpeed(this.value)); this.blur(); });
    speedNum.addEventListener('change', function() { applySpeed(clampSpeed(this.value)); });        // 回车/失焦时夹值并发送
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

void handleSpeed() {
  if (server.hasArg("v")) {
    SPEED = constrain(server.arg("v").toInt(), 60, 255); // 下限 60 防止电机堵转; 太低不动就把这个数调大
  }
  replyAPI(String(SPEED));
}

// 红外三档: <240 -> No IR, 240-400 -> 312, >400 -> 547 (右边再跟实测速率)
String irStatus() {
  if (IRpulseRate > 400)       return "547";
  else if (IRpulseRate >= 240) return "312";
  else                         return "No IR";
}
void handleIR() { replyAPI(irStatus() + " - " + String((int)IRpulseRate) + "/s"); }

// 磁场: >680 上(N), <560 下(S), 中间无 (右边跟原始 ADC 值)
String magStatus() {
  int v = analogRead(MAG_PIN);
  String dir;
  if (v > 680)      dir = "UP (North)";
  else if (v < 560) dir = "DOWN (South)";
  else              dir = "None";
  return dir + " - " + String(v);
}
void handleMag() { replyAPI(magStatus()); }

void handleAge() { replyAPI(String(lastAge)); }   // 返回最近一条年龄读数

// 超声: D8 数字读, HIGH=检测到 40kHz, LOW=无
String usStatus() { return digitalRead(US_PIN) == HIGH ? "Detected" : "None"; }
void handleUS() { replyAPI(usStatus()); }

void setup() {
  pinMode(DIR_LEFT, OUTPUT); pinMode(EN_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT); pinMode(EN_RIGHT, OUTPUT);
  stopBoth(); 
  Serial.begin(9600);
  Serial1.begin(600);     // Zifan 年龄: 石头 UART 600bps 从 D0(RX) 进
  pinMode(IR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), countPulse, RISING);
  pinMode(MAG_PIN, INPUT);
  pinMode(US_PIN, INPUT);
  WiFi.config(IPAddress(192, 168, 0, groupNumber + 1));
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) { delay(500); }
  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.on("/speed", handleSpeed);
  server.on("/ir", handleIR);
  server.on("/mag", handleMag);
  server.on("/age", handleAge);
  server.on("/us", handleUS);
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

  // 红外: 每 ~500ms 用实际经过时间算脉冲率 (count/time, 自带时间补偿)。窗口 200→500ms (Chris 更新: 计数更多, 速率更稳)
  unsigned long irElapsed = millis() - lastIRTime;
  if (irElapsed >= 500) {
    noInterrupts();
    unsigned long cnt = pulseCnt;
    pulseCnt = 0;
    interrupts();
    lastIRTime = millis();
    IRpulseRate = (float)cnt * 1000.0 / (float)irElapsed;
    if (Serial) { Serial.print("IR rate: "); Serial.print((int)IRpulseRate); Serial.print("/s -> "); Serial.println(irStatus()); }
  }

  // Radio/Age: 判定① 只收数字(非数字如 '?'、符号跳过); 判定② 换行/'#'(一条读数结束)才更新显示
  //            -> 网页稳定显示完整一条, 不再每来一个数字就闪半截。每轮限读 32 字节防卡死。
  int ageBudget = 32;
  while (Serial1.available() && ageBudget-- > 0) {
    char c = Serial1.read();
    if (c == '#' || c == '\n' || c == '\r') {     // 换行/# = 一条读数结束
      if (ageLen > 0) {                            // 判定②: 收完整了才把这一条更新到显示
        ageBuf[ageLen] = '\0';
        strncpy(lastAge, ageBuf, AGE_BUF - 1); lastAge[AGE_BUF - 1] = '\0';
        lastAgeCommit = millis();                  // 记下最近一条完整读数的时刻 (超时基准)
      }
      ageLen = 0;                                  // 开新一条
    } else if (c >= '0' && c <= '9') {             // 判定①: 只累积数字 (非数字 ?、符号跳过, 不立即刷新)
      if (ageLen < AGE_BUF - 1) ageBuf[ageLen++] = c;
    }
  }
  // 判定③: 超过 2s 没有新的完整读数 (石头拿开/没信号) -> 清屏 no signal, 不卡在上一条
  if (lastAgeCommit != 0 && (millis() - lastAgeCommit) > AGE_TIMEOUT && strcmp(lastAge, "no signal") != 0) {
    strncpy(lastAge, "no signal", AGE_BUF - 1); lastAge[AGE_BUF - 1] = '\0';
  }
}
