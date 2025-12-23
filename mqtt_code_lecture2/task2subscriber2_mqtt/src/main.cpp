#include <Arduino.h>

/******************
 * MQTT Subscriber 2 (Serial Monitor)
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

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  mqtt.connect("ESP32_Subscriber_2"); // UNIQUE ID
  mqtt.subscribe(TOPIC_TEMP);
  mqtt.subscribe(TOPIC_HUM);
}

void loop() {
  mqtt.loop();
}