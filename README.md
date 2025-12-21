# 🐾 ESP32 Smart Pet Feeder (MQTT Edition)

Este repositorio contiene el firmware para un alimentador de mascotas inteligente basado en el microcontrolador **ESP32**. El sistema permite la dispensación remota de alimento y el monitoreo de peso en tiempo real.

## Nueva Arquitectura de Comunicación

A diferencia de versiones anteriores, este dispositivo ya no se comunica directamente con Firebase. Ahora utiliza una arquitectura más robusta y escalable basada en eventos:

1.  **ESP32**: Actúa como cliente MQTT, publica el peso y se suscribe a comandos.
2.  **Broker MQTT (HiveMQ)**: Gestiona el paso de mensajes en tiempo real.
3.  **Backend (Node.js)**: Actúa como puente (Bridge) entre MQTT y Firebase.
4.  **Firebase**: Almacena el historial y gestiona la autenticación.

---

## Funcionalidades Implementadas

- **Protocolo MQTT**: Comunicación bidireccional de baja latencia.
- **Control de Servomotor**: Lógica de apertura y cierre para la ración de alimento.
- **Lectura de Celda de Carga (HX711)**: Monitoreo preciso del inventario de alimento.
- **Seguridad**: Credenciales separadas del código fuente.

---

## Configuración e Instalación

### 1. Requisitos de Hardware

- ESP32 DevKit V1
- Sensor de Peso (Celda de carga + HX711)
- Servomotor
- Fuente de alimentación de 5V/2A

### 2. Configuración del Firmware

Debido a razones de seguridad, el archivo de configuración ha sido omitido. Debes crearlo manualmente en la siguiente ruta: `src/config/Config.cpp`.

**Estructura del archivo `Config.cpp`:**

```cpp
#include "Config.h"

// Credenciales de Red
String WIFI_SSID = "TU_WIFI_NOMBRE";
String WIFI_PASS = "TU_PASSWORD";

// Configuración del Broker (HiveMQ)
const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

// Identificación y Tópicos
String DEVICE_ID = "TU-ID-UNICO-EQUIPO";
String TOPIC_STATUS = "petfeeder/status/" + DEVICE_ID;
String TOPIC_COMMAND = "petfeeder/command/" + DEVICE_ID;
```
