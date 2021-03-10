#pragma once

#include <Arduino.h>

void wifiLocalIp (char *buffer);
boolean wifiConnect(const char *ssid, const char *passphrase);
boolean wifiIsConnected();