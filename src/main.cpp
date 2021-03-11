#include <Arduino.h>
#include "../include/main.h"
#include "../include/conf.h"
#include "../include/sensor.h"
#include "../include/wifi.h"
#include "../include/mqtt.h"

void setup (){
  Serial.begin(9600);
  sensorInit();
  Serial.println("");
}

void loop(){

  /* MQTT parameters */
  IPAddress mqttIP;
  uint16_t mqttPort = MQTT_PORT;
  const char *mqttDeviceName = "GasSensor";
  const char *mqttTopics[] = {
    "jeedom/gas/alert"
  };
  const char *mqttGasLevelTopic = "jeedom/gas/level";

  /* WiFi parameters */
  const char *wifiSsid = WIFI_SSID;
  const char *wifiPassphrase = WIFI_PASSPHRASE;

  int gasLevel = 0;
  char msg[1024];
  char ip[255];

  delay(1000);

  if (!wifiIsConnected()){
    
    Serial.println("[WiFi] Not connected to WiFi.");
    sprintf(msg, "[WiFi] Connecting to %s...", wifiSsid);
    Serial.println(msg);
    
    if (!wifiConnect(wifiSsid, wifiPassphrase)){
      Serial.println("[WiFi] Could not connect to WiFi.");
      return;
    }

    Serial.println("[WiFi] Now connected.");
    wifiLocalIp(ip);
    sprintf(msg, "[WiFi] Our IP address is: %s", ip);
    Serial.println(msg);
  }
  
  if (!mqttIsConnected()){

    if (!mqttIP.fromString(MQTT_IP_ADDR)){
      Serial.println("[MQTT] Could not translate MQTT_IP_ADDR to a valid IP address.");
      return;
    }

    Serial.println("[MQTT] Not connected.");
    sprintf(msg, "[MQTT] Connecting to %s:%u...", mqttIP.toString().c_str(), (unsigned int)mqttPort);
    Serial.println(msg);
    
    if (!mqttConnect(
      mqttIP,
      mqttPort,
      mqttDeviceName, 
      mqttTopics, 
      sizeof(mqttTopics) / sizeof(const char *)
    )){
      sprintf(msg, "[MQTT] Failed to connect. State => %d", mqttState());
      Serial.println(msg);
      return;
    }
    Serial.println("[MQTT] Now connected.");
  }

  gasLevel = sensorReadGasLevel(); 
 
  sprintf(msg, "[SENSOR] Gas level => %d", gasLevel);
  Serial.println(msg);

  if (!mqttPublishGasLevel(mqttGasLevelTopic, gasLevel)){
    sprintf(
      msg, 
      "[MQTT] Error while publishing gas level to %s. State => %d", 
      mqttGasLevelTopic,
      mqttState()
    );
    Serial.println(msg);
  }

  if (!mqttLoop()){
    sprintf(
      msg, 
      "[MQTT] Error while processing MQTT messages. State => %d", 
      mqttState()
    );
    Serial.println(msg);
  }
}
