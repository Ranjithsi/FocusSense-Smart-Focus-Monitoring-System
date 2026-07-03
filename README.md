# FocusSense – An ESP32 IoT System for Focus, Posture, and Distraction Monitoring

An ESP32-based system that monitors whether a person is present, focused, distracted, or sitting at the wrong distance from their desk — with live feedback on an OLED display, buzzer alerts, and cloud logging to ThingSpeak.

## Overview

The system uses an ultrasonic sensor to detect presence and correct sitting distance, and an IR sensor to detect distractions (e.g. a phone placed near the desk). Based on these inputs, the ESP32 determines a focus state and reacts locally (OLED + buzzer) while also logging aggregated focus data to ThingSpeak for long-term tracking.

## Features

- Presence detection using an ultrasonic sensor (no PIR required)
- Distraction detection using an IR sensor
- Posture/distance monitoring (ideal sitting range check)
- Real-time status on a 128x64 OLED display
- Buzzer alerts with distinct patterns for distraction vs bad posture
- Automatic focus-percentage logging to ThingSpeak every 20 seconds
- Simple state machine: `AWAY`, `FOCUSED`, `DISTRACTED`, `BAD_POSTURE`

## Block diagram

```
Ultrasonic sensor ─┐
                    ├──▶ ESP32 controller ──▶ OLED display
IR sensor ──────────┘         │            ──▶ Buzzer
                               └──────────────▶ ThingSpeak (WiFi)
```

## Hardware required

| Component | Qty |
|---|---|
| ESP32 dev board | 1 |
| HC-SR04 ultrasonic sensor | 1 |
| IR obstacle/proximity sensor module | 1 |
| SSD1306 OLED display (128x64, I2C) | 1 |
| Active buzzer | 1 |
| Breadboard + jumper wires | - |

## Pin connections

### HC-SR04 Ultrasonic Sensor
| HC-SR04 Pin | ESP32 Pin |
|---|---|
| VCC | 5V (Vin) |
| TRIG | GPIO 5 |
| ECHO | GPIO 18 |
| GND | GND |

### IR Sensor
| IR Module Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| OUT | GPIO 26 |

### OLED Display (I2C)
| OLED Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Buzzer
| Buzzer Pin | ESP32 Pin |
|---|---|
| + | GPIO 4 |
| – | GND |



## How it works

1. The ultrasonic sensor continuously measures distance. A valid reading within `PRESENCE_MAX_CM` means someone is at the desk; otherwise the system enters the `AWAY` state after a timeout.
2. If the IR sensor is triggered while someone is present, the state becomes `DISTRACTED`.
3. If the measured distance falls outside the ideal range (`IDEAL_MIN_CM` to `IDEAL_MAX_CM`), the state becomes `BAD_POSTURE`.
4. Otherwise, the state is `FOCUSED`.
5. The OLED shows the current state and a running focus percentage.
6. The buzzer beeps differently depending on state (distraction vs posture warning).
7. Every 20 seconds, the aggregated percentages for each state are uploaded to a ThingSpeak channel.

## Software setup

1. Install the Arduino IDE and add ESP32 board support.
2. Install these libraries via Library Manager:
   - `Adafruit SSD1306`
   - `Adafruit GFX`
   - `ThingSpeak` (by MathWorks)
3. Create a ThingSpeak channel with 5 fields:
   - Field 1: Focus %
   - Field 2: Distracted %
   - Field 3: Bad Posture %
   - Field 4: Away %
   - Field 5: Current state (0=Away, 1=Focused, 2=Distracted, 3=BadPosture)
4. Open `SmartFocusMonitor.ino`, fill in:
   - `WIFI_SSID`, `WIFI_PASSWORD`
   - `CHANNEL_ID`, `WRITE_API_KEY`
5. Upload to your ESP32 and open the Serial Monitor at 115200 baud to confirm uploads.

## Troubleshooting

If the OLED doesn't light up or the ultrasonic sensor seems unresponsive, run `diagnostic_test.ino` first:
- It scans the I2C bus and reports the OLED's address (should be `0x3C` or `0x3D`).
- It prints live ultrasonic distance readings so you can confirm the sensor responds to hand movement.

## Repository structure

```
├── SmartFocusMonitor.ino     # Main firmware
├── diagnostic_test.ino       # I2C + ultrasonic diagnostic sketch
└── README.md
```

## Future improvements

- Add a Blynk app layer for remote monitoring/control
- Log focus sessions with timestamps for daily/weekly reports
- Add a physical mute button for the buzzer
- Machine-learning-based focus scoring using accumulated ThingSpeak data

## License

This project is open for personal, academic, and educational use.
