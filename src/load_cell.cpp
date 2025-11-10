#include "load_cell.h"

LoadCell::LoadCell(uint8_t doutPin, uint8_t sckPin, float calibration)
    : calibrationFactor(calibration), lastReading(0.0f)
{
    scale.begin(doutPin, sckPin);
}

void LoadCell::begin()
{
    Serial.println("Iniciando celda de carga...");
    if (!scale.is_ready())
    {
        Serial.println("modulo no conectado o mal cableado");
        return;
    }

    scale.set_scale(calibrationFactor);
    scale.tare();
    Serial.println("Celda de carga calibrada y lista.");
}

float LoadCell::readWeight()
{
    if (!scale.is_ready())
    {
        Serial.println("modulo no listo, devolviendo último valor.");
        return lastReading;
    }

    lastReading = scale.get_units(10); // promedio de 10 lecturas
    return lastReading;
}

void LoadCell::tare()
{
    Serial.println("Ajustando tara...");
    scale.tare();
}

bool LoadCell::isReady()
{
    return scale.is_ready();
}

float LoadCell::getLastReading() const
{
    return lastReading;
}
