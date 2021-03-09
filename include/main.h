#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

void connectWifi();
void setAlertState (boolean state);
boolean connectMqtt ();
void callback(char *topic, byte *payload, unsigned int length);
void setup ();
void loop();

#endif