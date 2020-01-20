
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

//*****************************************************************************
// pin definitions
#define KB_IN_1     D7
#define KB_IN_0     D6
#define KB_OUT_0    D5
#define KB_OUT_1    D0

#define D_OUT    D2
#define NEO_OUT  D3

//*****************************************************************************
// key state
static uint8 s_key[5] = { 0, 0, 0, 0, 0, };

//*****************************************************************************
// neopixel config
#define NUMPIXELS   127
Adafruit_NeoPixel pixels_0( NUMPIXELS, NEO_OUT, NEO_GRB + NEO_KHZ800 );

#define DELAYVAL 5
#define BRIGHTNESS_0 64
#define BRIGHTNESS_LOW 1
#define BRIGHTNESS_OFF 0

const uint32_t YELLOW = 0x30FF00; //0xB2FE00
const uint32_t WHITE = 0x30FFFF;
const int WIDTH = 12;

//*****************************************************************************
// state
static bool s_white_backlight = false;
// static bool s_white_underglow = false;
static bool s_neo = false;
static bool s_power = false;

//*****************************************************************************
// Neopixel control
void pixel_loop( int pixel, int loop )
{
    int width = 1;
    if ( loop > WIDTH )
    {
        width = WIDTH;
    }
    else
    {
        width = loop;
    }

    if ( (pixel < loop + width) && (pixel > loop - width) )
    {
        // moving color
        pixels_0.setPixelColor( pixel, WHITE );
    }
    else
    {
        // background
        pixels_0.setPixelColor( pixel, YELLOW );
    }
}


//*****************************************************************************
void update_output()
{
    // digital out
    digitalWrite( D_OUT, !( s_white_backlight && s_power ) );
    
    // neopixels
    uint16_t brightness = BRIGHTNESS_0;
    if ( !s_power )
    {
        brightness = BRIGHTNESS_OFF;
    }
    else if ( !s_neo )
    {
        brightness = BRIGHTNESS_LOW;
    }
    pixels_0.setBrightness( brightness );
    pixels_0.show();
}

//*****************************************************************************
void key_press( uint8 key )
{
    Serial.println( key );
    if ( key == 0 )
    {
        s_power = !s_power;
    }
    // if power is off, ignore other keys
    else if ( s_power )
    {
        switch ( key )
        {
        case 1:
            s_white_backlight = !s_white_backlight;
            break;
        case 2:
            s_neo = !s_neo;
            break;
        case 3:
            break;
        case 4:
            break;
        }
    }
    update_output();
}

//*****************************************************************************
void set_key( uint8 key, bool is_pressed )
{
    const uint8 KEY_DEBOUNCE = 6;

    if ( !is_pressed )
    {
        s_key[key] = 0;
        return;
    }

    if ( s_key[key] < KEY_DEBOUNCE )
    {
        s_key[key]++;
    }
    else if ( s_key[key] == KEY_DEBOUNCE )
    {
        key_press( key );
        s_key[key]++;
    }
}

//*****************************************************************************
void read_kb()
{
    const int DELAY = 10;
    int in[4];

    // DO 0
    digitalWrite( KB_OUT_1, HIGH );
    digitalWrite( KB_OUT_0, LOW );
    delayMicroseconds( DELAY );
    in[0] = !digitalRead( KB_IN_0 );
    in[1] = !digitalRead( KB_IN_1 );

    delayMicroseconds( DELAY );

    // DO 1
    digitalWrite( KB_OUT_0, HIGH );
    digitalWrite( KB_OUT_1, LOW );
    delayMicroseconds( DELAY );
    in[2] = !digitalRead( KB_IN_0 );
    in[3] = !digitalRead( KB_IN_1 );

    // set keys
    bool power_key = (boolean)in[2] && (boolean)in[0];
    set_key( 0, power_key );
    if ( !power_key )
    {
        set_key( 1, (boolean)in[2] );
        set_key( 2, (boolean)in[0] );
        set_key( 3, (boolean)in[1] );
        set_key( 4, (boolean)in[3] );
    }

    delayMicroseconds( DELAY );
    digitalWrite( KB_OUT_0, HIGH );
    digitalWrite( KB_OUT_1, HIGH );
}

//*****************************************************************************
void setup()
{
    Serial.begin(9600);
    // init inputs
    pinMode( KB_IN_0, INPUT_PULLUP );
    pinMode( KB_IN_1, INPUT_PULLUP );
    pinMode( KB_OUT_0, OUTPUT );
    pinMode( KB_OUT_1, OUTPUT );

    // init white led strip
    pinMode( D_OUT, OUTPUT );

    // init NeoPixel strip
    pixels_0.begin();
    pixels_0.clear();

    update_output();
}

//*****************************************************************************
void loop()
{
    read_kb();

    static int j = 0;
    
    // neopixel control
    if ( s_power )
    {
        // For each pixel_0
        for( int i=0; i < NUMPIXELS; i++ )
        {
            pixel_loop( i, j );
        }
        
        if ( DELAYVAL )
        {
            delay( DELAYVAL );
        }
        pixels_0.show();

        j++;
        if ( j > NUMPIXELS )
        {
            j = 0;
        }
    }
}
