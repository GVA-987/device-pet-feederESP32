#include <Arduino.h>
#include "Config.h"
#include "mqtt/MQTTClient.h"
#include "network/WifiService.h"
#include "services/FileService.h"
#include "services/ScheduleService.h"
#include <WiFi.h>
#include "Config.h"
#include "network/TimeService.h"

unsigned long lastStatusUpdate = 0;
const unsigned long statusInterval = 10000; // Enviar estado cada 10 segundos

void setup()
{
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);

    Serial.println("Inicializando Sistema... ");
    setupFiles();

    setupNetwork();

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
    // Mantener la conexión y procesar mensajes entrantes
    mqttLoop();
    checkSchedule();

    static unsigned long lastLog = 0;
    if (millis() - lastLog > 5000)
    {
        Serial.print("📅 Hora actual: ");
        Serial.println(getFormattedTime());
        lastLog = millis();
    }

    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 1000)
    {
        checkResetButton();
        lastCheck = millis();
    }

    // Enviar estado periódicamente
    unsigned long currentMillis = millis();
    if (currentMillis - lastStatusUpdate >= statusInterval)
    {
        lastStatusUpdate = currentMillis;
        int foodLevel = 20; // Ejemplo
        int rssi = WiFi.RSSI();

        sendStatus(foodLevel, rssi);
    }
}