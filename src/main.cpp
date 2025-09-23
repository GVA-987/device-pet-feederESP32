#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "wifi_manager.h"
#include "firebase_service.h"

unsigned long lastReport = 0;
const unsigned long reportInterval = 10000; // 10 segundos

void setup()
{
  Serial.begin(115200);

  inicializarWiFi();
  inicializarFirebase();
}

void loop()
{
  checkBtnCredencialesDelte();
  unsigned long now = millis();

  if (millis() - lastReport > reportInterval)
  {
    lastReport = millis();
    reportarEstado();
  }