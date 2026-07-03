/*
  ===========================================================
   SMART FOCUS MONITORING SYSTEM - ESP32
  ===========================================================
  Sensors:
    - Ultrasonic (HC-SR04) -> Measures distance. Used BOTH to detect
                               presence (is someone at the desk at all)
                               AND to check correct sitting distance/posture
    - IR sensor       -> Detects distraction object (e.g. phone
                          placed in front of / picked up near sensor)
  Outputs:
    - OLED (SSD1306, I2C) -> Shows live focus status
    - Buzzer              -> Audible alert for distraction / bad posture
    - ThingSpeak          -> Cloud logging every UPDATE_INTERVAL sec

  Libraries needed (Arduino IDE Library Manager):
    - Adafruit SSD1306
    - Adafruit GFX
    - ThingSpeak (by MathWorks)
  ===========================================================
*/

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ThingSpeak.h"

// ---------------- WiFi & ThingSpeak Credentials ----------------
const char* WIFI_SSID     = "Indumathi4G";
const char* WIFI_PASSWORD = "indusiva@27";

unsigned long CHANNEL_ID        = 3421396;         // Your ThingSpeak Channel ID
const char*   WRITE_API_KEY     = "K8A7H5WFPEFTKLCD";

WiFiClient client;

// ---------------- Pin Definitions ----------------
#define IR_PIN         26   // Digital input
#define TRIG_PIN       5
#define ECHO_PIN       18
#define BUZZER_PIN     4

// ---------------- OLED Setup ----------------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Tunable Thresholds ----------------
const float IDEAL_MIN_CM   = 30.0;   // Ideal sitting distance range
const float IDEAL_MAX_CM   = 60.0;
const float PRESENCE_MAX_CM      = 80.0;   // Beyond this = nobody at desk
const unsigned long AWAY_TIMEOUT_MS      = 15000;  // No valid presence -> "Away"
const unsigned long SENSOR_SAMPLE_MS     = 500;    // How often to sample sensors
const unsigned long UPDATE_INTERVAL_MS   = 20000;  // ThingSpeak free tier needs >=15s

// ---------------- State Machine ----------------
enum FocusState { AWAY, FOCUSED, DISTRACTED, BAD_POSTURE };
FocusState currentState = AWAY;

unsigned long lastPresenceTime  = 0;
unsigned long lastSampleTime   = 0;
unsigned long lastUploadTime   = 0;

// Counters used to compute a "focus score" over each upload window
unsigned long focusedCount    = 0;
unsigned long distractedCount = 0;
unsigned long badPostureCount = 0;
unsigned long awayCount       = 0;
unsigned long totalSamples    = 0;

// ---------------- Function Prototypes ----------------
float readDistanceCM();
void updateFocusState();
void updateOLED();
void handleBuzzer();
void uploadToThingSpeak();
void beep(int durationMs, int times);

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Smart Focus Monitor");
  display.println("Connecting WiFi...");
  display.display();

  // WiFi connect
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  ThingSpeak.begin(client);

  lastPresenceTime = millis();
}

void loop() {
  unsigned long now = millis();

  // --- Sample sensors every SENSOR_SAMPLE_MS ---
  if (now - lastSampleTime >= SENSOR_SAMPLE_MS) {
    lastSampleTime = now;
    updateFocusState();
    updateOLED();
    handleBuzzer();
  }

  // --- Upload aggregated data to ThingSpeak every UPDATE_INTERVAL_MS ---
  if (now - lastUploadTime >= UPDATE_INTERVAL_MS) {
    lastUploadTime = now;
    uploadToThingSpeak();
  }
}

// ---------------------------------------------------------
// Reads distance from HC-SR04 ultrasonic sensor in cm
// ---------------------------------------------------------
float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout (~5m range)
  if (duration == 0) return -1;                   // no echo received

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

