#include <Firebase_ESP_Client.h>
#include "firebase_service.h"
#include "firebase_cred.h"

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData fbdo;
FirebaseData stream;

#define Relay_dispense 27

extern "C"
{
    uint8_t temprature_sens_read();
}

String streamPath;

void streamCallback(FirebaseStream data);
void streamTimeoutCallback(bool timeout);
void reportarEstado();

void inicializarFirebase()
{
    Serial.println("Conectando a Firebase...");

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    pinMode(Relay_dispense, OUTPUT);

    // configuracion en el nodo del equipo
    String streamPath = DEVICE_ID;
    streamPath += "/commands/dispense_manual";

    if (!Firebase.RTDB.beginStream(&stream, streamPath.c_str()))
    {
        Serial.print("❌ Error iniciando stream: ");
        Serial.println(stream.errorReason());
    }
    else
    {
        Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
        Serial.println("✅ Escuchando cambios en RTDB...");
    }
}

// Callback cuando hay cambios en RTDB
void streamCallback(FirebaseStream data)
{
    Serial.printf("🔔 Cambio detectado en %s\n", data.dataPath().c_str());

    if (data.dataType() == "string")
    {
        String valor = data.stringData();

        if (valor == "activado")
        {
            Serial.println("➡ DISPENSAR ACTIVADO");
            digitalWrite(Relay_dispense, HIGH);
        }
        else if (valor == "desactivado")
        {
            Serial.println("➡ DISPENSAR DESACTIVADO");
            digitalWrite(Relay_dispense, LOW);
        }
        else
        {
            Serial.print("⚠ Valor inesperado: ");
            Serial.println(valor);
        }
    }
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
        Serial.println("⏳ Stream timeout, reconectando...");
    if (!stream.httpConnected())
        Serial.printf("❌ Error de stream: %s\n", stream.errorReason().c_str());
}

// Reportar estado del dispositivo
void reportarEstado()
{
    String basePath = DEVICE_ID;
    basePath += "/status";

    // sub-rutas para el estado del equipo
    String pathRssi = basePath;
    pathRssi += "/rssi";
    String pathTemp = basePath;
    pathTemp += "/temperature";
    String pathUptime = basePath;
    pathUptime += "/uptime";
    String pathOnline = basePath;
    pathOnline += "/online";

    // Señal WiFi
    int rssi = WiFi.RSSI();                                // Señal WiFi
    unsigned long uptime = millis() / 1000;                // Tiempo encendido (segundos)
    float chipTempC = (temprature_sens_read() - 32) / 1.8; // Temperatura interna en °C

    if (Firebase.RTDB.setInt(&fbdo, pathRssi.c_str(), rssi) &&
        Firebase.RTDB.setFloat(&fbdo, pathTemp.c_str(), chipTempC) &&
        Firebase.RTDB.setInt(&fbdo, pathUptime.c_str(), uptime) &&
        Firebase.RTDB.setString(&fbdo, pathOnline.c_str(), "conectado"))
    {
        Serial.print("📡 Estado actualizado: ");
    }
    else
    {
        Serial.print("❌ Error reportando estado: ");
        Serial.println(fbdo.errorReason());
    }
}