#include "../include/sensor.h"

const int LED_PIN = D4;
const int CO2_ANALOG = A0;

void sensorInit (){
    pinMode(LED_PIN, OUTPUT);
    sensorSetAlertState(false);
}

void sensorSetAlertState (boolean state){
  if (state){
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
}

int sensorReadGasLevel (){
    return analogRead(CO2_ANALOG);
}