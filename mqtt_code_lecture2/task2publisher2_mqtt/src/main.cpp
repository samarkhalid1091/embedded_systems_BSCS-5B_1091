#include <Arduino.h>

/******************
 * MQTT Publisher 2 (Simulated Data)
 ******************/
#include <WiFi.h>
#include <PubSubClient.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const char* mqtt_server = "10.13.21.170";
const int mqtt_port = 1883;

const char* TOPIC_TEMP = "home/lab2/temp";
const char* TOPIC_HUM  = "home/lab2/hum";

WiFiClient espClient;
PubSubClient mqtt(espClient);

void connectWiFi() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void connectMQTT() {
  while (!mqtt.connected()) {
    mqtt.connect("ESP32_Publisher_2"); // UNIQUE ID
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqtt.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  float temp = random(250, 350) / 10.0; // 25.0–35.0
  float hum  = random(400, 700) / 10.0; // 40–70%

  char tBuf[8], hBuf[8];
  dtostrf(temp, 4, 2, tBuf);
  dtostrf(hum, 4, 2, hBuf);

  mqtt.publish(TOPIC_TEMP, tBuf);
  mqtt.publish(TOPIC_HUM, hBuf);

  Serial.println("Publisher 2 sent simulated data");
  delay(6000);
}