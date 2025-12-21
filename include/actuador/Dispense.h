#ifndef DISPENSER_H
#define DISPENSER_H

#include <arduino.h>
#include <PubSubClient.h>

void setupDispenser();
void executeManualDispense(int portion, PubSubClient &client);

#endif