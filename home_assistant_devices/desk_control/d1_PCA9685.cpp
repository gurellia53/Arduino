#include "d1_PCA9685.h"

d1_PCA9685::d1_PCA9685()
{
    Serial.println("PCA9685 start.");
    pwm1 = PCA9685(0x40);
    pwm1.begin();
    pwm1.setFreq(400);
}

#define MAX_PWM_VALUE 4095

void d1_PCA9685::SetPwm( uint8 channel, uint16 value )
{
    if ( value > MAX_PWM_VALUE)
    {
        value = MAX_PWM_VALUE;
    }
    
    pwm1.pwmWrite( channel, value );
}

void d1_PCA9685::update()
{
    // pwm1.pwmWrite( channel, value );
}
