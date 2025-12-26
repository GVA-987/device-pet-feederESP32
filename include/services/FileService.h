#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <Arduino.h>

void setupFiles();

bool saveFile(const char *path, const char *content);

String readFile(const char *path);

#endif