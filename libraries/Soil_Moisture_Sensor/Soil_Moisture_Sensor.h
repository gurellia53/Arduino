/* Soil_Moisture_Sensor library

*/
#ifndef Soil_Moisture_Sensor_H
#define Soil_Moisture_Sensor_H

#include "Arduino.h"

#define SENSOR_MODE_ANALOG      0
#define SENSOR_MODE_DIGITAL     255

class Soil_Moisture_Sensor
{
    public:
    Soil_Moisture_Sensor(uint8_t pin, uint8_t sensor_mode);
    void begin(void);
    int readMoisture(void);
    float convertPercent(int sensor_val);

    private:
    uint8_t _pin;
    uint8_t _sensor_mode;
    int _last_sensor_val;
    int _last_percent_val;
    int _max_sensor_val = 1023;

};


#endif
