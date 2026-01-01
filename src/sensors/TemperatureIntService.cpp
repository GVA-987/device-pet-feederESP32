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
    uint8_t temp_f = temprature_sens_read();
    float temp_c = (temp_f - 32) / 1.8;
    return temp_c;
}