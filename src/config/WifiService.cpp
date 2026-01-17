#include "../include/network/WifiService.h"
#include "Config.h"
#include <WiFiManager.h>
#include <WiFi.h>

WiFiManager wm;
const int TRIGGER_PIN = 0;
const String rst = "";

void checkResetButton()
{
    if (digitalRead(TRIGGER_PIN) == LOW)
    {
        Serial.println("Botón de reset detectado. Mantenga presionado por 3 segundos...");
        delay(3000);
        if (digitalRead(TRIGGER_PIN) == LOW)
        {
            resetNetworkSettings();
        }
    }
    if (rst == "reset")
    {
        resetNetworkSettings();
    }
}

void setupNetwork()
{
    wm.setTitle("PetFeeder - Conectar WiFi");
    wm.setConfigPortalTimeout(300);

    std::vector<const char *> menu = {"wifi", "exit"};
    wm.setMenu(menu);

    wm.setCustomHeadElement(
        "<style>"
        // Fuente y fondo
        "body { background-color: #2c3e50; font-family: 'Roboto', sans-serif; color: #ecf0f1; margin: 0; display: flex; justify-content: center; align-items: center; min-height: 100vh; }"
        // Contenedor principal
        ".wrap { background-color: #34495e; padding: 30px; border-radius: 12px; box-shadow: 0 8px 16px rgba(0,0,0,0.4); max-width: 400px; width: 90%; }"
        // Título
        "h1 { color: #2ecc71; text-align: center; margin-bottom: 25px; font-size: 2.2em; }"
        // Input y Selectores
        "input[type='text'], input[type='password'], select { "
        "   width: calc(100% - 20px); padding: 12px; margin-bottom: 15px; border: 1px solid #4a6a8c; border-radius: 6px; "
        "   background-color: #253342; color: #ecf0f1; box-sizing: border-box;"
        "}"
        "input[type='text']:focus, input[type='password']:focus, select:focus { border-color: #2ecc71; outline: none; box-shadow: 0 0 8px rgba(46, 204, 113, 0.4); }"
        // Botones
        "button { "
        "   background-color: #2ecc71; color: white; padding: 14px 25px; border: none; border-radius: 6px; cursor: pointer; "
        "   width: 100%; font-size: 1.1em; transition: background-color 0.3s ease;"
        "}"
        "button:hover { background-color: #27ae60; }"
        // Mensajes de estado
        ".msg { padding: 10px; margin-bottom: 15px; border-radius: 6px; text-align: center; }"
        ".msg-ok { background-color: #27ae60; color: white; }"
        ".msg-warn { background-color: #e67e22; color: white; }"
        // Enlaces
        "a { color: #3498db; text-decoration: none; }"
        "a:hover { text-decoration: underline; }"
        "</style>"
        // Añadir logo o icono
        "<h1 style='text-align:center;'>🐾 PetFeeder Pro</h1>");

    String apName = "PetFeeder - " + DEVICE_ID;

    Serial.println("Iniciando WiFiManager...");

    if (!wm.autoConnect(apName.c_str()))
    {
        Serial.println("Fallo en la conexión o timeout del portal.");
        delay(3000);
        ESP.restart();
    }

    Serial.println("WiFi Conectado con éxito.");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());
}

void resetNetworkSettings()
{
    Serial.println("Borrando configuraciones de WiFi...");
    wm.resetSettings();
    WiFi.disconnect(true, true);
    delay(3000);
    ESP.restart();
}