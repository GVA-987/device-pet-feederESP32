#include "wifi_manager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

#define PIN_PULSADOR 23 // Pulsador para resetear las credenciales del wifi

WiFiManager wifiManager;

// Función que se ejecuta si la configuración es exitosa
void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Iniciando el modo AP...");
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

// Función para borrar las credenciales de la red Wi-Fi
void borrarCredenciales()
{
    Serial.println("Borrando credenciales de Wi-Fi...");
    wifiManager.resetSettings();
    Serial.println("Credenciales borradas. Reiniciando Dispositivo...");
    delay(1000);
    ESP.restart();
}

void inicializarWiFi()
{
    pinMode(PIN_PULSADOR, INPUT_PULLUP);
    wifiManager.setAPCallback(configModeCallback);

    if (!wifiManager.autoConnect("Dispensador_GVA"))
    {
        Serial.println("Conexión fallida, reiniciando...");
        ESP.restart();
    }

    Serial.println("Conexión Wi-Fi exitosa.");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void checkBtnCredencialesDelte()
{
    if (digitalRead(PIN_PULSADOR) == LOW)
    {
        delay(500); // Para evitar rebotes
        if (digitalRead(PIN_PULSADOR) == LOW)
        {
            borrarCredenciales();
        }
    }

    // Si estás usando el comando por Serial para probar:
    if (Serial.available())
    {
        String reset_cmd = Serial.readStringUntil('\n');
        reset_cmd.trim();
        reset_cmd.toLowerCase();
        if (reset_cmd == "borrar")
        {
            borrarCredenciales();
        }
    }
}