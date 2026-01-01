#include "sensors/WeightSensor.h"

HX711 scale;

const int LOADCELL_DOUT_PIN = 26;
const int LOADCELL_SCK_PIN = 27;
const int TARE_BTN_PIN = 4;

float calibration_factor = 420.0;

void setupWeightSensor()
{
    pinMode(TARE_BTN_PIN, INPUT_PULLUP);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor);
    Serial.println("Sensor de peso inicializado y tarado.");
}

void calibrateTare()
{
    Serial.println("Realizando TARA manual...");
    scale.tare();
    Serial.println("TARA finalizado. Sensor en 0g. ");
}
void updateWeightSensor()
{
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

float getFoodWeight()
{
    if (scale.is_ready())
    {
        // Retorna el promedio de 5 lecturas para mayor estabilidad
        float weight = scale.get_units(5);
        return (weight < 0) ? 0 : weight; // Evita valores negativos
    }
    else
    {
        Serial.println("HX711 no encontrado.");
        return 0;
    }
}