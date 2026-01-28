#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <Wire.h>

class DisplayManager
{
private:
    LiquidCrystal_I2C lcd;
    unsigned long lastUpdate;
    int currentScreen;
    const int sda_pin = 33;
    const int scl_pin = 32;
    const uint8_t lcd_addr = 0x27;

    const unsigned long interval = 4000; 

public:
    DisplayManager() : lcd(0x27, 16, 2), lastUpdate(0), currentScreen(0) {}

    void begin();
    void update(float peso, bool isWifiOk, String hora, int rssi);
    void showFeeding();
    void showSyncing();
    void showTaraStatus(bool start);
};

extern DisplayManager display;

#endif