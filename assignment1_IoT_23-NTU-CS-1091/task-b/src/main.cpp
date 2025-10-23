//Assignmnet1 Task-b
//Long Press for buzzer and short press for led (ON)
//Embedded IOT system Fall-2025
//Samar Khalid          //23-NTU-CS-1091
#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Pin Definitions
#define BUTTON_PIN 25      // Push button input
#define LED_PIN 18         // LED output
#define BUZZER_PIN 27      // Buzzer output
#define SDA_PIN 21         // I2C SDA
#define SCL_PIN 22         // I2C SCL

//OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Global variables
bool ledState = false;
bool buttonPressed = false;
unsigned long pressStartTime = 0;
const unsigned long longPressDuration = 1500; // 1.5 seconds

//Display message
void showMessage(const String &msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print(msg);
  display.display();
}

//Setup
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  //Initialize OLED
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  display.clearDisplay();
  display.display();

  showMessage("System Ready");
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

//Loop
void loop() {
  bool btnState = digitalRead(BUTTON_PIN);

  //Button pressed (active LOW)
  if (!btnState && !buttonPressed) {
    buttonPressed = true;
    pressStartTime = millis();
  }

  //Button held down
  if (buttonPressed && !btnState) {
    unsigned long pressDuration = millis() - pressStartTime;
    if (pressDuration > longPressDuration) {
      // Long press detected:play buzzer continuously
      showMessage("Long Press");
     
      tone(BUZZER_PIN, 2000); //2kHz tone(continuous)

    }
  }

  //Button released
  if (buttonPressed && btnState) {
    unsigned long pressDuration = millis() - pressStartTime;
    buttonPressed = false;

    //Stop buzzer
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);

    if (pressDuration <= longPressDuration) {
      // Short press: toggle LED
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      showMessage("Short Press");
    }
  }
}