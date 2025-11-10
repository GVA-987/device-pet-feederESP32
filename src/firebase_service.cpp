#include <Firebase_ESP_Client.h>
#include "firebase_service.h"
#include "firebase_cred.h"
#include "load_cell.h"
#include "servo_control.h"
#include <time.h>

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData fbdo;
FirebaseData stream;

FirebaseData fbdoFirestore;

#define Relay_dispense 27
#define HX711_DOUT 33
#define HX711_SCK 32
#define SERVO_PIN 13

LoadCell loadCell(HX711_DOUT, HX711_SCK, -7050.0);
ServoControl servoControl(SERVO_PIN, 90, 0);

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
void reconexionFirebase();

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
        Serial.print("\n Hora NTP sincronizada. El tiempo actual (UTC) es: ");
        Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");

        time_t now = time(nullptr);
        Serial.printf("   Timestamp UNIX enviado (segundos): %ld\n", now);
    }
    else
    {
        Serial.println("\n Falla al obtener la hora del servidor NTP. La conexión será inestable.");
    }
}

void reconexionFirebase()
{
    Serial.println("Intentando reconectar a Firebase...");
    Firebase.RTDB.endStream(&stream); // Finalizar el stream actual

    Firebase.begin(&config, &auth);
    delay(500);

    if (Firebase.ready())
    {
        Serial.println("Firebase re-inicializado con éxito.");

        //  Reestablecer el stream RTDB
        String streamPath = DEVICE_ID;
        streamPath += "/commands/dispense_manual";

        if (!Firebase.RTDB.beginStream(&stream, streamPath.c_str()))
        {
            Serial.print("Error reestableciendo stream: ");
            Serial.println(stream.errorReason());
        }
        else
        {
            Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
            Serial.println("Stream RTDB reestablecido.");
        }
    }
    else
    {
        Serial.println("Fallo en re-inicializar Firebase.");
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

    config.timeout.serverResponse = 10000;
    config.timeout.socketConnection = 10000;
    Firebase.begin(&config, &auth);

    while (Firebase.ready() == false)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\n Conexión con Firebase establecida.");

    servoControl.begin();

    pinMode(Relay_dispense, OUTPUT);

    String streamPath = DEVICE_ID;
    streamPath += "/commands/dispense_manual";

    if (!Firebase.RTDB.beginStream(&stream, streamPath.c_str()))
    {
        Serial.print("Error iniciando stream: ");
        Serial.println(stream.errorReason());
    }
    else
    {
        Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
        Serial.println("Escuchando cambios en RTDB...");
    }
}

// Callback cuando hay cambios en RTDB
void streamCallback(FirebaseStream data)
{
    String commands = DEVICE_ID;
    commands += "/commands/dispense_manual";

    String reception = DEVICE_ID;
    reception += "/commands/command_reception";

    Serial.printf("Cambio detectado en %s\n", data.dataPath().c_str());
    Firebase.RTDB.setString(&fbdo, reception.c_str(), "success");

    if (data.dataType() == "string")
    {
        String valor = data.stringData();

        if (valor == "activado")
        {
            Serial.println("Dispenador activado manualmente.");
            servoControl.openGate();
            delay(2500);
            servoControl.closeGate();

            digitalWrite(Relay_dispense, HIGH);
            delay(3000);
            digitalWrite(Relay_dispense, LOW);

            delay(200);
            logDispenseToFirestore("manual", "completado", 1);

            Firebase.RTDB.setString(&fbdo, commands.c_str(), "desactivado");
        }
        else if (valor == "desactivado")
        {
            Serial.println("Dispenador desactivado.");
            digitalWrite(Relay_dispense, LOW);
        }
        else
        {
            Serial.print("Valor inesperado: ");
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

    Serial.println("Enviando registro a Firestore...");

    if (Firebase.Firestore.createDocument(&fbdoFirestore, project_id.c_str(), "", collectionName, content.raw()))
    {
        Serial.println("Historial de dispensado registrado en Firestore.");
    }
    else
    {
        Serial.print("Error al registrar historial en Firestore: ");
        Serial.println(fbdoFirestore.errorReason());
    }
}
// Callback cuando hay timeout en el stream
void streamTimeoutCallback(bool timeout)
{
    String reception = DEVICE_ID;
    reception += "/commands/command_reception";

    if (timeout)
        Serial.println("Stream timeout, reconectando...");
    Firebase.RTDB.setString(&fbdo, reception.c_str(), "timeout_error");
    if (!stream.httpConnected())
        Serial.printf("Error de stream: %s\n", stream.errorReason().c_str());
    Firebase.RTDB.setString(&fbdo, reception.c_str(), "connection_error");

    reconexionFirebase();
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
    String pathFood = basePath;
    pathFood += "/food_weight";

    int rssi = WiFi.RSSI();                                // señal WiFi
    unsigned long uptime = millis() / 1000;                // tiempo de actividad en segundos
    float peso = loadCell.readWeight();                    // peso actual del alimento en gramos
    float chipTempC = (temprature_sens_read() - 32) / 1.8; // temperatura del chip en °C

    if (Firebase.RTDB.setInt(&fbdo, pathRssi.c_str(), rssi) &&
        Firebase.RTDB.setFloat(&fbdo, pathTemp.c_str(), chipTempC) &&
        Firebase.RTDB.setInt(&fbdo, pathUptime.c_str(), uptime) &&
        Firebase.RTDB.setString(&fbdo, pathOnline.c_str(), "conectado") &&
        Firebase.RTDB.setInt(&fbdo, pathLastSeen.c_str(), time(nullptr)) &&
        Firebase.RTDB.setFloat(&fbdo, pathFood.c_str(), peso))

    {
        Serial.print("Estado actualizado: ");
    }
    else
    {
        Serial.print("Error reportando estado: ");
        Serial.println(fbdo.errorReason());
        if (fbdo.errorReason().indexOf("timed out") != -1 ||
            fbdo.errorReason().indexOf("connection refused") != -1 ||
            fbdo.errorReason().indexOf("SSL") != -1)
        {
            reconexionFirebase();
        }
    }
}