# Dispositivo Alimentador de Mascotas Inteligente (ESP32)

## Descripción del Proyecto

Este repositorio contiene el firmware y la documentación para un alimentador de mascotas automatizado y conectado, utilizando un microcontrolador ESP32. El dispositivo permite monitorear el nivel de alimento y dispensar raciones de forma remota o programada, enviando los datos a una plataforma en la nube de Firebase.

1.  **Monitorear** el nivel de alimento restante mediante una celda de carga.
2.  **Dispensar** raciones de forma controlada utilizando un servomotor.
3.  **Sincronizar** todos los datos y el estado con Firebase.

## 🚀 Funcionalidades Implementadas

- **Conectividad Wi-Fi:** Uso del ESP32 para conexión a internet.
- **Lectura de Peso (Celda de Carga HX711):** Monitoreo del peso actual del alimento restante y envío de estos datos a Firebase.
- **Envío de Datos a Firebase:** Sincronización en tiempo real del peso y el estado del dispositivo.
- **Activación del Servomotor:** Lógica para la dispensación de alimento (actualmente implementada con un giro de 90 grados para pruebas iniciales).

## Hardware Necesario

Para construir y operar este proyecto, necesitarás los siguientes componentes:

- ESP32
- Celda de carga con módulo HX711
- Servomotor
- Fuente de alimentación

## 🔒 Configuración de Credenciales y Seguridad

**¡ATENCIÓN!** Las claves y credenciales de Firebase deben mantenerse privadas. El repositorio **NO** incluye estos datos sensibles. Sigue estos pasos para configurar tu conexión de forma segura:

### 1. Configuración Inicial de Firebase

1.  Crea un nuevo proyecto en la Consola de **Firebase**.
2.  Configura los servicios que vayas a utilizar: **Realtime Database** o **Firestore**, y **Authentication**.
3.  Obtén tu `API Key`, `Database URL`, y `Project ID` desde la configuración del proyecto.

### 2. Crear `include/firebase_cred.h`

Crea manualmente un archivo llamado `firebase_cred.h` dentro del directorio `include/`. Este archivo contendrá las claves necesarias para la conexión.

### 3. ¡Paso Crítico de Seguridad! Actualizar `.gitignore`

Para evitar que tus credenciales se suban accidentalmente al repositorio público, debes incluir la ruta del archivo de credenciales en tu `.gitignore` (ubicado en la raíz del proyecto)

**Asegúrate de reemplazar los valores de ejemplo con tus propias credenciales de Firebase:**

```c
#ifndef CREDENCIALES_H
#define CREDENCIALES_H

// Credenciales de la API de Firebase
#define API_KEY "TU_API_KEY_AQUI"
#define DATABASE_URL "TU_DATABASE_URL_AQUI"
#define PROJECT_ID "TU_PROJECT_ID_AQUI"

// Credenciales del usuario para autenticación (si se usa)
#define USER_EMAIL "TU_EMAIL_DE_PRUEBA_AQUI"
#define USER_PASSWORD "TU_PASSWORD_AQUI"

// Identificador único para este dispositivo
#define DEVICE_ID "ESP-PET-TU_ID_UNICO"

#endif

```
