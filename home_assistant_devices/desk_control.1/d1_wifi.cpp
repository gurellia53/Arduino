#include "d1_wifi.h"

//******************************************************************************
d1_wifi::d1_wifi()
{
}

//******************************************************************************
void d1_wifi::init()
{
    Serial.println( "wifi initializing" );
    WiFi.begin( ssid, password );
    WiFi.softAPdisconnect( true ); //Disable the node as an access point
    uint8_t led_state = HIGH;
    digitalWrite( WIFI_LED, led_state );
    
    // Wait for connection
    while ( WiFi.status() != WL_CONNECTED )
    {
        delay(50);
        Serial.print( "." );
        led_state = ( led_state == HIGH ) ? LOW : HIGH;
        digitalWrite( WIFI_LED, led_state );
    }
    Serial.println( ":" );

    led_state = HIGH;
    digitalWrite( WIFI_LED, led_state );
    Serial.println( "wifi connected" );
}
