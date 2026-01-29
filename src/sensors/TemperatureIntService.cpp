#include "sensors/TemperatureIntService.h"

#ifdef __cplusplus
extern "C"
{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

float getInternalTemperature()
{
    float total = 0;
    for(int i = 0; i < 10; i++) {
        total += temprature_sens_read();
        delay(2); 
    }
    float avg_f = total / 10.0;
    float temp_c = (avg_f - 32) / 1.8;

    return temp_c;
}