// ---------------------------------------------------------
// Core sensor fusion logic -> decides the current FocusState
// ---------------------------------------------------------
void updateFocusState() {
  bool distractionFlag  = digitalRead(IR_PIN);    // HIGH = IR beam broken (e.g. phone picked up)
  float distance         = readDistanceCM();

  // Presence is inferred from the ultrasonic sensor: a valid echo within
  // PRESENCE_MAX_CM means someone is sitting at the desk.
  bool presenceDetected = (distance > 0 && distance <= PRESENCE_MAX_CM);

  if (presenceDetected) {
    lastPresenceTime = millis();
  }

  bool isAway = (millis() - lastPresenceTime) > AWAY_TIMEOUT_MS;

  if (isAway) {
    currentState = AWAY;
  }
  else if (distractionFlag) {
    currentState = DISTRACTED;
  }
  else if (distance > 0 && (distance < IDEAL_MIN_CM || distance > IDEAL_MAX_CM)) {
    currentState = BAD_POSTURE;
  }
  else {
    currentState = FOCUSED;
  }

  // Tally for focus-score calculation
  totalSamples++;
  switch (currentState) {
    case FOCUSED:     focusedCount++;     break;
    case DISTRACTED:  distractedCount++;  break;
    case BAD_POSTURE: badPostureCount++;  break;
    case AWAY:        awayCount++;        break;
  }
}

// ---------------------------------------------------------
// Displays current status on OLED
// ---------------------------------------------------------
void updateOLED() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Smart Focus Monitor");
  display.println("--------------------");
  display.setTextSize(2);
  display.setCursor(0, 20);

  switch (currentState) {
    case FOCUSED:
      display.println("FOCUSED");
      break;
    case DISTRACTED:
      display.println("DISTRACTED");
      break;
    case BAD_POSTURE:
      display.println("POSTURE!");
      break;
    case AWAY:
      display.println("AWAY");
      break;
  }

  display.setTextSize(1);
  display.setCursor(0, 50);
  if (totalSamples > 0) {
    int focusPercent = (focusedCount * 100) / totalSamples;
    display.print("Focus: ");
    display.print(focusPercent);
    display.println("%");
  }
  display.display();
}

// ---------------------------------------------------------
// Buzzer alert logic - different beep patterns per state
// ---------------------------------------------------------
void handleBuzzer() {
  switch (currentState) {
    case DISTRACTED:
      beep(150, 2);   // two short beeps = distraction warning
      break;
    case BAD_POSTURE:
      beep(400, 1);   // one long beep = posture warning
      break;
    case FOCUSED:
    case AWAY:
    default:
      digitalWrite(BUZZER_PIN, LOW); // silent
      break;
  }
}

void beep(int durationMs, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(durationMs);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < times - 1) delay(100);
  }
}

// ---------------------------------------------------------
// Sends aggregated focus data to ThingSpeak
// Field mapping (create these fields in your ThingSpeak channel):
//   Field 1 -> Focus %        Field 2 -> Distracted %
//   Field 3 -> Bad Posture %  Field 4 -> Away %
//   Field 5 -> Current State (0=Away,1=Focused,2=Distracted,3=BadPosture)
// ---------------------------------------------------------
void uploadToThingSpeak() {
  if (totalSamples == 0) return;

  int focusPct     = (focusedCount * 100) / totalSamples;
  int distractPct  = (distractedCount * 100) / totalSamples;
  int posturePct   = (badPostureCount * 100) / totalSamples;
  int awayPct      = (awayCount * 100) / totalSamples;

  ThingSpeak.setField(1, focusPct);
  ThingSpeak.setField(2, distractPct);
  ThingSpeak.setField(3, posturePct);
  ThingSpeak.setField(4, awayPct);
  ThingSpeak.setField(5, (int)currentState);

  int result = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
  if (result == 200) {
    Serial.println("ThingSpeak update successful");
  } else {
    Serial.println("ThingSpeak update failed, code: " + String(result));
  }

  // Reset counters for the next window
  focusedCount = distractedCount = badPostureCount = awayCount = totalSamples = 0;
}
