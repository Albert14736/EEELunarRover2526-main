# Magnetism Detection - Research & Design

## Hardware Configuration
- **Sensor**: SS49E Hall Effect Sensor
- **Amplification**: MCP6002 Op-amp (Non-inverting amplifier) to be built on a breadboard if the signal is too weak.
- **Wiring**:
    - Pin 1 (VCC): 3.3V on Metro board.
    - Pin 2 (GND): GND.
    - Pin 3 (Output): Analog Pin (e.g., A0).

## Logic & Calibration
- **Neutral Point**: ~1.65V (Analog value ~512).
- **Polarity**:
    - **North (UP)**: Voltage rises toward 3.3V (Value -> 1023).
    - **South (DOWN)**: Voltage drops toward 0V (Value -> 0).
- **Calibration**: The setup should account for magnetism from motors and zero it out during `setup()`.
- **Filtering**: Use a **Rolling Average** (e.g., average of the last 4 snapshots using an array) to prevent noise from small fluctuations.
- **Interference**: Need to account for potential magnetic fields from other nearby rocks.

## Positioning
- **Orientation**: Point the face (side with writing) down and point forward (closest to the rock).
- **Location**: Position it furthest away from the motors to minimize interference.
- **Mounting**: Plan to 3D print a mount that connects to the pins.
