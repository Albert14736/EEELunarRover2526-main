# EEELunarRover (探月车 / Lunar Rover) 项目理解与思考

## 1. 项目核心目标 (Project Core Objective)
造一辆基于 WiFi 遥控的小车 (Remote-controlled rover)，在模拟场地上行驶，通过不同的传感器探测并解码“岩石 (Rocks)”发出的信号，从而判断岩石的**年龄 (Age)**和**种类 (Type)**。

> 🗣️ **English Script (Read aloud / Show to team):**
> "Our main goal is to build a remote-controlled lunar rover via WiFi. It needs to navigate a simulated lunar surface, detect rocks, and decode their signals to find out their age and type."

## 2. 物理与硬件限制 (Physical & Hardware Constraints)
- **大脑**: 主板/微控制器 (Microcontroller / MCU) 使用 Adafruit Metro M0，加上 WiFi 扩展板 (WiFi Shield) 来实现无线遥控。
- **肌肉**: 提供现成的双路 H桥电机驱动模块 (H-Bridge Motor Driver)，用来控制车轮前进和后退。
- **重量红线 (Weight Limit)**: 整车重量**绝不能超过 750克**，否则会陷进场地的“对重量敏感的橡胶地板区 (Weight Sensitive Zone)”，任务直接失败。
- **预算 (Budget)**: 全组 £60。前期桌面测试用面包板 (Breadboard)，后期要装配到结实的车身底盘 (Chassis) 上。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the hardware constraints: We are using the Adafruit Metro M0 with a WiFi shield. We have a dual H-bridge motor driver to control the wheels. The absolute weight limit is 750 grams, otherwise, the rover will get stuck in the weight-sensitive zone. And our total budget is £60, so we must be careful with our component choices."

## 3. 岩石探测四大模块 (The 4 Detection Modules)

### 3.1 测年龄: 无线电 (Radio) —— “收音机”原理
- **概念**: 岩石会一直对外广播 89kHz 的无线电波 (Carrier Frequency)。有波代表 1，无波代表 0，这叫开关键控 (On-Off Keying / OOK)。发出的信息是串口协议 (UART) 的字符串，如 `#123`。
- **硬件流水线**:
  1. **天线**: 绕一个空心线圈 (Air-cored inductor) 加电容，做成谐振电路 (Tuned Circuit)，只接收 89kHz 信号。
  2. **放大**: 用运算放大器 (Op-Amp) 把微弱信号放大。
  3. **解调 (剥皮)**: 用包络检波器 / 精密整流器 (Envelope Detector / Precision Rectifier)。二极管 (Diode) 砍掉负波，电容 (Capacitor) 填平缝隙，提取低频轮廓。
  4. **整形**: 比较器 (Comparator) 把轮廓切成方波。
  5. **解码**: 主板 (MCU) 串口读取。

> 🗣️ **English Script (Read aloud / Show to team):**
> "To determine the rock's age, it transmits an 89kHz radio signal using On-Off Keying (OOK) modulation. We need to build a tuned circuit with an air-cored inductor to receive it. Then, we use an op-amp to amplify the weak signal, an envelope detector to extract the low-frequency data, and a comparator to convert it into a square wave. Finally, our microcontroller will decode the UART string, like '#123'."

### 3.2 测种类特征一: 红外线 (Infrared) —— 测“闪烁频率”
- **概念**: 岩石在模拟放射性，会随机发出极短 (50μs) 的红外光脉冲。
- **目标**: 写代码或搭电路统计它一秒钟闪多少次，也就是泊松分布脉冲率 (Poisson Distribution Pulse Rate)。高频约 547 次/秒，低频约 312 次/秒。
- **坑点**: 室内的照明灯光 (Ambient light) 会有 100Hz 的频闪干扰。必须加滤光片或在代码/电路里过滤掉 (Filter out)。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the first type characteristic, the rocks emit random infrared pulses following a Poisson distribution. High radiation is about 547 pulses per second, and low is 312. The main challenge here is to filter out the 100Hz ambient light interference from the room lights, either by using a physical optical filter or doing it in our software."

