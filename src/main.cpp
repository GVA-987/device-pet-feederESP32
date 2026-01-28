#include <Arduino.h>
#include "Config.h"
#include "mqtt/MQTTClient.h"
#include "network/WifiService.h"
#include "services/FileService.h"
#include "services/ScheduleService.h"
#include "sensors/WeightSensor.h"
#include <WiFi.h>
#include "Config.h"
#include "network/TimeService.h"
#include "actuador/Dispense.h"
#include "sensors/DisplayManager.h"
#include "sensors/TemperatureIntService.h"

unsigned long lastStatusUpdate = 0;
const unsigned long statusInterval = 5000; // Enviar estado cada 5 segundos

void setup()
{
    Serial.begin(115200);
    display.begin();
    pinMode(0, INPUT_PULLUP);

    Serial.println("Inicializando Sistema... ");
    setupFiles();

    setupNetwork();
    setupWeightSensor();
    setupDispenser();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Conectado");

    setupTime();
    setupMQTT();
    // reconnect();
}

void loop()
{
    mqttLoop();
    checkSchedule();
    updateWeightSensor();
    float pesoActual = getFoodWeight();
    
    static float weightRead = 0;
    if (pesoActual != -1.0) {
        weightRead = pesoActual;
    }

    int rssi = WiFi.RSSI();
    bool wifiOk = (WiFi.status() == WL_CONNECTED);
    String hora = getFormattedTime();

    static unsigned long lastLcdUpdate = 0;
    if (millis() - lastLcdUpdate >= 500) {
        display.update(weightRead, wifiOk, hora, rssi);
        lastLcdUpdate = millis();
    }

    static unsigned long lastLog = 0;
    if (millis() - lastLog > 5000) {
        Serial.print(" Hora: "); Serial.print(hora);
        Serial.print(" | Peso: "); Serial.print(weightRead);
        Serial.print(" | Temp: "); Serial.println(getInternalTemperature());
        lastLog = millis();
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastStatusUpdate >= statusInterval) {
        lastStatusUpdate = currentMillis;
        sendStatus(rssi, weightRead);
    }

    // Botón de Reset
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 1000) {
        checkResetButton();
        lastCheck = millis();
    }
}