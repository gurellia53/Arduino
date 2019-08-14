#include "desk_control.h"

// #include "d1_wifi.h"
// #include "d1_mqtt_rgbw.h"
// #include "d1_PCA9685.h"

// d1_wifi app_wifi;
// d1_mqtt_rgbw app_mqtt;
// d1_PCA9685 app_PCA9685;
#include <SunFounder_PCA9685.h>
PCA9685 pwm1;

//******************************************************************************
void setup()
{
    Serial.begin( 115200 );
    Serial.println( "." );
    Serial.println( "application start" );
    pwm1.begin();
    // app_wifi.init();
    // app_mqtt.init();
}

//******************************************************************************
void loop()
{
    // app_mqtt.update();

    // app_PCA9685.SetPwm( 12, app_mqtt.GetRed() );
    // app_PCA9685.SetPwm( 13, app_mqtt.GetGreen() );
    // app_PCA9685.SetPwm( 14, app_mqtt.GetBlue() );
    while(1)
    {
        // app_PCA9685.SetPwm( 15, 100 );
        pwm1.pwmWrite(15, 100);
        delay(500);
        // app_PCA9685.SetPwm( 14, 200 );
        pwm1.pwmWrite(15, 2000);
        delay(500);
    }
    // app_PCA9685.SetPwm( 14, 100 );
    // app_PCA9685.SetPwm( 13, 100 );
    // app_PCA9685.SetPwm( 12, 100 );
    // app_PCA9685.SetPwm( 11, 100 );
    // app_PCA9685.SetPwm( 10, 100 );
    // app_PCA9685.SetPwm( 9, 100 );
    // app_PCA9685.SetPwm( 8, 100 );
    // app_PCA9685.SetPwm( 7, 100 );
    // app_PCA9685.SetPwm( 6, 100 );
    // app_PCA9685.SetPwm( 5, 100 );
    // app_PCA9685.SetPwm( 4, 100 );
    // app_PCA9685.SetPwm( 3, 100 );
    // app_PCA9685.SetPwm( 2, 100 );
    // app_PCA9685.SetPwm( 1, 100 );
    // app_PCA9685.SetPwm( 0, 100 );
    // app_PCA9685.SetPwm( 15, app_mqtt.GetWhite() );

    // if ( val > 4095 )
    //     val=0;

    // app_PCA9685.SetPwm( 15, val );
    // app_PCA9685.SetPwm( 14, val );
    // val = val + 100;
    // delay(100);
}
