#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>

void setupMQTT();

void reconnect();

void mqttLoop();

void sendStatus(int rssi);

extern String DEVICE_ID;
extern String TOPIC_STATUS;
extern String TOPIC_COMMAND;

#endif