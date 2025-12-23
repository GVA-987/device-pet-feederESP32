#ifndef WIFISERVICE_H
#define WIFISERVICE_H

#include <WiFiManager.h>

extern WiFiManager wm;

void setupNetwork();

void resetNetworkSettings();
void checkResetButton();

#endif