#include <Arduino.h>

/******************
 * ESP32 + DHT22 + MQTT (PUBLISHER)
 ******************/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ---------- WiFi ----------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ---------- MQTT ----------
const char* mqtt_server = "10.13.21.170";
const int mqtt_port = 1883;

const char* TOPIC_TEMP = "home/lab2/temp";
const char* TOPIC_HUM  = "home/lab2/hum";

// ---------- DHT ----------
#define DHTPIN  23
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---------- MQTT ----------
WiFiClient espClient;
PubSubClient mqtt(espClient);

// ---------- WiFi ----------
void connectWiFi() {
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// ---------- MQTT ----------
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting MQTT...");
    if (mqtt.connect("ESP32_Publisher_1")) {   // UNIQUE ID
      Serial.println("connected");
    } else {
      Serial.print("failed rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
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

  if (isnan(t) || isnan(h)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  char tBuf[8], hBuf[8];
  dtostrf(t, 4, 2, tBuf);
  dtostrf(h, 4, 2, hBuf);

  mqtt.publish(TOPIC_TEMP, tBuf);
  mqtt.publish(TOPIC_HUM,  hBuf);

  Serial.print("Published → Temp: ");
  Serial.print(tBuf);
  Serial.print(" °C | Hum: ");
  Serial.print(hBuf);
  Serial.println(" %");

  delay(5000);
}