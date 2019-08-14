#include "d1_PCA9685.h"

d1_PCA9685::d1_PCA9685()
{
    Serial.println("PCA9685 start.");
    pwm1 = PCA9685(0x40);
    pwm1.begin();
    pwm1.setFreq(400);
}

void d1_PCA9685::SetPwm( uint8 channel, uint16 value )
{
    pwm1.pwmWrite( channel, value );
}

void d1_PCA9685::update()
{
    // pwm1.pwmWrite( channel, value );
}
