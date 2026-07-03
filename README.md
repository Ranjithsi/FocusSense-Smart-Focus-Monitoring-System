# FocusSense – IoT-Based Smart Focus Monitoring System Using ESP32

A lightweight and intelligent focus monitoring system that helps users improve concentration during study or work sessions. Using an ESP32 with ultrasonic and IR sensors, the system detects user presence, sitting posture, and distractions, provides real-time feedback through an OLED display and buzzer, and uploads focus statistics to the ThingSpeak cloud for remote monitoring.

---

## 📖 What this project does

- Monitors whether the user is present at the desk using an ultrasonic sensor.
- Detects distractions using an IR sensor.
- Checks whether the user is sitting within an ideal distance.
- Displays the current focus status on an OLED display.
- Activates a buzzer whenever distraction or poor posture is detected.
- Calculates focus performance over time.
- Uploads focus statistics to ThingSpeak using Wi-Fi.

---

## ✨ Features

- 📡 ESP32-based IoT system
- 👤 Presence detection
- 📏 Sitting posture monitoring
- 📱 Distraction detection
- 📺 OLED status display
- 🔔 Real-time buzzer alerts
- ☁️ ThingSpeak cloud integration
- 📊 Focus percentage calculation
- ⚡ Low-cost and easy-to-build hardware

---

## 🛠 Hardware Used

| Component | Quantity |
|----------|----------|
| ESP32 Development Board | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| IR Obstacle Sensor | 1 |
| OLED Display (SSD1306) | 1 |
| Active Buzzer | 1 |
| Breadboard | 1 |
| Jumper Wires | As Required |

---

## 💻 Software Used

- Arduino IDE
- ESP32 Board Package
- ThingSpeak
- Adafruit SSD1306 Library
- Adafruit GFX Library
- Adafruit BusIO Library

---

## ⚙️ System Workflow

```
User
   │
   ▼
Ultrasonic Sensor
(Presence & Distance)
   │
   ▼
IR Sensor
(Distraction Detection)
   │
   ▼
ESP32 Controller
   │
   ├────────► OLED Display
   │
   ├────────► Buzzer Alert
   │
   └────────► Wi-Fi
                 │
                 ▼
           ThingSpeak Cloud
                 │
                 ▼
        Focus Statistics Dashboard
```

---

## 📌 Focus States

| State | Description |
|--------|-------------|
| Focused | User is attentive and sitting correctly |
| Distracted | Distraction detected |
| Bad Posture | Sitting too close or too far |
| Away | User is absent from the desk |

---

## 📡 ThingSpeak Data

| Field | Data |
|--------|------|
| Field 1 | Focus Percentage |
| Field 2 | Distraction Percentage |
| Field 3 | Bad Posture Percentage |
| Field 4 | Away Percentage |
| Field 5 | Current Focus State |

---

## 📂 Project Structure

```
FocusSense/
│
├── SmartFocus.ino
├── README.md
├── Images/
│   ├── BlockDiagram.png
│   ├── CircuitDiagram.png
│   ├── Hardware.jpg
│   └── ThingSpeakDashboard.png
└── Report/
    └── Project_Report.pdf
```

---

## 🔌 ESP32 Pin Configuration

| ESP32 Pin | Connected Device |
|-----------|------------------|
| GPIO 5 | Ultrasonic Trigger |
| GPIO 18 | Ultrasonic Echo |
| GPIO 26 | IR Sensor |
| GPIO 4 | Buzzer |
| GPIO 21 | OLED SDA |
| GPIO 22 | OLED SCL |

---

## 🚀 Future Enhancements

- AI-based face attention detection
- Eye blink monitoring
- Mobile application
- Firebase integration
- Email/SMS alerts
- Attendance tracking
- Productivity reports
- Machine learning-based focus prediction

---

## 🎯 Applications

- Student study monitoring
- Online learning
- Library monitoring
- Office productivity
- Workplace wellness
- Smart classrooms
- Personal productivity tracking

---

## 👨‍💻 Developed By

**Ranjith**

Bachelor of Engineering (Electronics and Communication Engineering)

---

## 📜 License

This project is released under the MIT License.

---

⭐ If you found this project useful, consider giving it a **Star** on GitHub!
