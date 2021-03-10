#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "../include/wifi.h"
#include "../include/sensor.h"
#include "../include/mqtt.h"

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void mqttProcessMessageRoutine(char *topic, byte *payload, unsigned int length){

  const char *alertPayload = "event:alert";
  
  char msg[1024];

  size_t stringPayloadSize = (length + 1) * sizeof(char);
  char *stringPayload = (char *)malloc(stringPayloadSize);
  
  if (stringPayload == NULL){
    Serial.println("[MQTT] Error while allocating memory.");
    return;
  }
  
  memset(stringPayload, 0, stringPayloadSize);
  memcpy(stringPayload, payload, length);

  sprintf(msg, "[MQTT] Received %s on topic %s", stringPayload, topic);
  Serial.println(msg);

  sensorSetAlertState(
    strcmp(stringPayload, alertPayload) == 0
  );

  free(stringPayload);
}

boolean mqttPublishGasLevel (const char *topic, int gasLevel){
  
  char gasLevelPayload[255];
  sprintf(gasLevelPayload, "%d", gasLevel);
  
  return mqtt.publish(topic, gasLevelPayload);
}

boolean mqttConnect (
  IPAddress ip,
  uint16_t port,
  const char *deviceName, 
  const char *topics[], 
  size_t numberOfTopics
){

  size_t i = 0;

  mqtt.setServer(ip, port);
  mqtt.setCallback(mqttProcessMessageRoutine);
  
  if (!mqtt.connect(deviceName)){
    return false;
  }
  
  for (i = 0; i < numberOfTopics; i++){
    
    if (!mqtt.subscribe(topics[i])){
      return false;
    }
  }
  
  return true;
}

boolean mqttIsConnected (){
  return mqtt.connected();
}

int mqttState (){
  return mqtt.state();
}

boolean mqttLoop(){
  return mqtt.loop();
}