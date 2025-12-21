#include "../../include/actuador/Dispense.h"
#include "mqtt/MQTTClient.h"
#include <ESP32Servo.h>
#include <ArduinoJson.h>

Servo myservo;
const int SERVO_PIN = 13;

void setupDispenser()
{
    ESP32PWM::allocateTimer(0);
    myservo.setPeriodHertz(50);
    myservo.attach(SERVO_PIN, 500, 2400);
    myservo.write(0);
}

void executeManualDispense(int portions, PubSubClient &client)
{
    Serial.println("Iniciando ciclo de dispensación...");

    // Simular Apertura
    myservo.write(90);

    delay(1000 * portions);

    // Simular Cierre
    myservo.write(0);
    Serial.println("Compuerta cerrada.");

    StaticJsonDocument<256> report;

    // Estructura para Firestore
    report["portion"] = portions;
    report["status"] = "completado";
    report["type"] = "manual";

    // estado actual para que RTDB
    report["temp"] = 25.0; // Valores de ejemplo
    report["food"] = 75;
    report["online"] = true;

    char buffer[256];
    serializeJson(report, buffer);

    // Publicar reporte en MQTT
    if (client.publish(TOPIC_STATUS.c_str(), buffer))
    {
        Serial.println("Reporte de éxito enviado al Broker.");
    }
    else
    {
        Serial.println("Error al enviar el reporte MQTT.");
    }
}