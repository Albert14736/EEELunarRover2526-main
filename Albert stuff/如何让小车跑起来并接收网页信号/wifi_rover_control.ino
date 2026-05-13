#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#include <WiFiWebServer.h>

// ==========================================
// 1. 硬件引脚定义 (完全采用队友测试成功的方案)
// ==========================================
const int DIR_LEFT  = 2;  // 根据队友文档建议：原12号与WiFi冲突，改为2
const int EN_LEFT   = 3;  // 改为3
const int DIR_RIGHT = 9;  // 队友测试成功的右轮
const int EN_RIGHT  = 8;  // 队友测试成功的右轮

const int SPEED = 200; // 队友测试的有效速度 (0-255)，足够克服摩擦力

// ==========================================
// 2. WiFi 配置
// ==========================================
const char ssid[] = "EEERover";
const char pass[] = "exhibition";
const int groupNumber = 0; // 填入你们组的号码，可以将IP固定为 192.168.0.(groupNumber+1)

WiFiWebServer server(80);

// ==========================================
// 3. 极简版网页前端 (队友写好后替换这里)
// ==========================================
const char webpage[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <style>
    body { font-family: sans-serif; text-align: center; background: #222; color: #fff; margin-top: 50px; user-select: none; -webkit-user-select: none; }
    .grid { display: grid; grid-template-columns: repeat(3, 90px); grid-gap: 15px; justify-content: center; margin-top: 30px; }
    .btn { width: 90px; height: 90px; font-size: 20px; font-weight: bold; border-radius: 15px; border: none; background: #4CAF50; color: white; touch-action: manipulation; }
    .btn:active { background: #45a049; }
    .stop { background: #f44336; }
    .empty { background: transparent; }
  </style>
</head>
<body>
  <h2>Lunar Rover PRO</h2>
  <div class="grid">
    <div class="empty"></div>
    <button class="btn" onmousedown="startMove('/forward')" onmouseup="stopMove()" ontouchstart="startMove('/forward')" ontouchend="stopMove()">FWD</button>
    <div class="empty"></div>
    <button class="btn" onmousedown="startMove('/left')" onmouseup="stopMove()" ontouchstart="startMove('/left')" ontouchend="stopMove()">LFT</button>
    <button class="btn stop" onmousedown="stopMove()" ontouchstart="stopMove()">STOP</button>
    <button class="btn" onmousedown="startMove('/right')" onmouseup="stopMove()" ontouchstart="startMove('/right')" ontouchend="stopMove()">RGT</button>
    <div class="empty"></div>
    <button class="btn" onmousedown="startMove('/backward')" onmouseup="stopMove()" ontouchstart="startMove('/backward')" ontouchend="stopMove()">BWD</button>
    <div class="empty"></div>
  </div>
  <script>
    var timer;
    function sendCmd(cmd) {
      var xhttp = new XMLHttpRequest();
      xhttp.open('GET', cmd, true);
      xhttp.send();
    }
    function startMove(cmd) {
      sendCmd(cmd); // 立即发送第一次指令
      clearInterval(timer);
      timer = setInterval(function() { sendCmd(cmd); }, 200); // 随后每 200ms 发送一次心跳包防断联
    }
    function stopMove() {
      clearInterval(timer);
      sendCmd('/stop'); // 手指松开立刻发停车指令
    }
  </script>
</body>
</html>
)rawliteral";

// ==========================================
// 4. 核心驱动函数 (Backend Logic)
// ==========================================
unsigned long lastCmdTime = 0;
const unsigned long WATCHDOG_TIMEOUT = 500; // 500毫秒没收到任何信号，强制停车！

void handleRoot() { server.send(200, "text/html", webpage); }

// 队友写的神仙级控制函数：通过正负数自动判断方向
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

// 注意：队友提到"ends connected the other way around"（线接反了）
// 如果实际上车是往后开的，只要把下面的 SPEED 改成 -SPEED 即可！
void moveForward() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis(); // 喂狗 (重置超时时间)
  server.send(200, "text/plain", "Moving Forward");
}

void moveBackward() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
  lastCmdTime = millis(); // 喂狗
  server.send(200, "text/plain", "Moving Backward");
}

void turnLeft() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  lastCmdTime = millis(); // 喂狗
  server.send(200, "text/plain", "Turning Left");
}

void turnRight() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
  lastCmdTime = millis(); // 喂狗
  server.send(200, "text/plain", "Turning Right");
}

void stopRover() {
  stopBoth();
  server.send(200, "text/plain", "Stopped");
}

// ==========================================
// 5. 初始化与网络绑定
// ==========================================
void setup() {
  pinMode(DIR_LEFT, OUTPUT); pinMode(EN_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT); pinMode(EN_RIGHT, OUTPUT);
  stopBoth(); // 上电先刹车

  Serial.begin(9600);
  // while (!Serial && millis() < 10000); // 可以注释掉以加快上电启动速度

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

  // 路由绑定 (Routing)
  server.on("/", handleRoot);
  server.on("/forward", moveForward);
  server.on("/backward", moveBackward);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/stop", stopRover);
  
  server.begin();
}

void loop() {
  server.handleClient(); // 不断监听来自网页的点击信号
  
  // 终极防撞墙狗 (Watchdog) 逻辑
  if (millis() - lastCmdTime > WATCHDOG_TIMEOUT) {
    stopBoth(); // 超过 500ms 没收到指令，强制停车
  }
}