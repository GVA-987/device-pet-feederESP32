#include "firebase_service.h"
#include "firebase_cred.h"
#include <Firebase_ESP_Client.h>

FirebaseConfig config;
FirebaseAuth auth;

void inicializarFirebase()
{
    Serial.println("Conectando a Firebase...");

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Conexión exitosa a Firebase.");
}

void escucharFirebase()
{
    // codigo para ecuchar a firebase
}