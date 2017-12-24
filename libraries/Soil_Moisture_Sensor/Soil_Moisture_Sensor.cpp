// Arduino Soil_Moisture_Sensor library

#include "Soil_Moisture_Sensor.h"

Soil_Moisture_Sensor::Soil_Moisture_Sensor(uint8_t pin, uint8_t sensor_mode)
{
    _pin = pin;
    _sensor_mode = sensor_mode;
}

// init
void Soil_Moisture_Sensor::begin(void)
{
    // set up the pin
    pinMode(_pin, INPUT);
}

// periodic readMoisture
int Soil_Moisture_Sensor::readMoisture(void)
{
    int sensor_val = -1;
    if(SENSOR_MODE_ANALOG == _sensor_mode)
    {
        sensor_val = analogRead(_pin);
    }
    else if (SENSOR_MODE_DIGITAL == _sensor_mode)
    {
        sensor_val = digitalRead(_pin);
    }
    
    _last_sensor_val = sensor_val;
    
    return sensor_val;
}

// convert reading to percent
float Soil_Moisture_Sensor::convertPercent(int sensor_val)
{
    float pct = -1;
    
    if(SENSOR_MODE_ANALOG == _sensor_mode)
    {
        pct = (float)(_max_sensor_val - sensor_val)/_max_sensor_val;
        _last_percent_val = pct;
    }
    
    return pct;
}