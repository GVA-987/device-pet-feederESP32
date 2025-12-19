#include <Arduino.h>
#include "Config.h"
#include "mqtt/MQTTClient.h"
#include <WiFi.h>
#include "Config.h"
// #include "sensors/TempSensor.h"

unsigned long lastStatusUpdate = 0;
const unsigned long statusInterval = 30000; // Enviar estado cada 30 segundos

void setup()
{
    Serial.begin(115200);

    // Conectar WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Conectado");

    // Configurar MQTT
    setupMQTT();
}

void loop()
{
    // Mantener la conexión y procesar mensajes entrantes
    mqttLoop();

    // Enviar estado periódico
    unsigned long currentMillis = millis();
    if (currentMillis - lastStatusUpdate >= statusInterval)
    {
        lastStatusUpdate = currentMillis;
        float tempSimulada = 24.5;
        int comidaSimulada = 80;
        int rssi = WiFi.RSSI();

        sendStatus(tempSimulada, comidaSimulada, rssi);
    }
}