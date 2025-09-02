#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "wifi_manager.h"
#include "firebase_service.h"

// #define LED_BUILTIN 2

void setup()
{
  Serial.begin(115200);
  // pinMode(LED_BUILTIN, OUTPUT);

  inicializarWiFi();
  inicializarFirebase();

  // El LED parpadea para confirmar la conexión
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(500);
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(500);
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(500);
  // digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  checkBtnCredencialesDelte();
  escucharFirebase();

  // delay(2000);
}