### 3.3 测种类特征二: 超声波 (Ultrasound) —— 听“海豚音”
- **概念**: 岩石要么发射 40kHz 的高频声波，要么不发射。人耳听不见。
- **目标**: 给小车装个超声波传感器 (Ultrasonic transducer) 充当耳朵，只判断“有信号 (Presence)”还是“无信号 (Absence)”。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the ultrasound feature, we just need to detect the presence or absence of a 40kHz signal using an ultrasonic transducer. It's a simple binary check: is there a 40kHz sound or not?"

### 3.4 测种类特征三: 磁场 (Magnetic) —— 当“指南针”
- **概念**: 岩石里面埋了一块永久磁铁。这是静态磁场 (Static magnetic field)，不是会动的波。
- **目标**: 就像用指南针一样，判断磁场的 N极 是朝上 (Up) 还是 朝下 (Down)。
- **坑点**: 官方没有教这个，需要自己买传感器。通常会用 霍尔效应传感器 (Hall effect sensor)，它能根据磁场方向输出不同的电压。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For magnetism, there is a static permanent magnet inside the rock. We need to buy a magnetic sensor, like a Hall effect sensor, to determine if the magnetic field is pointing UP or DOWN."

## 4. 小车怎么动起来？(How it drives)
- **遥控**: 主板上插了 WiFi 模块，它会在局域网里开个小网页 (Webpage)。你用手机/电脑连上，点网页按钮就能下发指令。
- **动力**: 主板给电机驱动器发两种信号：一个是高低电平 (DIR) 决定轮子正转还是反转；另一个是连续跳动的脉冲 (PWM) 决定轮子转多快。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For driving and mobility, the WiFi shield will host a webpage so we can control the rover from our phones or laptops. The Metro microcontroller will send DIR and PWM signals to the motor driver module to control the direction and speed of the wheels."

## 5. 明天开会时的工作分配建议 (Task Allocation)
一个组 6 个人，这个项目基本可以天然划分为以下几个研发方向：
1. **无线电/模电组**: 专攻 89kHz 天线接收、运放 (Op-Amp) 和包络检波 (Envelope Detector) 的纯硬件电路。这是最容易翻车的地方。
2. **多传感器组**: 搞定红外的滤波放大、超声波检测和磁场传感器选型。
3. **单片机代码组**: 负责写 C++ 代码，通过串口 (UART) 解码无线电，通过统计算法算红外闪烁频率 (Poisson distribution)，以及控制小车。
4. **机械/集成组**: 负责控制 750g 的重量，设计 CAD 并用激光切割机 (Laser cutter) 制作外壳底盘，把所有人的电路焊到一起。

> 🗣️ **English Script (Read aloud / Show to team):**
> "Since we have 6 team members, I suggest we divide into 4 main sub-teams. First, the Radio & Analog team, focusing on the 89kHz antenna and envelope detector. Second, the Multi-sensor team for infrared, ultrasound, and the magnetic sensor. Third, the Software team for C++ coding, UART decoding, and WiFi control. Finally, the Mechanical & Integration team to manage the 750g weight limit and design the laser-cut chassis."

## 6. 头脑风暴与技术备选方案 (Brainstorming & Alternative Solutions)
*留作开会讨论使用，每个难点提供不同维度的解决思路，讨论出最优解后再做决定。*

### 6.1 89kHz 无线电解码 (Radio Demodulation)
- **思路A: 纯正规军模电法 (The Classic Analog Way)**。也就是官方推荐的：线圈 -> 运放放大 -> 精密整流(包络检波) -> 比较器 -> 单片机串口。最稳妥，但需要的芯片多，电路调试花时间。**【预计预算: £2 - £5】**(运放芯片如LM358/TL072通常不到£1，二极管电容几便士，成本极低)。
- **思路B: 暴力数字降维法 (The Digital DSP Way)**。只用运放把接收到的 89kHz 信号放大到 0~3.3V，然后直接硬怼给单片机的 ADC (模拟转数字引脚) 或者高频中断引脚。用代码去捕捉高频波，在软件里写算法完成“剥皮”。省硬件，但极度考验写代码人的水平，可能挤占单片机资源。**【预计预算: £1 - £2】**(只需要一级或两级便宜的运放放大，省下了后面的检波和比较器芯片)。
- **思路C: 偷懒模拟芯片法 (The IC Cheat Way)**。去查查有没有现成的 AM(调幅) 收音机解调芯片。虽然频率不一定是常见的广播频段，但如果有频段支持 89kHz 附近的现成解调 IC，直接买来用，能省下巨量搭运放的时间。**【预计预算: £3 - £8】**(专用芯片通常较贵，且可能需要付额外运费，找现货有风险)。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the 89kHz radio decoding, we have three ideas. Plan A is the classic analog way using op-amps, an envelope detector, and a comparator. It's safe and costs about £2 to £5. Plan B is a digital DSP approach: just amplify the signal and use the microcontroller's ADC to decode it in software, costing only £1 to £2. Plan C is to find a dedicated AM radio decoder IC that supports 89kHz, which might cost £3 to £8 but saves time."

