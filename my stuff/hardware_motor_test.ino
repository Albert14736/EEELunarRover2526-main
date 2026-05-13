// 1. Define Motor Pins (Safely avoiding WiFi reserved pins: 5, 7, 10)
// 1. 定义电机引脚 (完美避开 WiFi 模块占用的 5, 7, 10 引脚)
const int LEFT_DIR = 2;
const int LEFT_PWM = 3;
const int RIGHT_DIR = 4;
const int RIGHT_PWM = 6;

// Set testing speed (0-255). 
// 150 is a stable speed to prevent the rover from flying off the desk!
// 测试速度设定 (0-255)。150 是个比较稳的速度，防止小车飞下桌子！
const int SPEED = 150; 

void setup() {
  // Initialize all motor control pins as OUTPUT
  // 初始化所有电机控制引脚为输出模式
  pinMode(LEFT_DIR, OUTPUT);
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
}

void loop() {
  // --------------------------------------------------
  // 1. Move Forward (Both left and right directions are HIGH)
  // 1. 前进 (左右轮方向均设为 HIGH)
  // --------------------------------------------------
  digitalWrite(LEFT_DIR, HIGH);
  digitalWrite(RIGHT_DIR, HIGH);
  analogWrite(LEFT_PWM, SPEED);
  analogWrite(RIGHT_PWM, SPEED);
  delay(2000); // Run for 2 seconds / 运行 2 秒

  // --------------------------------------------------
  // 2. Stop (Set PWM signals to 0)
  // 2. 停止 (将 PWM 动力信号设为 0)
  // --------------------------------------------------
  analogWrite(LEFT_PWM, 0);
  analogWrite(RIGHT_PWM, 0);
  delay(1000); // Stop for 1 second / 停顿 1 秒

  // --------------------------------------------------
  // 3. Move Backward (Both left and right directions are LOW)
  // 3. 后退 (左右轮方向均设为 LOW)
  // --------------------------------------------------
  digitalWrite(LEFT_DIR, LOW);
  digitalWrite(RIGHT_DIR, LOW);
  analogWrite(LEFT_PWM, SPEED);
  analogWrite(RIGHT_PWM, SPEED);
  delay(2000); // Run for 2 seconds / 运行 2 秒

  // --------------------------------------------------
  // 4. Stop
  // 4. 停止
  // --------------------------------------------------
  analogWrite(LEFT_PWM, 0);
  analogWrite(RIGHT_PWM, 0);
  delay(1000); // Stop for 1 second / 停顿 1 秒

  // --------------------------------------------------
  // 5. Spin Left (Left motor backward/LOW, Right motor forward/HIGH)
  // 5. 原地左转 (左轮后退 LOW，右轮前进 HIGH)
  // --------------------------------------------------
  digitalWrite(LEFT_DIR, LOW);
  digitalWrite(RIGHT_DIR, HIGH);
  analogWrite(LEFT_PWM, SPEED);
  analogWrite(RIGHT_PWM, SPEED);
  delay(2000); // Run for 2 seconds / 运行 2 秒

  // --------------------------------------------------
  // 6. Stop and Rest before the next loop
  // 6. 停止并休息，准备进入下一次循环
  // --------------------------------------------------
  analogWrite(LEFT_PWM, 0);
  analogWrite(RIGHT_PWM, 0);
  delay(3000); // Rest for 3 seconds / 休息 3 秒
}