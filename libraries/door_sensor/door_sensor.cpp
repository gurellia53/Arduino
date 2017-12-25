// Door_Sensor library

#include "door_sensor.h"

door_sensor::door_sensor(uint8_t pin_0, uint8_t pin_1)
{
    _pin[0] = pin_0;
    _pin[1] = pin_1;
    
    // set up the pin
    pinMode(pin_0, INPUT_PULLUP);
    pinMode(pin_1, INPUT_PULLUP);
}

char door_sensor::check_door()
{
    // NO pin
    _sensor_val[0] = digitalRead(_pin[0]);
    // NC pin
    _sensor_val[1] = digitalRead(_pin[1]);
    
    if(_sensor_val[0] == _sensor_val[1])
    {
        _door_val = DOOR_FAULTED;
    }
    else if((HIGH == _sensor_val[0]) && (LOW == _sensor_val[1]))
    {
        _door_val = DOOR_OPEN;
    }
    else if((LOW == _sensor_val[0]) && (HIGH == _sensor_val[1]))
    {
        _door_val = DOOR_CLOSED;
    }
    
    return _door_val;
}