### 6.2 红外线测频率 & 滤除100Hz灯光干扰 (Infrared Pulse Counting & Noise Filtering)
- **思路A: 物理防御法 (Optical Filter)**。去找一块只能让 950nm 红外光透过的滤光玻璃（或者最土的办法：找一截曝光过变黑的旧胶卷/拆个旧软盘的磁片），贴在红外接收管前面。直接把室内的可见照明光物理屏蔽掉，从源头消灭 100Hz 干扰。**【预计预算: £0 - £3】**(废物利用就是£0，如果去买正规红外亚克力滤光片可能要几镑)。
- **思路B: 硬件魔法滤镜 (Active Band-pass Filter)**。既然石头闪烁频率是 312Hz 和 547Hz，用运放搭一个中心频率在 400Hz 左右的带通滤波器，把 100Hz 的低频灯光噪音全部滤掉，然后接进单片机的外部中断引脚 (Hardware Interrupt) 计数。**【预计预算: £1 - £3】**(需要额外的运放芯片和高精度电阻电容)。
- **思路C: 软件数学硬算 (Software Subtraction)**。如果滤不掉 100Hz，就干脆一起读进来。用代码计算每秒所有的脉冲总数，再减去固定的 100（或者测算出来的环境基准值），剩下的就是石头的频率。**【预计预算: £0】**(纯代码解决，零成本，强推作为首选尝试方案！)。

> 🗣️ **English Script (Read aloud / Show to team):**
> "To filter out the 100Hz room light noise from the infrared signal, we have three plans. Plan A: Use a physical optical filter, like a piece of exposed black film, over the sensor. Cost is almost zero. Plan B: Build an active band-pass filter circuit around 400Hz using op-amps. Cost: £1 to £3. Plan C: Read everything and subtract the 100Hz noise mathematically in the software. This is free, so we should try it first."

### 6.3 磁场方向判断 (Magnetic Polarity)
- **思路A: 模拟霍尔传感器 (Analog Hall Effect Sensor)**。这种芯片能输出模拟电压，比如没磁场时输出 1.6V，N极靠近变 3.3V，S极靠近变 0V。用单片机 ADC 读电压大小判断方向。精度高。**【预计预算: £1 - £2】**(单颗线性霍尔如 SS39E/SS49E 非常便宜)。
- **思路B: 开关型霍尔 (Digital/Latch Hall Sensor)**。用两个只认单极性的数字霍尔芯片，一个专门感应 N极 亮，一个专门感应 S极 亮。直接输出 0 或 1，单片机读取极其简单，完全不需要算电压。**【预计预算: £1 - £3】**(需要买两个不同极性的开关霍尔，略贵一点点，但代码最好写)。
- **思路C: I2C 电子罗盘模块 (Magnetometer Breakout)**。去淘宝或电子商城买一个现成的三轴磁力计模块（比如 HMC5883L），走 I2C 通信，精度吊打一切，甚至还能顺便当小车的指南针用，但可能有点大材小用。**【预计预算: £4 - £10】**(现成的I2C模块比较贵，会显著吃掉预算，非土豪慎选)。
- **思路D: 电磁感应法 (Electromagnetic Induction / Moving Coil) - ❌ 避坑警告 (Trap Warning)**。利用线圈切割磁感线产生电流。**强烈不推荐**！因为石头里是静态磁场，只有小车移动时才会产生微弱电流，小车一停电流就是 0。而且极难判断 Up/Down 极性，官方指南也专门警告了其不可靠性。

