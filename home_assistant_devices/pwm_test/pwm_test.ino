#include "desk_control.h"
#include <SunFounder_PCA9685.h>
PCA9685 pwm_test;

//******************************************************************************
void setup()
{
    pwm_test.begin();
}

//******************************************************************************
void loop()
{
        pwm_test.pwmWrite(15, 100);
        delay(500);
        pwm_test.pwmWrite(15, 2000);
        delay(500);
}
