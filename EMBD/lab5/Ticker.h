#include <Arduino.h>

#ifndef Ticker_h
#define Ticker_h

class Ticker {
  private: 
    unsigned long period;
    unsigned long previousStart;
    void (*trigger)(void);
  public: 
    Ticker(unsigned long p, void(*fun)(void));
    void check();
};

#endif
