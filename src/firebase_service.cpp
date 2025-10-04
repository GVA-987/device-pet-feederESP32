#include <Firebase_ESP_Client.h>
#include "firebase_service.h"
#include "firebase_cred.h"
#include <time.h>

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData fbdo;
FirebaseData stream;

FirebaseData fbdoFirestore;

#define Relay_dispense 27

extern "C"
{
    uint8_t temprature_sens_read();
}

String streamPath;

void streamCallback(FirebaseStream data);
void streamTimeoutCallback(bool timeout);
void reportarEstado();
void logDispenseToFirestore(const char *type, const char *status, int portionSize);
void sincronizarTiempoNTP();
void inicializarFirebase();

void sincronizarTiempoNTP()

{
    Serial.println("Sincronizando tiempo con NTP (en UTC)...");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    struct tm timeinfo;
    int maxAttempts = 20;

    while (!getLocalTime(&timeinfo) && maxAttempts > 0)
    {
        Serial.print(".");
        delay(100);
        maxAttempts--;
    }

    if (maxAttempts > 0)
    {
        Serial.print("\n✅ Hora NTP sincronizada. El tiempo actual (UTC) es: ");
        Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");

        time_t now = time(nullptr);
        Serial.printf("   Timestamp UNIX enviado (segundos): %ld\n", now);
    }
    else
    {
        Serial.println("\n❌ Falla al obtener la hora del servidor NTP. La conexión será inestable.");
    }
}

void inicializarFirebase()
{
    Serial.println("Conectando a Firebase...");
    sincronizarTiempoNTP();

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.fcs.upload_buffer_size = 4096;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);

    while (Firebase.ready() == false)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\n✅ Conexión con Firebase establecida.");

    pinMode(Relay_dispense, OUTPUT);

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

    String commands = DEVICE_ID;
    commands += "/commands/dispense_manual";

    Serial.printf("🔔 Cambio detectado en %s\n", data.dataPath().c_str());

    if (data.dataType() == "string")
    {
        String valor = data.stringData();

        if (valor == "activado")
        {
            Serial.println("➡ DISPENSAR ACTIVADO");
            digitalWrite(Relay_dispense, HIGH);
            delay(3000);
            digitalWrite(Relay_dispense, LOW);

            delay(200);
            logDispenseToFirestore("manual", "completado", 1);

            Firebase.RTDB.setString(&fbdo, commands.c_str(), "desactivado");
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

void logDispenseToFirestore(const char *type, const char *status, int portionSize)
{
    FirebaseJson content;
    fbdoFirestore.clear();

    time_t now = time(nullptr);

    content.set("fields/deviceId/stringValue", DEVICE_ID);
    content.set("fields/portion/integerValue", String(portionSize));
    content.set("fields/type/stringValue", type);
    content.set("fields/status/stringValue", status);
    content.set("fields/timestamp/integerValue", String((long)now));

    String project_id = PROJECT_ID;
    const char *collectionName = "dispense_history";

    Serial.println("📤 Enviando registro a Firestore...");

    if (Firebase.Firestore.createDocument(&fbdoFirestore, project_id.c_str(), "", collectionName, content.raw()))
    {
        Serial.println("✅ Historial de dispensado registrado en Firestore.");
    }
    else
    {
        Serial.print("❌ Error al registrar historial en Firestore: ");
        Serial.println(fbdoFirestore.errorReason());
    }
}
// Callback cuando hay timeout en el stream
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
    String pathLastSeen = basePath;
    pathLastSeen += "/lastSeen";

    // Señal WiFi
    int rssi = WiFi.RSSI();
    unsigned long uptime = millis() / 1000;
    float chipTempC = (temprature_sens_read() - 32) / 1.8;

    if (Firebase.RTDB.setInt(&fbdo, pathRssi.c_str(), rssi) &&
        Firebase.RTDB.setFloat(&fbdo, pathTemp.c_str(), chipTempC) &&
        Firebase.RTDB.setInt(&fbdo, pathUptime.c_str(), uptime) &&
        Firebase.RTDB.setString(&fbdo, pathOnline.c_str(), "conectado") &&
        Firebase.RTDB.setInt(&fbdo, pathLastSeen.c_str(), time(nullptr)))
    {
        Serial.print("📡 Estado actualizado: ");
    }
    else
    {
        Serial.print("❌ Error reportando estado: ");
        Serial.println(fbdo.errorReason());
    }
}