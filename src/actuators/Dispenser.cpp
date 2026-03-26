#include "../../include/actuador/Dispense.h"
#include "mqtt/MQTTClient.h"
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include "sensors/TemperatureIntService.h"
#include "sensors/WeightSensor.h"
#include "sensors/DisplayManager.h"

Servo myservo;
const int SERVO_PIN = 13;
extern DisplayManager display;
String errorMsg = "";

void setupDispenser()
{
    ESP32PWM::allocateTimer(2);
    myservo.setPeriodHertz(50);
    myservo.attach(SERVO_PIN, 500, 2400);
    myservo.write(52);
}

void executeDispense(int gramsToDispense, int portions, PubSubClient &client, String type)
{
    if (gramsToDispense <= 0)
    {
        return;
    }
    if (!myservo.attached())
    {
        myservo.attach(SERVO_PIN, 500, 2400);
    }

    // Parametros de dosificacion
    const int SERVO_FAST = 0;
    const int SERVO_FINE = 26;
    const int SERVO_CLOSED = 52;

    const float PORCENTAJE_FINO = 0.60f;  // 60% rápido, luego lento
    const float OFFSET_PREDICTIVO = 8.0f; // Cierra 8g antes
    const float TOLERANCIA = 5.0f;        // +/- 5g es aceptable
    const unsigned long TIMEOUT_MS = 25000UL;

    // Logica de dosificación
    updateWeightSensor();
    float pesoInicial = getFoodWeight(20);
    if (pesoInicial < 0)
    {
        Serial.println("Error de lectura inicial. Abortando.");
        return;
    }
    float pesoObjetivo = pesoInicial - (float)gramsToDispense;
    if (pesoObjetivo < 0)
        pesoObjetivo = 0;

    // float gramosParaFino = (float)gramsToDispense * 0.50f;
    // float umbralFaseFina = pesoInicial - gramosParaFino;

    // float umbralFaseFina = pesoObjetivo + ZONA_FINA;

    // float pesoSwitchFino = pesoInicial - (float)(gramsToDispense - (int)ZONA_FINA);

    // float margenCompensacion = 20.0; // ayuda a compensar lecturas inestables
    float umbralFaseFina = pesoInicial - ((float)gramsToDispense * PORCENTAJE_FINO);

    display.showFeedingStatus((float)gramsToDispense);
    Serial.printf("Comando recibido: %d gramos / %d porciones\n", gramsToDispense, portions);

    String finalStatus = "completado";
    String errorMsg = "";

    // display.showFeeding(pesoInicial, pesoInicial, (float)gramsToDispense);

    // Reporte MQTT

    StaticJsonDocument<128> busyReport;
    busyReport["status"] = "busy";
    busyReport["event"] = "dispensing";
    busyReport["online"] = true; // Sigue online estado ocupado
    char busyBuffer[128];
    serializeJson(busyReport, busyBuffer);
    client.publish(TOPIC_STATUS.c_str(), busyBuffer);

    myservo.write(SERVO_FAST); // Abrir compuerta
    bool faseFinaActiva = false;
    unsigned long startTime = millis();
    float pesoActual = pesoInicial;

    while (true)
    {
        updateWeightSensor();
        float lectura = getFoodWeight(1);

        if (lectura != -1.0)
        {
            pesoActual = lectura;
        }

        client.loop();

        // ── Timeout de seguridad
        if (millis() - startTime > TIMEOUT_MS)
        {
            Serial.println("Timeout alcanzado. Cerrando.");
            break;
        }

        // ── Transición a fase fina
        if (!faseFinaActiva && pesoActual <= umbralFaseFina)
        {
            myservo.write(SERVO_FINE);
            faseFinaActiva = true;
        }

        float puntoCierre = faseFinaActiva ? (pesoObjetivo + OFFSET_PREDICTIVO) : pesoObjetivo;

        if (pesoActual <= puntoCierre)
        {
            myservo.write(SERVO_CLOSED);
            break;
        }
    }
    delay(2500);

    updateWeightSensor();
    float currentWeight = getFoodWeight(20);
    float realDispensado = pesoInicial - currentWeight;

    if (realDispensado < 2.0f)
    {
        finalStatus = "error";
        errorMsg = "TOLVA_OBSTRUIDA_O_VACIA";
    }
    else if (realDispensado < (gramsToDispense - TOLERANCIA))
    { // Margen de 5g
        finalStatus = "parcial";
        errorMsg = "COMIDA_INSUFICIENTE";
    }
    else if (realDispensado < (gramsToDispense + TOLERANCIA))
    { // Margen de 5g
        finalStatus = "parcial";
        errorMsg = "COMIDA_EXCESIVA";
    }
    else
    {
        finalStatus = "completado";
        errorMsg = "";
    }

    display.showDispenseResult(finalStatus, realDispensado);

    StaticJsonDocument<400> report;
    // Estructura para Firestore
    report["event"] = (finalStatus == "error") ? "dispense_error" : "dispense_done";
    ;
    report["status"] = finalStatus;
    report["requested_grams"] = gramsToDispense;
    report["real_grams"] = realDispensado;
    report["portion"] = portions;
    report["type"] = type;
    report["error_type"] = errorMsg;
    report["food"] = currentWeight;

    char buffer[400];
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