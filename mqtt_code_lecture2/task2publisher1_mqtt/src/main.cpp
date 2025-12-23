#include <Arduino.h>

/******************
 * MQTT Publisher 1 (DHT22)
 ******************/
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const char* mqtt_server = "10.13.21.170";
const int mqtt_port = 1883;

#define DHTPIN 23
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

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
    mqtt.connect("ESP32_Publisher_1"); // UNIQUE ID
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWiFi();
  mqtt.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) return;

  char tBuf[8], hBuf[8];
  dtostrf(t, 4, 2, tBuf);
  dtostrf(h, 4, 2, hBuf);

  mqtt.publish(TOPIC_TEMP, tBuf);
  mqtt.publish(TOPIC_HUM, hBuf);

  Serial.println("Publisher 1 sent DHT data");
  delay(5000);
}