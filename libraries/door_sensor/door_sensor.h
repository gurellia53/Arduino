/* door_sensor library

*/
#ifndef DOOR_SENSOR_H
#define DOOR_SENSOR_H

#include "Arduino.h"

#define DOOR_OPEN       0x00
#define DOOR_CLOSED     0x01
#define DOOR_FAULTED    0xff

class door_sensor
{
    public:
    door_sensor(uint8_t pin_0, uint8_t pin_1);
    void begin(void);
    char check_door();
    
    private:
    uint8_t _pin[2]; //[NO, NC]
    int _sensor_val[2];
    uint8_t _door_val;
    
};


#endif
