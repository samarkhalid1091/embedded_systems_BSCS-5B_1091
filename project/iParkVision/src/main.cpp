/******************** BLYNK ********************/
#define BLYNK_TEMPLATE_ID "TMPL6K3TqVME_"
#define BLYNK_TEMPLATE_NAME "iParkVision"
#define BLYNK_AUTH_TOKEN "_pw9lDZoI42f5zvlMXKiPK1PqKYugXr6"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "NTU-FSD";
char pass[] = "";

/******************** LIBRARIES ********************/
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/******************** BLYNK V-PINS ********************/
#define VPIN_S1        V1
#define VPIN_S2        V2
#define VPIN_S3        V3
#define VPIN_FREE      V4
#define VPIN_BARRIER   V5
#define VPIN_VEHICLE   V6
#define VPIN_MANUAL    V7
#define VPIN_RFID_LOG  V8

/******************** RFID ********************/
#define SS_PIN   5
#define RST_PIN  27
MFRC522 rfid(SS_PIN, RST_PIN);

/******************** SERVO ********************/
#define SERVO_PIN 26
#define PWM_CHANNEL 0
#define PWM_FREQ 50
#define PWM_RESOLUTION 16
#define SERVO_CLOSED 2000
#define SERVO_OPEN   4500

/******************** ULTRASONIC ********************/
// Barrier
#define B_TRIG 4
#define B_ECHO 34

// Slots
#define S1_TRIG 13
#define S1_ECHO 35
#define S2_TRIG 17
#define S2_ECHO 36
#define S3_TRIG 16
#define S3_ECHO 33

/******************** DISPLAYS ********************/
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/******************** VARIABLES ********************/
bool barrierOpen = false;
unsigned long barrierTimer = 0;

/******************** FUNCTIONS ********************/
float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return -1;
  return (duration / 2.0) / 29.1;
}

bool isOccupied(float distance) {
  return (distance > 0 && distance <= 7);
}

/******************** BLYNK MANUAL GATE ********************/
BLYNK_WRITE(VPIN_MANUAL) {
  int value = param.asInt();
  if (value == 1 && !barrierOpen) {
    ledcWrite(PWM_CHANNEL, SERVO_OPEN);
    barrierOpen = true;
    barrierTimer = millis();
  }
}

/******************** SETUP ********************/
void setup() {
  Serial.begin(115200);

  // I2C
  Wire.begin(21, 22);

  // LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  // Ultrasonic
  pinMode(B_TRIG, OUTPUT);
  pinMode(B_ECHO, INPUT);
  pinMode(S1_TRIG, OUTPUT);
  pinMode(S1_ECHO, INPUT);
  pinMode(S2_TRIG, OUTPUT);
  pinMode(S2_ECHO, INPUT);
  pinMode(S3_TRIG, OUTPUT);
  pinMode(S3_ECHO, INPUT);

  // Servo PWM
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(SERVO_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, SERVO_CLOSED);

  // RFID
  SPI.begin(18, 19, 23, SS_PIN);
  rfid.PCD_Init();

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("iParkVision System Initialized");
}

/******************** LOOP ********************/
void loop() {
  Blynk.run();

  /* ---------- BARRIER LOGIC ---------- */
  float barrierDist = getDistance(B_TRIG, B_ECHO);
  bool vehicleDetected = (barrierDist > 0 && barrierDist <= 7);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  if (!barrierOpen) {
    if (vehicleDetected) {
      display.setCursor(5, 25);
      display.println("Scan Card");

      // RFID scan
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        String uidStr = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
          uidStr += String(rfid.uid.uidByte[i], HEX);
          uidStr += " ";
        }
        Serial.println(uidStr);

        Blynk.virtualWrite(VPIN_RFID_LOG, "Card: " + uidStr);

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();

        ledcWrite(PWM_CHANNEL, SERVO_OPEN);
        barrierOpen = true;
        barrierTimer = millis();
      }
    } else {
      display.setCursor(10, 25);
      display.println("iParkVision");
      ledcWrite(PWM_CHANNEL, SERVO_CLOSED);
    }
  } else {
    display.setCursor(30, 25);
    display.println("OPEN");

    if (millis() - barrierTimer >= 5000) {
      ledcWrite(PWM_CHANNEL, SERVO_CLOSED);
      barrierOpen = false;
    }
  }

  display.display();

  /* ---------- SLOT MONITORING ---------- */
  bool s1 = isOccupied(getDistance(S1_TRIG, S1_ECHO));
  bool s2 = isOccupied(getDistance(S2_TRIG, S2_ECHO));
  bool s3 = isOccupied(getDistance(S3_TRIG, S3_ECHO));

  int freeSlots = (!s1) + (!s2) + (!s3);

  // LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S1:");
  lcd.print(s1 ? "Occ " : "Emp ");
  lcd.print("S2:");
  lcd.print(s2 ? "Occ" : "Emp");
  lcd.setCursor(0, 1);
  lcd.print("S3:");
  lcd.print(s3 ? "Occ " : "Emp ");

  // Blynk Updates
  Blynk.virtualWrite(VPIN_S1, s1);
  Blynk.virtualWrite(VPIN_S2, s2);
  Blynk.virtualWrite(VPIN_S3, s3);
  Blynk.virtualWrite(VPIN_FREE, freeSlots);
  Blynk.virtualWrite(VPIN_BARRIER, barrierOpen);
  Blynk.virtualWrite(VPIN_VEHICLE, vehicleDetected);

  delay(300);
}