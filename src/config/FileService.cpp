#include "services/FileService.h"
#include <LittleFS.h>

void setupFiles()
{
    if (!LittleFS.begin(true))
    { // true formateará si falla el montaje inicial
        Serial.println("❌ Error al montar LittleFS");
        return;
    }
    Serial.println("Sistema de archivos listo.");
}

bool saveFile(const char *path, const char *content)
{
    File file = LittleFS.open(path, FILE_WRITE);
    if (!file)
        return false;
    bool success = file.print(content);
    file.close();
    return success;
}

String readFile(const char *path)
{
    if (!LittleFS.exists(path))
        return "";
    File file = LittleFS.open(path, FILE_READ);
    if (!file || file.isDirectory())
        return "";
    String content = file.readString();
    file.close();
    return content;
}