#include "sensors/DisplayManager.h"

DisplayManager display;

void DisplayManager::begin()
{

    Wire.begin(sda_pin, scl_pin);
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DOSIFICADOR");
    lcd.setCursor(0, 1);
    lcd.print("INICIALIZANDO...");
}

void DisplayManager::showFeeding()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">>> DOSIFICANDO <<<");
    lcd.setCursor(0, 1);
    lcd.print("   ESPERANDO...");
}

void DisplayManager::update(float peso, bool isWifiOk, String hora, int rssi)
{
    if (millis() - lastUpdate > interval)
    {
        lastUpdate = millis();
        currentScreen = (currentScreen + 1) % 3;

        lcd.clear();
        switch (currentScreen)
        {
        case 0: // Estado de Conexión
            lcd.setCursor(0, 0);
            lcd.print("WIFI:");
            lcd.print(isWifiOk ? "ONLINE" : "OFFLINE");
            lcd.setCursor(0, 1);
            lcd.print(" RSSI:");
            lcd.print(rssi);
            break;

        case 1: // Datos del Sensor de Peso
            lcd.setCursor(0, 0);
            lcd.print("PESO COMIDA:");
            lcd.setCursor(0, 1);
            lcd.print("[ ");
            lcd.print(peso, 1);
            lcd.print(" g ]");
            break;

        case 2: // Tiempo
            lcd.setCursor(0, 0);
            lcd.print("HORA ACTUAL:");
            lcd.setCursor(0, 1);
            lcd.print(hora == "" ? "SINCRONIZANDO..." : hora);
            break;
        }
    }
}