> 🗣️ **English Script (Read aloud / Show to team):**
> "To check the magnetic direction, Plan A is to use an analog Hall effect sensor and read the voltage via ADC, costing £1 to £2. Plan B is to use two digital latch Hall sensors, one for North and one for South, costing £1 to £3. Plan C is an I2C Magnetometer module, costing £4 to £10. Also, we should absolutely avoid using a simple coil (Electromagnetic Induction). The official guide warns that since the magnetic field is static, a coil will only work if the rover is moving constantly, which is highly unreliable for detecting Up/Down polarity."

---

## 7. 项目进度追踪 (Project Progress Tracker)

- [x] **Phase 0:** 项目需求理解与框架梳理 (Project brief understanding & architecture breakdown).
- [ ] **Phase 1 (当前/Current):** 搭建基础底盘，建立 WiFi 网页服务器，实现小车的基础遥控与电机驱动 (Build basic chassis, host WiFi web server, and implement motor driving).
  - [x] 硬件连接与组装 (Hardware wiring & assembly)
  - [ ] 基础编程与输出调试 (Basic programming & output debugging)
- [ ] **Phase 2:** 待定 (TBD)

> 🗣️ **English Script (Read aloud / Show to team):**
> "For our current progress, we are in Phase 1. Our goal is to build the basic chassis, host a simple web page using the WiFi shield, and get the motors running so the rover can drive."

---

## 8. 我的专属任务清单 (My Personal Task List)

- [x] **任务 1: 硬件连接 (Hardware Wiring)** - 已完成！(Completed)
- [ ] **任务 2: 基础编程与输出 (Basic Programming & Output)** - 正在进行中 (In Progress)
  - 目标: 编写单片机基础代码，实现小车的基本控制逻辑和串口输出。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For my specific tasks, I have already finished the hardware wiring. My next step is to work on the basic programming and output for the microcontroller. I will be reviewing and writing the core control logic now."

---

## 9. 官方 README 避坑指南 (Official README Pitfalls)
在进行具体电路和代码实现时，必须时刻牢记以下官方警告：
1. **3.3V 逻辑红线 (Strict 3.3V Logic)**: Metro M0 是 3.3V 系统。绝对不能把 5V 的模拟电路或传感器信号直接接入引脚，必须使用分压电阻 (Potential dividers)，否则会烧毁主板。
2. **WiFi 引脚冲突 (WiFi Pin Conflicts)**: WiFi 扩展板物理占用了 **Pin 5, 7, 10**。规划电机和传感器引脚时，**必须避开这三个引脚**。
3. **双串口优势 (Two UARTs)**: 主板有两个独立串口。用 `Serial` 连接 USB 打印电脑调试信息，用 `Serial1` (Pin 0/RX) 读取岩石信号，完美分离。
4. **红外脉冲被吞噬风险 (Infrared Pulse Smoothing)**: 红外脉冲极窄 (50μs)。如果硬件滤波电路电容过大 (Low-pass filter)，会直接把 50μs 的脉冲抹平导致漏测。需权衡响应速度与抗干扰能力。
5. **激光切割误差 (Laser Kerf)**: 激光切割会烧掉 0.1~0.4mm 材料。CAD 图纸中的孔和插槽必须做尺寸补偿，否则组装会松动。

> 🗣️ **English Script (Read aloud / Show to team):**
> "Based on the official README, we must remember a few critical rules. First, the Metro M0 is strictly 3.3V; 5V will fry it. Second, pins 5, 7, and 10 are reserved for WiFi, we cannot use them. Third, the 50-microsecond infrared pulses are so short that heavy low-pass filtering might accidentally erase them. Lastly, for chassis CAD design, we must account for the 0.1 to 0.4mm laser kerf."

---

## 10. 基础底盘电机连线方案 (Motor Driver Wiring Plan)
官方提供的 H桥电机驱动模块 控制的是“双轨差速转向 (Differential / Skid Steering)”，即通过左右轮的速度和方向差来实现转弯（如坦克原地打转）。

### 引脚功能与共地 (Pin Functions & Common Ground)
- **GND (地线)**: **极其重要！** 必须在面包板上规划一条“公共地线轨 (Common Ground Rail)”，将单片机的 GND 和电机驱动的 GND 连通，否则信号没有参考电压，电机绝对不会动。
- **DIR (方向)**: 接收普通数字信号 (Digital I/O)。给高电平 (HIGH) 正转，低电平 (LOW) 反转。
- **EN (使能/速度)**: 必须接单片机支持 **PWM (脉冲宽度调制)** 的引脚。通过代码 `analogWrite(pin, 0~255)` 发送脉冲来控制车速，绝不能直接给死电平。

