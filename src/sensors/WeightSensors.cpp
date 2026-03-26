#include "sensors/WeightSensor.h"
#include <LittleFS.h>

HX711 scale;

const int LOADCELL_DOUT_PIN = 26;
const int LOADCELL_SCK_PIN = 27;
const int TARE_BTN_PIN = 4;

float calibration_factor = 420.1; // Ajustar según calibración

void saveOffsetToMemory(long offset)
{
    File file = LittleFS.open("/offset.txt", "w");
    if (file)
    {
        file.print(offset);
        file.close();
        Serial.println("Offset guardado en memoria.");
    }
}

long readOffsetFromMemory()
{
    if (LittleFS.exists("/offset.txt"))
    {
        File file = LittleFS.open("/offset.txt", "r");
        String content = file.readString();
        file.close();

        long valor = content.toInt();
        if (valor != 0)
            return valor;
    }
    return 0; // Si no existe, devuelve 0
}

void setupWeightSensor()
{
    pinMode(TARE_BTN_PIN, INPUT_PULLUP);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor);

    long savedOffset = readOffsetFromMemory();
    if (savedOffset != 0)
    {
        scale.set_offset(savedOffset);
        Serial.print("Tara recuperada de memoria: ");
        Serial.println(savedOffset);
    }
    else
    {
        Serial.println("No hay tara guardada. Realizando tara inicial...");
        scale.tare();
        saveOffsetToMemory(scale.get_offset());
    }
}

void calibrateTare()
{
    Serial.println("Realizando TARA y guardando en memoria...");
    scale.tare();
    long newOffset = scale.get_offset();
    saveOffsetToMemory(newOffset);
    Serial.println("TARA finalizado y guardada.");
}

void updateWeightSensor()
{
    // Leer comando desde el monitor serial
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando == "TARAR")
    {
        calibrateTare();
    }
    // Fin de lectura de comando

    // Detectar si el botón se presiona (LOW porque es PULLUP)
    if (digitalRead(TARE_BTN_PIN) == LOW)
    {
        delay(50);
        if (digitalRead(TARE_BTN_PIN) == LOW)
        {
            calibrateTare();
            // Esperar a que suelte el botón para no repetir
            while (digitalRead(TARE_BTN_PIN) == LOW)
                ;
        }
    }
}

float getFoodWeight(int samples)
{
    // Esperar hasta 200ms si el sensor está ocupado
    unsigned long timeout = millis();
    while (!scale.is_ready() && (millis() - timeout < 200))
    {
        delay(1);
    }

    if (scale.is_ready())
    {
        float weight = scale.get_units(samples); // Promedio de samples lecturas
        if (weight > -2.0 && weight < 2.0)
        {
            return 0.0;
        }

        return (weight < 0) ? 0 : weight;
    }
    else
    {
        Serial.println("Error: Sensor de peso no listo."); // Debug
        return -1.0;
    }
}