//Week3 Assignment_1 Task_a
//Use one button to cycle through LED modes ans use second button to reset to OFF
//Embedded IoT System Fall-2025
//Samar Khalid                      Reg no:23-NTU-CS-1091

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED Setup 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Define Pin numbers
#define LED1 14
#define LED2 16
#define LED3 17
#define BTN1 25   //Mode button
#define BTN2 26   //Reset button

//PWM Setup 
#define PWM_CH1 0
#define PWM_CH2 1
#define PWM_CH3 2
#define FREQ 5000
#define RES 8

//Timer
hw_timer_t *My_timer = nullptr;
volatile bool toggleState = false;

//Global variable
int mode = 0; // 0=OFF, 1=Alternate Blink, 2=All ON, 3=PWM Fade
bool lastBtn1 = HIGH, lastBtn2 = HIGH;
unsigned long lastDebounce = 0;
const int debounceDelay = 200;

//Display Helper
void showMode() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  switch (mode) {
    case 0: display.print("All OFF"); break;
    case 1: display.print("Alternate Blink"); break;
    case 2: display.print("All ON"); break;
    case 3: display.print("PWM Fade"); break;
  }
  display.display();
}

//Timer ISR
void IRAM_ATTR onTimer() {
  if (mode != 1) return;  

  toggleState = !toggleState;

  ledcWrite(PWM_CH1, toggleState ? 255 : 0);
  ledcWrite(PWM_CH2, toggleState ? 0   : 255);
  ledcWrite(PWM_CH3, toggleState ? 255 : 0);
}

//Setup
void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);

  //OLED setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  //PWM setup
  ledcSetup(PWM_CH1, FREQ, RES);
  ledcSetup(PWM_CH2, FREQ, RES);
  ledcSetup(PWM_CH3, FREQ, RES);
  ledcAttachPin(LED1, PWM_CH1);
  ledcAttachPin(LED2, PWM_CH2);
  ledcAttachPin(LED3, PWM_CH3);

  //Timer setup for blinking
  My_timer = timerBegin(0, 80, true);               //1 tick = 1 µs (80 MHz / 80)
  timerAttachInterrupt(My_timer, &onTimer, true);  
  timerAlarmWrite(My_timer, 500000, true);          //Toggle every 0.5 sec
  timerAlarmEnable(My_timer);                      

  //Initial state
  ledcWrite(PWM_CH1, 0);
  ledcWrite(PWM_CH2, 0);
  ledcWrite(PWM_CH3, 0);
  showMode();
}

//Loop
void loop() {
  bool btn1 = digitalRead(BTN1);
  bool btn2 = digitalRead(BTN2);

  //Debounce logic
  if (millis() - lastDebounce > debounceDelay) {
    if (btn1 == LOW && lastBtn1 == HIGH) {
      mode = (mode + 1) % 4;
      showMode();
      lastDebounce = millis();
    }
    if (btn2 == LOW && lastBtn2 == HIGH) {
      mode = 0;
      showMode();
      lastDebounce = millis();
    }
  }

  lastBtn1 = btn1;
  lastBtn2 = btn2;

  //Mode Logic
  switch (mode) {
    case 0: // All OFF
      ledcWrite(PWM_CH1, 0);
      ledcWrite(PWM_CH2, 0);
      ledcWrite(PWM_CH3, 0);
      break;

    case 1: //Alternate Blink (handled by timer)
      //nothing in loop
      break;

    case 2: // All ON
      ledcWrite(PWM_CH1, 255);
      ledcWrite(PWM_CH2, 255);
      ledcWrite(PWM_CH3, 255);
      break;

    case 3: //PWM Fade (all together)
      for (int d = 0; d <= 255 && mode == 3; d++) {
        ledcWrite(PWM_CH1, d);
        ledcWrite(PWM_CH2, d);
        ledcWrite(PWM_CH3, d);
        delay(10);
        if (digitalRead(BTN1) == LOW || digitalRead(BTN2) == LOW) return;
      }
      for (int d = 255; d >= 0 && mode == 3; d--) {
        ledcWrite(PWM_CH1, d);
        ledcWrite(PWM_CH2, d);
        ledcWrite(PWM_CH3, d);
        delay(10);
        if (digitalRead(BTN1) == LOW || digitalRead(BTN2) == LOW) return;
      }
      break;
  }
}