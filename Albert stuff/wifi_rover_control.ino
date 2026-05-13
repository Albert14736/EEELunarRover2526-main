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
const char webpage[] = \
"<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no\">" \
"<style>.btn{padding:20px 40px; margin:10px; font-size:20px; font-weight:bold; background-color:#4CAF50; color:white; border:none; border-radius:10px; touch-action:manipulation;}</style></head>" \
"<body style=\"text-align:center; background-color:#222; color:white; font-family:sans-serif; margin-top:50px;\">" \
"<h2>Lunar Rover Control</h2>" \
"<div>" \
"<button class=\"btn\" onmousedown=\"sendCmd('/forward')\" onmouseup=\"sendCmd('/stop')\" ontouchstart=\"sendCmd('/forward')\" ontouchend=\"sendCmd('/stop')\">Forward</button><br>" \
"<button class=\"btn\" onmousedown=\"sendCmd('/left')\" onmouseup=\"sendCmd('/stop')\" ontouchstart=\"sendCmd('/left')\" ontouchend=\"sendCmd('/stop')\">Left</button>" \
"<button class=\"btn\" style=\"background-color:#f44336;\" onmousedown=\"sendCmd('/stop')\" ontouchstart=\"sendCmd('/stop')\">STOP</button>" \
"<button class=\"btn\" onmousedown=\"sendCmd('/right')\" onmouseup=\"sendCmd('/stop')\" ontouchstart=\"sendCmd('/right')\" ontouchend=\"sendCmd('/stop')\">Right</button><br>" \
"<button class=\"btn\" onmousedown=\"sendCmd('/backward')\" onmouseup=\"sendCmd('/stop')\" ontouchstart=\"sendCmd('/backward')\" ontouchend=\"sendCmd('/stop')\">Backward</button>" \
"</div>" \
"<script>" \
"function sendCmd(cmd) { var xhttp = new XMLHttpRequest(); xhttp.open('GET', cmd, true); xhttp.send(); }" \
"</script>" \
"</body></html>";

// ==========================================
// 4. 核心驱动函数 (Backend Logic)
// ==========================================
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
  server.send(200, "text/plain", "Moving Forward");
}

void moveBackward() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
  server.send(200, "text/plain", "Moving Backward");
}

void turnLeft() {
  setMotor(DIR_LEFT, EN_LEFT, -SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, SPEED);
  server.send(200, "text/plain", "Turning Left");
}

void turnRight() {
  setMotor(DIR_LEFT, EN_LEFT, SPEED);
  setMotor(DIR_RIGHT, EN_RIGHT, -SPEED);
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
}