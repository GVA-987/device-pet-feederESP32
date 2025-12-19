#include "mqtt/MQTTClient.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Config.h"

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *message, unsigned int length)
{
    Serial.print("Mensaje recibido [");
    Serial.print(topic);
    Serial.print("] ");

    // comandos manuales que mande el Backend
}

void setupMQTT()
{
    if (DEVICE_ID == "") // Por si acaso no se ha inicializado
    {
        // DEVICE_ID = "ESP32_" + WiFi.macAddress();
        DEVICE_ID = "ESP-PET-J1PZ8X7-A3B6C9D";
        TOPIC_STATUS = DEVICE_ID + "/status";
        TOPIC_COMMAND = DEVICE_ID + "/command";
    }

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
}

void reconnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi perdido. Reintentando MQTT después...");
        return;
    }

    while (!client.connected())
    {
        Serial.print("Intentando conexión MQTT...");
        Serial.print(MQTT_SERVER);
        Serial.print("...");

        if (client.connect(DEVICE_ID.c_str()))
        {
            Serial.println("¡Conectado!");
            client.subscribe(TOPIC_COMMAND.c_str());
            Serial.print("Suscrito a: ");
            Serial.println(TOPIC_COMMAND);
        }
        else
        {
            Serial.print("Falló, rc=");
            Serial.print(client.state());
            Serial.println(" reintentando en 5 segundos");
            delay(5000);
        }
    }
}

// Función para enviar el estado
void sendStatus(float temp, int foodLevel, int rssi)
{
    StaticJsonDocument<200> doc;
    doc["temp"] = temp;
    doc["food"] = foodLevel;
    doc["rssi"] = rssi;
    doc["online"] = true;

    char buffer[200];
    serializeJson(doc, buffer);
    client.publish(TOPIC_STATUS.c_str(), buffer);
    Serial.println("Estado enviado al Backend");
}

void mqttLoop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}