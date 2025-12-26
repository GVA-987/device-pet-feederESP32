#include "../include/network/TimeService.h"
#include <time.h>

// El formato es: Offset en segundos ( -4 * 3600 = -14400 )
const long gmtOffset_sec = -14400;
const int daylightOffset_sec = 0;
const char *ntpServer = "pool.ntp.org";

void setupTime()
{
    Serial.println("Sincronizando hora con NTP...");

    // tiempo interno
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("❌ Error al obtener la hora");
        return;
    }
    Serial.println("Hora sincronizada correctamente.");
}

String getFormattedTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return "00:00:00";
    }
    char timeStringBuff[9]; // HH:MM:SS
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
    return String(timeStringBuff);
}

int getDayOfWeek()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return -1;
    }
    return timeinfo.tm_wday; // 0=Domingo, 1=Lunes...
}