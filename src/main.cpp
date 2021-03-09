#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "../include/main.h"
#include "../include/conf.h"

const int LED_PIN = D4;
const int CO2_ANALOG = A0;

const char *wifiSsid = WIFI_SSID;
const char *wifiPassphrase = WIFI_PASSPHRASE;

IPAddress mqttIP(192, 168, 1, 2);
uint16_t mqttPort = 1883;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

const char *clientId = "GasSensor";
const char *gasLevelTopic = "gas/level";
const char *alertTopic = "gas/alert";

void setAlertState (boolean state){
  if (state){
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
}

void callback(char *topic, byte *payload, unsigned int length){

  const char *alertPayload = "event:alert";
  const char *cleanPayload = "event:clean";

  size_t stringPayloadSize = (length + 1) * sizeof(char);
  char *stringPayload = (char *)malloc(stringPayloadSize);
  
  if (stringPayload == NULL){
    Serial.println("[MQTT callback] Error while allocating memory for string payload.");
    return;
  }
  
  memset(stringPayload, 0, stringPayloadSize);
  memcpy(stringPayload, payload, length);

  Serial.print("Received \"");
  Serial.print(stringPayload);
  Serial.print("\" on topic ");
  Serial.println(topic);

  if (strcmp(topic, alertTopic) == 0){

    if (strcmp(stringPayload, alertPayload) == 0){
      setAlertState(true);
    }

    else if (strcmp(stringPayload, cleanPayload) == 0){
      setAlertState(false);
    }
  }

  free(stringPayload);
}

void connectWifi(){

  Serial.print("[WiFi] Connecting to ");
  Serial.println(wifiSsid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassphrase);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  Serial.println("[WiFi] Now connected.");
  Serial.print("[WiFi] Our IP address is: ");
  Serial.println(WiFi.localIP());
}

boolean connectMqtt(){

  Serial.print("[MQTT] Connecting to ");
  Serial.print(mqttIP);
  Serial.print(":");
  Serial.println(mqttPort);

  if (!mqtt.connect(clientId)){
    Serial.print("[MQTT] Failed to connect. State => ");
    Serial.println(mqtt.state());
  } else {
    Serial.println("[MQTT] Now connected.");
    mqtt.subscribe(alertTopic);
  }

  return mqtt.connected();
}

boolean publishGasLevel (float gasLevel){
  
  char co2LevelPayload[255];
  sprintf(co2LevelPayload, "%f", gasLevel);

  return mqtt.publish(gasLevelTopic, co2LevelPayload);
}

void setup (){

  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  setAlertState(false);

  connectWifi();

  mqtt.setServer(mqttIP, mqttPort);
  mqtt.setCallback(callback);
  
  connectMqtt();
}

void loop(){

  float gasLevel = 0;

  delay(1000);
  
  if (!mqtt.connected()){
    connectMqtt();
    return;
  }

  mqtt.loop();

  gasLevel = analogRead(CO2_ANALOG); 
 
  Serial.print("Gas level => ");
  Serial.println(gasLevel);

  if (!publishGasLevel(gasLevel)){
    Serial.print("Error while publishing gas level. State => ");
    Serial.println(mqtt.state());
  }
}
