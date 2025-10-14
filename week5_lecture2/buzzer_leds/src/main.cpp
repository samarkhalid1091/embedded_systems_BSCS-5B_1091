#include <Arduino.h>

#define BUZZER_PIN  27     // GPIO connected to buzzer
#define PWM_CH      0      // PWM channel (0–15)
#define FREQ        2000   // Default frequency (Hz)
#define RESOLUTION  10     // 10-bit resolution (0–1023)

#define LED1_PIN 18        // First LED pin
#define LED2_PIN 19        // Second LED pin
#define LED_CH1  1         // PWM channel for LED1
#define LED_CH2  2         // PWM channel for LED2
#define LED_FREQ 5000      // LED PWM frequency
#define LED_RES  8         // LED PWM resolution

void setup() {
  // Setup PWM channel for buzzer
  ledcSetup(PWM_CH, FREQ, RESOLUTION);
  ledcAttachPin(BUZZER_PIN, PWM_CH);

  // Setup PWM channels for LEDs
  ledcSetup(LED_CH1, LED_FREQ, LED_RES);
  ledcAttachPin(LED1_PIN, LED_CH1);
  ledcSetup(LED_CH2, LED_FREQ, LED_RES);
  ledcAttachPin(LED2_PIN, LED_CH2);

  // --- 1. Simple beep pattern ---
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(PWM_CH, 2000 + i * 400); // change tone
    delay(150);
    ledcWrite(PWM_CH, 0);                  // stop tone
    delay(150);
  }

  // --- 2. Frequency sweep (400Hz → 3kHz) ---
  for (int f = 400; f <= 3000; f += 100) {
    ledcWriteTone(PWM_CH, f);
    delay(20);
  }
  ledcWrite(PWM_CH, 0);
  delay(500);

  // --- 3. Melody ---
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    ledcWriteTone(PWM_CH, melody[i]);
    ledcWrite(PWM_CH, 800);
    delay(250);
  }
}

void loop() {
  // --- LED brightness fade for two LEDs ---
  for (int d = 0; d <= 255; d++) { 
    ledcWrite(LED_CH1, d);
    ledcWrite(LED_CH2, 255 - d); // opposite fade
    delay(2); 
  }
  for (int d = 255; d >= 0; d--) { 
    ledcWrite(LED_CH1, d);
    ledcWrite(LED_CH2, 255 - d);
    delay(2); 
  }
}