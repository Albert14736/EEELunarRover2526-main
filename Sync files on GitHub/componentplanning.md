# Component Planning & Discussion Board

**🚨 DEADLINE:** This Friday (We need to submit the Online Order Form to EEE Stores!)
**💰 TOTAL BUDGET:** £60.00

Hi team, to make sure we are ready to order our parts by Friday, please drop your research, component ideas, and proposed solutions here. We can discuss and finalize them during our next meeting.

---

## 1. Ultrasonic Detection (40kHz Presence)
*Objective: Detect the presence or absence of a 40kHz sound signal.*

- **Albert Ma:** I highly recommend using an **LM567 Tone Decoder IC** as a brilliant shortcut. Instead of building a complex envelope detector circuit, we just tune the LM567 to 40kHz with a few resistors/capacitors. It simply outputs a low digital signal when it hears the 40kHz sound. It's cheap, extremely reliable, and costs only about £1 to £3.
- **Wangmo Koo (Lead):** [Please add your thoughts/research here]
- **Christopher Koh:** 
- **Devesh Kemani:** 
- **Mohammed Salem:** 
- **Ye Zifan:** 

---

## 2. Infrared Detection (Radiation Pulse Rate)
*Objective: Count the 50μs IR pulses (312Hz or 547Hz) and filter out the 100Hz room light noise.*

- **Albert Ma:** To filter out the 100Hz room light noise, we should try the **Software Subtraction** method first. We just read all the pulses using hardware interrupts and mathematically subtract the 100Hz baseline noise in the C++ software. This costs £0. If that fails, we can add a physical optical filter (e.g., a piece of exposed black film) over the sensor for almost £0, or build an active band-pass filter around 400Hz using op-amps (£1 - £3).
- **Christopher Koh (Lead):** [Please add your thoughts/research here]
- **Wangmo Koo:** 
- **Devesh Kemani:** 
- **Mohammed Salem:** 
- **Ye Zifan:** 

---

## 3. Radio Detection (89kHz Age Decoding)
*Objective: Receive 89kHz OOK signal and decode the UART string (e.g., `#123`).*

- **Albert Ma:** We have a few options here. 
  - *Plan A (Classic):* Use an air-cored inductor antenna, amplify with op-amps, an envelope detector, and a comparator (£2 - £5). Safe but needs lots of tuning.
  - *Plan B (Digital DSP):* Amplify the signal and use the microcontroller's ADC to decode it in software (£1 - £2).
  - *Plan C (Shortcut):* Find a dedicated AM radio decoder IC that supports 89kHz to save time (£3 - £8). 
  What do you guys think?
- **Ye Zifan (Lead):** [Please add your thoughts/research here]
- **Christopher Koh:** 
- **Wangmo Koo:** 
- **Devesh Kemani:** 
- **Mohammed Salem:** 

---

## 4. Magnetic Detection (Up/Down Polarity)
*Objective: Determine if the static magnetic field is pointing UP or DOWN.*

- **Albert Ma:** We should buy an **Analog Hall Effect Sensor (like SS39E or SS49E)**. It costs < £1. We can just read the voltage via ADC (e.g., ~1.65V at baseline, approaches 3.3V for North, 0V for South). 
  *⚠️ Warning: We should absolutely avoid using a simple coil (Electromagnetic Induction). Because the magnetic field is static, a coil will only work if the rover is moving constantly, which is highly unreliable for detecting Up/Down polarity.*
- **Devesh Kemani (Lead):** [Please add your thoughts/research here]
- **Christopher Koh:** 
- **Wangmo Koo:** 
- **Mohammed Salem:** 
- **Ye Zifan:** 

---
*Note: If you have specific part numbers or RS Components / Farnell links, please paste them next to your name!*