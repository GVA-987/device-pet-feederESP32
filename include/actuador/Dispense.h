#ifndef DISPENSER_H
#define DISPENSER_H

#include <arduino.h>
#include <PubSubClient.h>

void setupDispenser();
void executeDispense(int gramsToDispense, int portion, PubSubClient &client, String type);

#endif