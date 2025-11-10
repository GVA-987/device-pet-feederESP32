#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoControl
{
private:
    Servo servoMotor;
    int pin;
    int openAngle;
    int closeAngle;
    bool isOpen;

public:
    ServoControl(int servoPin, int open = 90, int close = 0);
    void begin();
    void openGate();
    void closeGate();
    void testCycle();
    bool getState() const;
};

#endif
