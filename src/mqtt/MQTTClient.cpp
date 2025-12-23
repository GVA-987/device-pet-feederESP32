#include "mqtt/MQTTClient.h"
#include "../../include/actuador/Dispense.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "network/WifiService.h"

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
        executeManualDispense(portions, client);
    }
    else if (String(action) == "reset_wifi")
    {
        Serial.println("COMANDO REMOTO: Reset de WiFi solicitado.");
        resetNetworkSettings();
    }
}

void setupMQTT()
{
    if (DEVICE_ID)
    {
        TOPIC_STATUS = "petfeeder/" + DEVICE_ID + "/status";
        TOPIC_COMMAND = "petfeeder/" + DEVICE_ID + "/command";
    }

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

        if (client.connect(DEVICE_ID.c_str(), TOPIC_STATUS.c_str(), 1, true, "{\"online\": false}"))
        {
            Serial.println("¡Conectado!");

            client.publish(TOPIC_STATUS.c_str(), "{\"online\": true}", true);

            client.subscribe(TOPIC_COMMAND.c_str());
        }
        else
        {
            Serial.print("falló, rc=");
            Serial.print(client.state());
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
    client.publish(TOPIC_STATUS.c_str(), buffer, true);
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