### 黄金连线方案 (The Golden Pinout)
为了完美避开 WiFi 模块霸占的 **Pin 5, 7, 10**，推荐以下紧凑且安全的连线方案：
- **左轮 (Left Motor)**: `DIR_A` 接 **Pin 2**, `EN_A` (PWM) 接 **Pin 3**
- **右轮 (Right Motor)**: `DIR_B` 接 **Pin 4**, `EN_B` (PWM) 接 **Pin 6**

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the motor driver, it uses differential steering. We have DIR pins for direction (HIGH/LOW) and EN pins for speed, which must connect to PWM-capable pins. Crucially, we must create a Common Ground rail on the breadboard connecting the microcontroller and the motor driver. To safely avoid the WiFi pins (5, 7, and 10), here is our wiring plan: Left Motor DIR to Pin 2, EN to Pin 3. Right Motor DIR to Pin 4, EN to Pin 6."

### 6.4 40kHz 超声波检测 (Ultrasonic Presence)
- **思路A: 包络检波复用**。和 89kHz 无线电一模一样，把接收超声波的探头当成天线，放大后用检波电路变成直流电平，单片机判断高低电平。**【预计预算: £3 - £5】**(超声波接收头通常£2左右，加上运放和检波元器件)。
- **思路B: 音频解码神仙芯片 (Tone Decoder - LM567)**。电子工程里的经典老芯片 **LM567**！只需外接几个电阻电容调到 40kHz。只要它听到了 40kHz 的声音，它的引脚就直接输出低电平，听不到就输出高电平。堪称本项目超声波模块的完美逃课神器！讨论时抛出这个芯片绝对惊艳全场。**【预计预算: £2 - £4】**(超声波探头£2 + LM567芯片几十便士，性价比爆表且极度省心)。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the 40kHz ultrasound, Plan A is to copy our radio circuit and use an envelope detector. Cost: £3 to £5. Plan B is a brilliant shortcut: use an LM567 Tone Decoder IC. We just tune it to 40kHz, and it outputs a low digital signal when it hears the sound. It's cheap, reliable, and costs only £2 to £4. I highly recommend this."

### 6.5 小车软件多任务处理 (Software Multitasking)
- **难点**：小车要同时开着 WiFi 等遥控指令，同时要数红外脉冲，同时还要读串口串口。如果代码里到处都是 `delay()` 死等，车就会卡死不听使唤。
- **思路A: 状态机 (State Machine)**。写一个非阻塞的轮询系统，用 `millis()` 替代所有的 `delay()`。**【预计预算: £0】**(白嫖程序员的肝)。
- **思路B: 中断驱动 (Interrupt Driven)**。把一切能用硬件引脚触发的任务（比如算红外脉冲频率），全都挂在硬件中断上。主循环只负责听 WiFi 和打方向盘。**【预计预算: £0】**(同样是白嫖程序员的头发)。

> 🗣️ **English Script (Read aloud / Show to team):**
> "For the software multitasking, we cannot use 'delay()' functions, otherwise the rover will freeze. Plan A is to use a State Machine with the 'millis()' function. Plan B is an Interrupt-Driven approach, where sensor reading is handled by hardware interrupts. Both cost nothing but require good coding skills."

### 💡 预算控制终极建议 (Budget Pro-Tips)
1. **多用零散件，少用模块板**: 不要去买那种自带插针、直接插树莓派或Arduino的“红色/蓝色小模块”（Breakout board），那种贵好几倍。自己买芯片 (IC) 和电阻电容手焊，不仅省钱，老师给分（Implementation mark）还会更高！
2. **小心运费背刺**: 在 RS Components 或 Farnell 等大网站买东西，有时候几便士的元件要收十几镑运费。一定要凑单让组里的“采购负责人 (Approver)”一次性在学校系统的在线订单 (Online Order Form) 里买齐！

> 🗣️ **English Script (Read aloud / Show to team):**
> "Finally, a quick tip on the budget. First, let's buy individual IC chips and resistors to solder ourselves instead of expensive breakout boards. It saves money and gets us higher implementation marks. Second, let's group our orders through the official 'Approver' to avoid paying multiple heavy shipping fees from sites like RS Components or Farnell."

