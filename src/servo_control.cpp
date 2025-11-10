#include "servo_control.h"

ServoControl::ServoControl(int servoPin, int open, int close)
    : pin(servoPin), openAngle(open), closeAngle(close), isOpen(false)
{
}

void ServoControl::begin()
{
    Serial.println("Inicializando servomotor...");
    servoMotor.attach(pin);
    closeGate();
    Serial.println("Servomotor listo.");
}

void ServoControl::openGate()
{
    Serial.println("Abriendo compuerta...");
    servoMotor.write(openAngle);
    delay(1000);
    isOpen = true;
}

void ServoControl::closeGate()
{
    Serial.println("Cerrando compuerta...");
    servoMotor.write(closeAngle);
    delay(1000);
    isOpen = false;
}

void ServoControl::testCycle()
{
    Serial.println("Ciclo de prueba del servomotor...");
    openGate();
    delay(1000);
    closeGate();
}

bool ServoControl::getState() const
{
    return isOpen;
}
