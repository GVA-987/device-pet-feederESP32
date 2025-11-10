#ifndef LOAD_CELL_H
#define LOAD_CELL_H

#include <Arduino.h>
#include <HX711.h>

class LoadCell
{
private:
    HX711 scale;
    float calibrationFactor;
    float lastReading;

public:
    LoadCell(uint8_t doutPin, uint8_t sckPin, float calibration = -7050.0f);
    void begin();
    float readWeight();
    void tare();
    bool isReady();
    float getLastReading() const;
};

#endif
