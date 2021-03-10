#pragma once

#include <Ethernet.h>
#include <Arduino.h>

boolean mqttConnect (
  IPAddress ip,
  uint16_t port,
  const char *deviceName, 
  const char *topics[], 
  size_t numberOfTopics
);
boolean mqttIsConnected();
boolean mqttPublishGasLevel (const char *topic, int gasLevel);
int mqttState();
boolean mqttLoop();
void mqttProcessMessageRoutine(char *topic, byte *payload, unsigned int length);