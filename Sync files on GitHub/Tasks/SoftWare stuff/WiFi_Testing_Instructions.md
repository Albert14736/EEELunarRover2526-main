# WiFi Remote Control - Field Test Instructions

**Objective:** Test the full integration of the C++ backend and the Web UI frontend, ensuring the rover can be driven via a mobile phone over a local network.

**Files in this folder:**
- `wifi_rover_control.ino`: The final C++ source code to be uploaded to the rover.
- `remote_ui_test.html`: A local HTML file to preview the UI on any device without connecting to the rover.
- `WiFi_Testing_Instructions.md`: This instruction file.

---

## Phase 0: Hardware Wiring (CRITICAL!)
Because the WiFi Shield actively uses pins **5, 7, 10, 11, 12, and 13**, we MUST NOT plug any motor wires into them, or the WiFi will crash instantly.

Please connect the motor driver (H-Bridge) to the Metro M0 board exactly as follows:
- **Left Motor:**
  - `DIR` (Blue wire)  -> **Pin 2**
  - `EN`  (Orange wire)-> **Pin 3**
- **Right Motor:**
  - `DIR` (Blue wire)  -> **Pin 4**
  - `EN`  (Orange wire)-> **Pin 6**

⚠️ **COMMON GROUND WARNING:** Ensure that the GND pin on the motor driver board is physically connected to the GND pin on the Metro M0 board. If they don't share the same ground, the motors will not spin!

## Phase 1: Uploading the Code (The Brain)
1. Connect the Adafruit Metro M0 board (with the WiFi shield attached) to the lab computer via USB.
2. Open the `wifi_rover_control.ino` file in VS Code (PlatformIO) or Arduino IDE.
3. Verify the WiFi credentials in the code:
   - `ssid`: `"EEERover"`
   - `pass`: `"exhibition"`
   - `groupNumber`: `10` (Since you are Group 10, this sets your static IP to `192.168.0.11`).
4. **Upload/Flash** the code to the Metro M0 board.
5. Open the **Serial Monitor** (Baud rate: 9600). You should see:
   - `Connecting to SSID: EEERover`
   - `Connected!`
   - `IP Address: 192.168.0.11` (Take note of this IP).

## Phase 2: Network Connection & Power Up
1. Turn on the 6V battery pack on the rover to provide power to the H-Bridge motor drivers.
2. Unplug the USB cable if you want to test fully wireless (the Metro board will now draw power from the battery).
3. Take out your mobile phone and connect to the lab's WiFi network: **`EEERover`** (Password: **`exhibition`**).

## Phase 3: Opening the Web UI
1. Open a web browser (Chrome/Safari) on your phone.
2. Type the IP address exactly as shown in the Serial Monitor (e.g., `http://192.168.0.11`) into the address bar and press Enter.
3. The "Lunar Rover PRO" control panel with the directional arrows (▲ ◀ ▶ ▼) should load on your screen.

### Alternative: Previewing the UI Remotely
You can preview the UI anytime, anywhere, using the public link below. This is great for checking the look and feel, but it won't actually control the rover.

**Public URL:** `https://Albert14736.github.io/EEELunarRover2526-main/Sync%20files%20on%20GitHub/Tasks/SoftWare%20stuff/remote_ui_test.html`

## Phase 4: Execution & Observation Checklist

- [ ] **Forward Test:** Press and *hold* the **▲ (FWD)** button. 
  - *Expected:* Both wheels spin forward.
  - *Troubleshooting:* If the rover moves backward, the motor wires are inverted. We can easily fix this by changing `SPEED` to `-SPEED` in the `moveForward()` C++ function.
- [ ] **Stop Logic:** Release the **▲** button. 
  - *Expected:* The rover stops immediately (Hold-to-drive logic works).
- [ ] **Spin Left:** Press and hold **◀ (LFT)**. 
  - *Expected:* Left wheel spins backward, right wheel spins forward. The rover spins on the spot.
- [ ] **Spin Right:** Press and hold **▶ (RGT)**. 
  - *Expected:* Left wheel spins forward, right wheel spins backward.

## Phase 5: The "Watchdog" Safety Test (Crucial)
We have implemented a 500ms timeout watchdog to prevent the rover from crashing if it loses the WiFi connection.
1. Press and hold the **▲ (FWD)** button so the rover keeps moving.
2. While still holding the button on the screen, use your other hand to **turn off your phone's WiFi** (or close the browser).
3. *Expected:* The rover MUST stop automatically within 0.5 seconds. It should NOT keep driving indefinitely.

---
*If all tests pass, the Movement & UI module is 100% complete!*