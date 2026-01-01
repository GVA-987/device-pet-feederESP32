#ifndef WEIGHTSENSOR_H
#define WEIGHTSENSOR_H

#include <Arduino.h>
#include "HX711.h"

// Inicializa el sensor y aplica la tara
void setupWeightSensor();

// Retorna el peso actual en gramos
float getFoodWeight();
void updateWeightSensor();
void calibrateTare();
#endif