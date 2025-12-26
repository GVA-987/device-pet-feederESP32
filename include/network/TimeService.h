#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <Arduino.h>

//  NTP y la zona horaria
void setupTime();

// Actualiza la hora
void updateTime();

// hora formateada como String (ej: "21:30:00")
String getFormattedTime();

// día de la semana (0 = Domingo, 1 = Lunes...)
int getDayOfWeek();

#endif