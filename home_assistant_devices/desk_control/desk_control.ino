#include "desk_control.h"

#include "d1_wifi.h"
#include "d1_mqtt_rgbw.h"
#include "d1_PCA9685.h"

d1_wifi app_wifi;
d1_mqtt_rgbw app_mqtt;
d1_PCA9685 app_PCA9685;

//******************************************************************************
void setup()
{
    Serial.begin( 115200 );
    Serial.println( "." );
    Serial.println( "application start" );
    app_wifi.init();
    app_mqtt.init();
}

//******************************************************************************
void loop()
{
    app_mqtt.update();

    app_PCA9685.SetPwm( 12, app_mqtt.GetRed() );
    app_PCA9685.SetPwm( 13, app_mqtt.GetGreen() );
    app_PCA9685.SetPwm( 14, app_mqtt.GetBlue() );
    app_PCA9685.SetPwm( 15, app_mqtt.GetWhite() );

    // if ( val > 4095 )
    //     val=0;

    // app_PCA9685.SetPwm( 15, val );
    // app_PCA9685.SetPwm( 14, val );
    // val = val + 100;
    // delay(100);
}
