# Computer Vision-Based Home Automation System

Overview
  This project is a comprehensive **home automation system** that leverages computer vision, microcontroller-based controls, and sensor technologies to enhance home security, comfort, and efficiency. The system combines **Arduino** and **Python** programming to manage and automate various home features.

---

## Features
1. **Face Recognition:**  
   - Identifies and authenticates users using a camera.  
   - Python scripts handle face detection, encoding, and recognition.

2. **RFID Card Scanner:**  
   - Provides secure access using RFID cards.  
   - Arduino-controlled RFID module scans and validates cards.

3. **Keypad Password System:**  
   - Allows access via a 4x4 keypad module.  
   - Ensures multi-layered security.

4. **Water Level Measurement and Automation:**  
   - Monitors and controls water levels in a tank using ultrasonic sensors.  
   - Automates water pump operation based on water levels.

5. **Temperature Control System:**  
   - Measures room temperature using sensors (e.g., DHT11/DHT22).  
   - Controls fans or HVAC systems based on predefined thresholds.

6. **Fire Detection and Alarm System:**  
   - Uses flame sensors to detect fire.  
   - Triggers alarms and safety protocols upon detection.

7. **Motion Detection and Automation:**  
   - Detects motion using PIR sensors.  
   - Automates lights or other systems based on movement.

8. **Display System:**  
   - Displays real-time system status on a 20x4 LCD module.  
   - Updates include water levels, temperature, access logs, and system alerts.

---

## Components Used
### Hardware:
- **Arduino Mega** for sensor and actuator control.  
- **ESP32** (Optional) for IoT-based remote control.  
- **RFID Card Reader** for access authentication.  
- **4x4 Keypad Module** for password entry.  
- **PIR Sensors** for motion detection.  
- **Ultrasonic Sensor** for water level measurement.  
- **DHT11/DHT22 Sensor** for temperature monitoring.  
- **Flame Sensors** for fire detection.  
- **20x4 LCD Module** for system display.  

### Software:
- Python 3.x  
- Arduino IDE  
- Libraries:  
  - OpenCV (Python)  
  - SimpleFacerec (Python)  
  - Various Arduino libraries (Keypad, RFID, LiquidCrystal_I2C, etc.)  

---

## How It Works
1. **Face Recognition:**  
   - Users are authenticated through face recognition powered by Python's OpenCV library.  
   - Pre-generated encodings are used for accurate recognition.  

2. **Access Control:**  
   - An RFID card scanner and keypad system provide alternative authentication options.  

3. **Automation Systems:**  
   - Water pumps are activated/deactivated based on water levels.  
   - Fans and HVAC systems are controlled according to room temperature.  
   - Lights are triggered by motion detection.  
   - Fire alarms are activated upon detecting flames.  

4. **Display:**  
   - All system statuses are displayed in real-time on a 20x4 LCD module.  

---

## Directory Structure
