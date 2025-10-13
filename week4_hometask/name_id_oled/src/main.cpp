
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED setup ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;); // If OLED init fails, halt here
  }

  display.clearDisplay();
}

void loop() {
  display.clearDisplay();

  // --- Set text properties ---
  display.setTextSize(1);            // Small text for name
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);         // Position for name
  display.println("Samar Khalid");     

  display.setTextSize(1);            // Small text for ID
  display.setCursor(10, 25);         // Position for ID below name
  display.println("23-NTU-CS-1091");     

  // --- Draw a rectangle around the text ---
  // Parameters: x, y, width, height, color
  display.drawRect(5, 5, 118, 35, SSD1306_WHITE); 

  display.display();                  // Push to OLED
  delay(5000);                        // Wait 5 seconds
}