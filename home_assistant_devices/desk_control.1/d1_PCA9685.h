#include <SunFounder_PCA9685.h>

class d1_PCA9685
{
public:
    d1_PCA9685();
    void update();
    void SetPwm( uint8 channel, uint16 value );

private:
    PCA9685 pwm1;
};
