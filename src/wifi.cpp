#include "../include/wifi.h"

#include <ESP8266WiFi.h>
#include <time.h>

boolean wifiConnect (const char *ssid, const char *passphrase){

  const time_t startTime = time(NULL);
  const time_t timeoutSec = 30UL;

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passphrase);

  while (!wifiIsConnected()){
    
    if ((time(NULL) - startTime) >= timeoutSec){
      return false;
    }
    
    delay(500);
  }

  return true;
}

boolean wifiIsConnected (){
  return WiFi.status() == WL_CONNECTED;
}

void wifiLocalIp (char *buffer){
  strcpy(buffer, WiFi.localIP().toString().c_str());
}