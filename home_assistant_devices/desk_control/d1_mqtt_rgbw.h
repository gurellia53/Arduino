#pragma once

#include "d1_mqtt.h"

#define CONFIG_MQTT_PAYLOAD_ON "ON"
#define CONFIG_MQTT_PAYLOAD_OFF "OFF"

#define CONFIG_DEFAULT_TRANSITION_TIME 1

// Default number of flashes if no value was given
#define CONFIG_DEFAULT_FLASH_LENGTH 2
// Number of seconds for one transition in colorfade mode
#define CONFIG_COLORFADE_TIME_SLOW 10
#define CONFIG_COLORFADE_TIME_FAST 3

// Leave this here. These are the choices for CONFIG_STRIP below.
enum strip
{
    BRIGHTNESS, // only one color/only white
    RGB,        // RGB LEDs
    RGBW        // RGB LEDs with an extra white LED per LED
};

//******************************************************************************
class d1_mqtt_rgbw : public d1_mqtt
{
public:
    d1_mqtt_rgbw();
    void init();
    void update();

    static void callback(char* topic, byte* payload, unsigned int length);

    uint16 GetRed()
    {
        return o_Red;
    }
    uint16 GetGreen()
    {
        return o_Green;
    }
    uint16 GetBlue()
    {
        return o_Blue;
    }
    uint16 GetWhite()
    {
        return o_White;
    }

    void process_message( char* message );
private:
    bool processJson( char* message );
    void sendState();
    void setColor(int inR, int inG, int inB, int inW);
    int calculateStep(int prevValue, int endValue);
    int calculateVal(int step, int val, int i);

    // Maintained state for reporting to HA
    byte red = 255;
    byte green = 255;
    byte blue = 255;
    byte white = 255;
    byte brightness = 255;

    bool stateOn = false;

    // Globals for fade/transitions
    bool startFade = false;
    unsigned long lastLoop = 0;
    int transitionTime = 0;
    bool inFade = false;
    int loopCount = 0;
    int stepR, stepG, stepB, stepW;
    int redVal, grnVal, bluVal, whtVal;

    // Globals for flash
    bool flash = false;
    bool startFlash = false;
    int flashLength = 0;
    unsigned long flashStartTime = 0;
    byte flashRed = red;
    byte flashGreen = green;
    byte flashBlue = blue;
    byte flashWhite = white;
    byte flashBrightness = brightness;

    // Globals for colorfade
    bool colorfade = false;
    int currentColor = 0;

    const int numColors = 7;

    // Real values to write to the LEDs (ex. including brightness and state)
    byte realRed = 0;
    byte realGreen = 0;
    byte realBlue = 0;
    byte realWhite = 0;

    // output colors
    uint16 o_Red;
    uint16 o_Green;
    uint16 o_Blue;
    uint16 o_White;
};
