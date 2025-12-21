#include "mqtt/MQTTClient.h"
#include "../../include/actuador/Dispense.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Config.h"

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    const char *action = doc["action"];
    int portions = doc["portion"] | 1;

    if (String(action) == "dispense")
    {
        // Llamamos a la lógica separada
        executeManualDispense(portions, client);
    }
}

void setupMQTT()
{
    if (DEVICE_ID)
    {
        TOPIC_STATUS = "petfeeder/" + DEVICE_ID + "/status";
        TOPIC_COMMAND = "petfeeder/" + DEVICE_ID + "/command";
    }

    // DEVICE_ID = "ESP32_" + WiFi.macAddress();
    // TOPIC_STATUS = "petfeeder/" + DEVICE_ID + "/status";
    // TOPIC_COMMAND = "petfeeder/" + DEVICE_ID + "/command";

    Serial.print("ID del Dispositivo: ");
    Serial.println(DEVICE_ID);

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);

    client.setSocketTimeout(15);
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