---

## 11. Adafruit 官方硬件手册核心排雷 (Official Hardware Manual Takeaways)
根据官方 270 页的 Metro M0 手册，我们确立了以下绝对不可违背的工程铁律：
1. **🚨 绝对禁止使用 Python 遥控 (No CircuitPython for WiFi)**: 手册明确指出，M0 芯片的内存极小，在 CircuitPython 环境下**无法装下 WINC1500 WiFi 库**。小车的主控程序**必须使用 Arduino IDE (C++)** 编写，否则无法实现网页遥控！
2. **引脚复核通过 (PWM Pins Verified)**: 我们的电机黄金引脚方案（2, 3, 4, 6）经过手册验证，**全部支持 PWM 调速**，方案 100% 安全可行。
3. **双串口区分 (Serial vs Serial1)**: `Serial` 对象专门用于 USB 连接电脑打印调试信息；主板边缘的 D0(RX) 和 D1(TX) 属于硬件串口，必须使用 `Serial1`。后续读取岩石的 UART 信号必须用 `Serial1.read()`。
4. **死机抢救指南 (Double-Tap Reset)**: 如果代码写错导致单片机遇难、USB 无法被电脑识别，**快速双击 RESET 按钮**即可强制进入 Bootloader 刷机模式进行抢救。

> 🗣️ **English Script (Read aloud / Show to team):**
> "I just reviewed the official Adafruit Metro M0 manual and found a critical warning: The M0 chip does not have enough flash memory to run the WINC1500 WiFi library in CircuitPython. This means we MUST write our main control code in C++ using the Arduino IDE. If we try to use Python, the WiFi will simply not work. Also, I verified our motor pins (2,3,4,6)—they fully support PWM. Lastly, when reading the rock's UART signal later, remember we must use 'Serial1' for the hardware pins, not 'Serial'."

---

## 12. 网页与小车的通信架构 (Web-to-Rover Communication Architecture)
在官方提供的 WiFi 遥控框架中，**没有任何部分使用 Python**。通信的核心逻辑如下：

1. **前端网页 (HTML + JavaScript)**: 网页并不是独立的文件，而是一段极长的纯 HTML 文本，被当作常量字符串直接硬编码（Hardcode）在小车的 C++ 程序里。
2. **手机加载网页**: 当手机浏览器访问小车的 IP 地址时，小车把这段 HTML 文本发送给手机渲染显示。
3. **发送控制暗号 (HTTP GET)**: 当用户在手机上点击网页的“前进”按钮时，网页内的 JavaScript 会向小车发送一个带有特定路径的 HTTP 请求（例如 `GET /FORWARD`）。
4. **后端执行 (C++ 控制硬件)**: 小车上的 C++ 代码通过 `server.on("/FORWARD", moveForward)` 监听到这个请求后，立刻触发底层的 `digitalWrite` 和 `analogWrite` 函数，使得电机转动。

**总结**: 负责网页的同学只需要写带有 AJAX 发送 GET 请求的 HTML/JS，负责硬件的同学只需要在 C++ 里绑定对应路径的执行函数，即可完美对接。

> 🗣️ **English Script (Read aloud / Show to team):**
> "Just to clarify our software architecture: We are NOT using Python for the web server. The rover acts as a standalone web server using pure C++. The webpage itself is just an HTML and JavaScript string embedded directly in our C++ code. When a button is clicked on the browser, the JavaScript sends an HTTP GET request (like '/FORWARD') to the rover, which then triggers our C++ motor functions."

---

## 13. 代码存档 (Code Archive)

### 13.1 纯硬件电机测试脚本 (Hardware-Only Motor Test Script)
*用途：不依赖 WiFi 和网页，烧录后小车会自动执行“前进->停止->后退->原地左转->原地右转”的循环，用于验证电机、H桥模块以及引脚接线是否绝对正确。*

*📝 **代码已独立保存**：为了方便在 Mac 和 Windows 之间跨设备传输和烧录，测试代码已被提取至独立的 Arduino 项目文件夹中。*
👉 **文件路径**: `my stuff/hardware_motor_test/hardware_motor_test.ino`