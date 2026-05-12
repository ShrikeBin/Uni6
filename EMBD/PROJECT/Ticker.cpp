#include <Arduino.h>

#include "Ticker.h"

Ticker::Ticker(unsigned long p, void(*fun)(void)) {
  period = p;
  trigger = fun;
  previousStart = 0;
}
void Ticker::check() {
  unsigned long timeNow = millis();
  if(timeNow - previousStart > period) {
    // call function
    (*trigger)();
    previousStart = timeNow;
  }
}
