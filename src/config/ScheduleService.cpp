#include "services/ScheduleService.h"
#include "services/FileService.h"
#include "network/TimeService.h"
#include "actuador/Dispense.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>

extern PubSubClient client;
int lastMinuteChecked = -1;

void checkSchedule()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
        return;

    // revicion una vez por minuto
    if (timeinfo.tm_min == lastMinuteChecked)
    {
        return;
    }

    String json = readFile("/schedule.json");
    if (json == "" || json == "null")
    {
        return;
    }

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, json);
    JsonArray arr = doc.as<JsonArray>();

    char currentTime[6]; // "HH:MM"
    sprintf(currentTime, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    int today = timeinfo.tm_wday; // 0=Dom, 1=Lun...

    for (JsonObject obj : arr)
    {
        bool enabled = obj["enabled"] | false;
        const char *scheduleTime = obj["time"];
        JsonArray days = obj["days"];
        int portion = obj["portion"] | 1;

        if (enabled && String(scheduleTime) == String(currentTime))
        {
            // Verificacion si el día de hoy está en la lista de días permitidos
            bool dayMatch = false;
            for (int d : days)
            {
                if (d == today)
                {
                    dayMatch = true;
                    break;
                }
            }

            if (dayMatch)
            {
                lastMinuteChecked = timeinfo.tm_min;
                Serial.printf("¡HORARIO PROGRAMADO! Ejecutando programa: %s\n", scheduleTime);
                executeDispense(portion, client, "programado");
                return;
            }
        }
    }
}