#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

extern const char *WIFI_SSID;
extern const char *WIFI_PASS;
extern const char *MQTT_SERVER;
extern const int MQTT_PORT;

extern String DEVICE_ID;
extern String TOPIC_STATUS;
extern String TOPIC_COMMAND;

#endif