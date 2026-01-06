#include "../../include/actuador/Dispense.h"
#include "mqtt/MQTTClient.h"
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include "sensors/TemperatureIntService.h"
#include "sensors/WeightSensor.h"

Servo myservo;
const int SERVO_PIN = 13;

void setupDispenser()
{
    ESP32PWM::allocateTimer(0);
    myservo.setPeriodHertz(50);
    myservo.attach(SERVO_PIN, 500, 2400);
    myservo.write(0);

    myservo.write(45);
    delay(500);
    myservo.write(0);
}

void executeDispense(int portions, PubSubClient &client, String type)
{
    Serial.println("Iniciando ciclo de dispensación...");
    Serial.printf("Dispensando %d porciones (%s)...\n", portions, type.c_str());

    // Simular Apertura
    myservo.write(90);

    delay(1000 * portions);

    // Simular Cierre
    myservo.write(0);
    Serial.println("Compuerta cerrada.");

    delay(500);

    float currentTemp = getInternalTemperature();
    float currentWeight = getFoodWeight();

    StaticJsonDocument<256> report;
    // Estructura para Firestore
    report["event"] = "dispense_done";
    report["portion"] = portions;
    report["type"] = type;
    report["status"] = "completado";

    report["temp"] = currentTemp;
    report["food"] = currentWeight;
    report["online"] = true;

    char buffer[256];
    serializeJson(report, buffer);

    if (client.publish(TOPIC_STATUS.c_str(), buffer))
    {
        Serial.println("Reporte de éxito enviado al Broker.");
    }
    else
    {
        Serial.println("Error al enviar el reporte MQTT.");
    }
}