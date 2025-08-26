#include <Firebase_ESP_Client.h>
#include "firebase_service.h"
#include "firebase_cred.h"
// #include <Firebase_ESP_Client.h>

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData fbdo; // objeto para enviar/recibir datos de Firebase

#define LED_BUILTIN 2

void inicializarFirebase()
{
    Serial.println("Conectando a Firebase...");
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.project_id = FIREBASE_PROJECT_ID;

    Firebase.begin(&config, &auth);

    pinMode(LED_BUILTIN, OUTPUT);

    // reconecta automaticamente al wifi si se corta
    Firebase.reconnectWiFi(true);
}
void escucharFirebase()
{
    // Ruta del documento en Firestore
    String documentPath;
    documentPath = "devicesPet/";
    documentPath += String(DEVICE_ID);

    // Leer el documento de Firestore
    if (Firebase.Firestore.getDocument(&fbdo,
                                       FIREBASE_PROJECT_ID, // Reemplaza con tu projectId real
                                       "(default)",         // databaseId casi siempre "(default)"
                                       documentPath.c_str(),
                                       "")) // documento espesifico, "" vacio traera todo los campos
    {
        Serial.println("Datos recibido:");
        Serial.println(fbdo.payload()); // Muestra el JSON que devuelve firestore

        // Carga el JSON en un objeto para manipularlo
        FirebaseJson payload;
        payload.setJsonData(fbdo.payload());

        // extrae un campo en especifico del JSON
        FirebaseJsonData data;
        if (payload.get(data, "dispense_manual"))
        {
            if (data.boolValue)
            {
                Serial.println("¡Comando de dispensar recibido!");
                Serial.println("➡ DISPENSAR: TRUE (activar motor)");
                digitalWrite(LED_BUILTIN, HIGH);

                // // Resetear el campo a false
                // String content = "{\"fields\":{\"dispensar\":{\"booleanValue\":false}}}";
                // if (Firebase.Firestore.patchDocument(&fbdo,
                //                                      "TU_PROJECT_ID",
                //                                      "(default)",
                //                                      documentPath.c_str(),
                //                                      content,
                //                                      "dispensar")) // updateMask
                // {
                //     Serial.println("Comando de dispensar reseteado.");
                // }
                // else
                // {
                //     Serial.print("Error al resetear: ");
                //     Serial.println(fbdo.errorReason());
                // }
            }
            else
            {
                Serial.println("➡ DISPENSAR: FALSE (no activar motor)");
                digitalWrite(LED_BUILTIN, LOW);
            }
        }
        else
        {
            Serial.println("⚠ El campo 'dispensar' no existe en Firestore.");
        }
    }
    else
    {
        Serial.print("❌ Error leyendo documento: ");
        Serial.println(fbdo.errorReason());
    }
}