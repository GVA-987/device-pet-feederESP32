#include <Firebase_ESP_Client.h>
#include "firebase_service.h"
#include "firebase_cred.h"
// #include <Firebase_ESP_Client.h>

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData fbdo;

#define LED_BUILTIN 2

void inicializarFirebase()
{
    Serial.println("Conectando a Firebase...");
    config.api_key = API_KEY;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    Firebase.begin(&config, &auth);

    pinMode(LED_BUILTIN, OUTPUT);

    // reconecta automaticamente al wifi si se corta
    Firebase.reconnectWiFi(true);
}
void escucharFirebase()
{

    String documentPath;
    documentPath = "devicesPet/";
    documentPath += DEVICE_ID;

    // Leer el documento de Firestore
    if (Firebase.Firestore.getDocument(&fbdo, "automatedpetfeedingsystem", "(default)", documentPath.c_str(), "")) // documento espesifico, "" vacio traera todo los campos
    {
        // Serial.println("Datos recibido:");
        // Serial.println(fbdo.payload()); // Muestra el JSON que devuelve firestore

        // Carga el JSON en un objeto para manipularlo
        FirebaseJson payload;
        payload.setJsonData(fbdo.payload());
        // Serial.println("Datos procesados:");
        // Serial.println(payload.raw()); // Muestra el JSON procesado

        // extrae un campo en especifico del JSON
        FirebaseJsonData data;
        if (payload.get(data, "fields/dispense_manual/stringValue"))
        {
            if (data.type == "string" && data.stringValue == "Activado")
            {
                // Serial.println("¡Comando de dispensar recibido!");
                Serial.println("DISPENSAR: TRUE (activar motor)");
                digitalWrite(LED_BUILTIN, HIGH);
                // delay(2000);

                // // Resetear el campo a false
                // String content = "{\"fields\":{\"dispensar\":{\"booleanValue\":false}}}";
                // if (Firebase.Firestore.patchDocument(&fbdo,
                //                                      "TU_PROJECT_ID",
                //                                      "(default)",
                //                                      documentPath.c_str(),
                //                                      content,
                //                                      "dispensar"))
                // {
                //     Serial.println("Comando de dispensar reseteado.");
                // }
                // else
                // {
                //     Serial.print("Error al resetear: ");
                //     Serial.println(fbdo.errorReason());
                // }
            }
            else if (data.type == "string" && data.stringValue == "Desactivado")
            {
                Serial.println("DISPENSAR: FALSE (no activar motor)");
                digitalWrite(LED_BUILTIN, LOW);
                // delay(2000);
            }
        }
        else
        {
            Serial.println("El campo 'dispense_manual' no existe en Firestore.");
        }
    }
    else
    {
        Serial.print("Error leyendo documento: ");
        Serial.println(fbdo.errorReason());
    }
}