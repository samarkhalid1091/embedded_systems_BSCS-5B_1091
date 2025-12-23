#include <Arduino.h>

/******************
 * MQTT Subscriber 1 (OLED)
 ******************/
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const char* mqtt_server = "10.13.21.170";
const int mqtt_port = 1883;
const char* TOPIC_TEMP = "home/lab2/temp";

Adafruit_SSD1306 display(128, 64, &Wire, -1);

WiFiClient espClient;
PubSubClient mqtt(espClient);

String temp = "--";

void showTemp() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Subscriber 1");
  display.println("Temperature:");
  display.setTextSize(2);
  display.println(temp + " C");
  display.display();
}

void callback(char* topic, byte* payload, unsigned int length) {
  temp = "";
  for (int i = 0; i < length; i++) temp += (char)payload[i];
  showTemp();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21,22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showTemp();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  mqtt.connect("ESP32_Subscriber_1"); // UNIQUE ID
  mqtt.subscribe(TOPIC_TEMP);
}

void loop() {
  mqtt.loop();
}