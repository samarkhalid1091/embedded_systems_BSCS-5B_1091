#include <Arduino.h>

/******************
 * ESP32 + MQTT Subscriber 2 (Serial Monitor)
 ******************/

#include <WiFi.h>
#include <PubSubClient.h>

// ---------- WiFi ----------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ---------- MQTT ----------
const char* mqtt_server = "10.13.21.170";
const int mqtt_port = 1883;

const char* TOPIC_TEMP = "home/lab2/temp";
const char* TOPIC_HUM  = "home/lab2/hum";

// ---------- MQTT ----------
WiFiClient espClient;
PubSubClient mqtt(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" | Value: ");
  Serial.println(msg);
}

void connectWiFi() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void connectMQTT() {
  while (!mqtt.connected()) {
    if (mqtt.connect("ESP32_Subscriber_2")) {   // UNIQUE ID
      mqtt.subscribe(TOPIC_TEMP);
      mqtt.subscribe(TOPIC_HUM);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  connectMQTT();
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
}