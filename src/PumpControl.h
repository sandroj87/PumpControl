// PumpControl.h
#ifndef PUMPCONTROL_H
#define PUMPCONTROL_H

#include <Arduino.h>

class PumpControl {
public:

    PumpControl(uint8_t pin, bool activeHigh = true, float flow = 0 );

    void setFlow(float flow);
    void setDropTimes(unsigned long dropTimeOn, unsigned long dropTimeOff);
    void resetDropCounter();
    bool stateChanged();
    bool getState();
    void on();
    void onFor(unsigned long durataMs);
    void off();
    void stop();
    void addVolume(float volume);
    void drop(unsigned long nDrops);
    unsigned long getDrops();
    void update();

private:
    uint8_t _pin;
    bool _logic;
    bool _stato;
    bool _lastState = false;
    unsigned long _durataFine;
    bool _timerAttivo;
    float _flow;
    float _volume;
    bool _drop = false;
    bool _dropOn = false;
    unsigned long _nDrops = 1;
    unsigned long _dropCount = 0;
    unsigned long _dropTimeOn = 250;
    unsigned long _dropTimeOff = 250;
    unsigned long _timeNext;
    unsigned long _totalDrops = 0;
};

#endif // PUMPCONTROL_H