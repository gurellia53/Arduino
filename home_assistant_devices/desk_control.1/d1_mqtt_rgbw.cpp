#include "d1_mqtt_rgbw.h"
#include "desk_control.h"
#include <ArduinoJson.h>

#define CONFIG_STRIP RGBW // Choose one of the options from above.

namespace
{
    const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);
    const bool rgb = (CONFIG_STRIP == RGB) || (CONFIG_STRIP == RGBW);
    const bool includeWhite = (CONFIG_STRIP == BRIGHTNESS) || (CONFIG_STRIP == RGBW);
    d1_mqtt_rgbw* my_d1_mqtt_rgbw = NULL;
    // {red, grn, blu, wht}
    const byte colors[][4] =
    {
        {255, 0, 0, 0},
        {0, 255, 0, 0},
        {0, 0, 255, 0},
        {255, 80, 0, 0},
        {163, 0, 255, 0},
        {0, 255, 255, 0},
        {255, 255, 0, 0}
    };

}

//******************************************************************************
d1_mqtt_rgbw::d1_mqtt_rgbw() : 
    red(255), green(255), blue(255), white(255), brightness(255), stateOn(false),
    startFade(false), lastLoop(0), transitionTime(0), inFade(false), loopCount(0),
    stepR(0), stepG(0), stepB(0), stepW(0), redVal(0), grnVal(0), bluVal(0), whtVal(0),
    flash(false), startFlash(false), flashLength(0), flashStartTime(0), 
    flashRed(255), flashGreen(255), flashBlue(255), flashWhite(255), flashBrightness(255),
    colorfade(false),currentColor(0), numColors(7), realRed(0), realGreen(0), realBlue(0),
    realWhite(0), o_Red(0), o_Green(0), o_Blue(0), o_White(255)
{
}

//******************************************************************************
void d1_mqtt_rgbw::init()
{
    Serial.println( "mqtt rgbw initializing" );
    IPAddress l_MQTT_server(MQTT_SUBNET_0, MQTT_SUBNET_1, MQTT_SUBNET_2, MQTT_SUBNET_3);
    MQTT_client.setServer(l_MQTT_server, MQTT_PORT);
    MQTT_client.setCallback(callback);

    my_d1_mqtt_rgbw = this;
}

//******************************************************************************
void d1_mqtt_rgbw::callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    char message[length + 1];
    for (int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    Serial.println(message);

    my_d1_mqtt_rgbw->process_message( message );
}

//******************************************************************************
void d1_mqtt_rgbw::process_message( char* message )
{
    Serial.println("Processing JSON message.");
    if (!processJson(message))
    {
        return;
    }

    if (stateOn)
    {
        // Update lights
        realRed = map(red, 0, 255, 0, brightness);
        realGreen = map(green, 0, 255, 0, brightness);
        realBlue = map(blue, 0, 255, 0, brightness);
        realWhite = map(white, 0, 255, 0, brightness);
    }
    else
    {
        realRed = 0;
        realGreen = 0;
        realBlue = 0;
        realWhite = 0;
    }

    startFade = true;
    inFade = false; // Kill the current fade

    sendState();
}


//******************************************************************************
bool d1_mqtt_rgbw::processJson( char* message )
{
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(message);

    if ( !root.success() )
    {
        Serial.println("parseObject() failed");
        return false;
    }
    else
    {
        Serial.println( "processing json" );
    }
    

    if (root.containsKey("state"))
    {
        if (strcmp(root["state"], CONFIG_MQTT_PAYLOAD_ON) == 0)
        {
            stateOn = true;
        }
        else if ( strcmp(root["state"], CONFIG_MQTT_PAYLOAD_OFF) == 0 )
        {
            stateOn = false;
        }
    }

    // If "flash" is included, treat RGB and brightness differently
    if (root.containsKey("flash") ||
       (root.containsKey("effect") && strcmp(root["effect"], "flash") == 0))
    {

        if (root.containsKey("flash"))
        {
            flashLength = (int)root["flash"] * 1000;
        }
        else
        {
            flashLength = CONFIG_DEFAULT_FLASH_LENGTH * 1000;
        }

        if (root.containsKey("brightness"))
        {
            flashBrightness = root["brightness"];
        }
        else
        {
            flashBrightness = brightness;
        }

        if (rgb && root.containsKey("color"))
        {
            flashRed = root["color"]["r"];
            flashGreen = root["color"]["g"];
            flashBlue = root["color"]["b"];
        }
        else
        {
            flashRed = red;
            flashGreen = green;
            flashBlue = blue;
        }

        if ( includeWhite && root.containsKey("white_value") )
        {
            flashWhite = root["white_value"];
        }
        else
        {
            flashWhite = white;
        }

        flashRed = map(flashRed, 0, 255, 0, flashBrightness);
        flashGreen = map(flashGreen, 0, 255, 0, flashBrightness);
        flashBlue = map(flashBlue, 0, 255, 0, flashBrightness);
        flashWhite = map(flashWhite, 0, 255, 0, flashBrightness);

        flash = true;
        startFlash = true;
    }
    else if (rgb && root.containsKey("effect") &&
        (strcmp(root["effect"], "colorfade_slow") == 0 || strcmp(root["effect"], "colorfade_fast") == 0))
    {
        flash = false;
        colorfade = true;
        currentColor = 0;
        if (strcmp(root["effect"], "colorfade_slow") == 0)
        {
            transitionTime = CONFIG_COLORFADE_TIME_SLOW;
        }
        else
        {
            transitionTime = CONFIG_COLORFADE_TIME_FAST;
        }
    }
    else if (colorfade && !root.containsKey("color") && root.containsKey("brightness"))
    {
        // Adjust brightness during colorfade
        // (will be applied when fading to the next color)
        brightness = root["brightness"];
    }
    else
    {
        // No effect
        flash = false;
        colorfade = false;
        Serial.println( "json no effect" );

        if (rgb && root.containsKey("color"))
        {
            Serial.println( "color" );
            red = root["color"]["r"];
            green = root["color"]["g"];
            blue = root["color"]["b"];
        }

        if (includeWhite && root.containsKey("white_value"))
        {
            white = root["white_value"];
        }

        if (root.containsKey("brightness"))
        {
            brightness = root["brightness"];
        }

        if (root.containsKey("transition"))
        {
            transitionTime = root["transition"];
        }
        else
        {
            transitionTime = CONFIG_DEFAULT_TRANSITION_TIME;
        }
    }

    return true;
}

//******************************************************************************
void d1_mqtt_rgbw::sendState()
{
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();

    root["state"] = (stateOn) ? CONFIG_MQTT_PAYLOAD_ON : CONFIG_MQTT_PAYLOAD_OFF;
    if (rgb) 
    {
        JsonObject& color = root.createNestedObject("color");
        color["r"] = red;
        color["g"] = green;
        color["b"] = blue;
    }

    root["brightness"] = brightness;

    if (includeWhite)
    {
        root["white_value"] = white;
    }

    if (rgb && colorfade)
    {
        if (transitionTime == CONFIG_COLORFADE_TIME_SLOW)
        {
            root["effect"] = "colorfade_slow";
        }
        else
        {
            root["effect"] = "colorfade_fast";
        }
    }
    else
    {
        root["effect"] = "null";
    }

    char buffer[root.measureLength() + 1];
    root.printTo(buffer, sizeof(buffer));

    MQTT_client.publish(CONFIG_MQTT_TOPIC_STATE, buffer, true);
}



//******************************************************************************
void d1_mqtt_rgbw::update()
{
    MQTT_client.subscribe(CONFIG_MQTT_TOPIC_SET);

    // MQTT publish
    if ( mqtt_ok_to_publish() )
    {
        // Serial.println("MQTT Publishing");
        
        char tempStr[10] = {0};
        MQTT_client.publish("GARAGE/Temp_F", "tempStr");
        // MQTT_client.publish("GARAGE/Humidity_Pct", humidStr);
        // MQTT_client.publish("GARAGE/Laundry_Door", doorStr);
        MQTT_client.loop();
    }
    else
    {
        Serial.println("Error: MQTT Publishing not allowed.");
    }

    if (flash)
    {
        if (startFlash)
        {
            startFlash = false;
            flashStartTime = millis();
        }

        if ((millis() - flashStartTime) <= flashLength)
        {
            if ((millis() - flashStartTime) % 1000 <= 500)
            {
                setColor(flashRed, flashGreen, flashBlue, flashWhite);
            }
            else
            {
                setColor(0, 0, 0, 0);
                // If you'd prefer the flashing to happen "on top of"
                // the current color, uncomment the next line.
                // setColor(realRed, realGreen, realBlue, realWhite);
            }
        }
        else
        {
            flash = false;
            setColor(realRed, realGreen, realBlue, realWhite);
        }
    }
    else if (rgb && colorfade && !inFade)
    {
        realRed = map(colors[currentColor][0], 0, 255, 0, brightness);
        realGreen = map(colors[currentColor][1], 0, 255, 0, brightness);
        realBlue = map(colors[currentColor][2], 0, 255, 0, brightness);
        realWhite = map(colors[currentColor][3], 0, 255, 0, brightness);
        currentColor = (currentColor + 1) % numColors;
        startFade = true;
    }

    if (startFade)
    {
        // If we don't want to fade, skip it.
        if (transitionTime == 0)
        {
            setColor(realRed, realGreen, realBlue, realWhite);

            redVal = realRed;
            grnVal = realGreen;
            bluVal = realBlue;
            whtVal = realWhite;

            startFade = false;
        }
        else
        {
            loopCount = 0;
            stepR = calculateStep(redVal, realRed);
            stepG = calculateStep(grnVal, realGreen);
            stepB = calculateStep(bluVal, realBlue);
            stepW = calculateStep(whtVal, realWhite);

            inFade = true;
        }
    }

    if (inFade)
    {
        startFade = false;
        unsigned long now = millis();
        if (now - lastLoop > transitionTime)
        {
            if (loopCount <= 1020)
            {
                lastLoop = now;

                redVal = calculateVal(stepR, redVal, loopCount);
                grnVal = calculateVal(stepG, grnVal, loopCount);
                bluVal = calculateVal(stepB, bluVal, loopCount);
                whtVal = calculateVal(stepW, whtVal, loopCount);

                setColor(redVal, grnVal, bluVal, whtVal); // Write current values to LED pins

                Serial.print("Loop count: ");
                Serial.println(loopCount);
                loopCount++;
            }
            else
            {
                inFade = false;
            }
        }
    }
}

//******************************************************************************
void d1_mqtt_rgbw::setColor(int inR, int inG, int inB, int inW) {
    if (CONFIG_INVERT_LED_LOGIC)
    {
        inR = (255 - inR);
        inG = (255 - inG);
        inB = (255 - inB);
        inW = (255 - inW);
    }

    if (rgb)
    {
        o_Red = inR;
        o_Green = inG;
        o_Blue = inB;
    }

    if (includeWhite)
    {
        o_White = inW;
    }

    if (1) {
        Serial.print("Setting LEDs: {");
        if (rgb) {
        Serial.print("r: ");
        Serial.print(o_Red);
        Serial.print(" , g: ");
        Serial.print(o_Green);
        Serial.print(" , b: ");
        Serial.print(o_Blue);
        }

        if (includeWhite) {
        if (rgb) {
            Serial.print(", ");
        }
        Serial.print("w: ");
        Serial.print(o_White);
        }

        Serial.println("}");
    }
}


// From https://www.arduino.cc/en/Tutorial/ColorCrossfader
/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
*
* The program works like this:
* Imagine a crossfade that moves the red LED from 0-10,
*   the green from 0-5, and the blue from 10 to 7, in
*   ten steps.
*   We'd want to count the 10 steps and increase or
*   decrease color values in evenly stepped increments.
*   Imagine a + indicates raising a value by 1, and a -
*   equals lowering it. Our 10 step fade would look like:
*
*   1 2 3 4 5 6 7 8 9 10
* R + + + + + + + + + +
* G   +   +   +   +   +
* B     -     -     -
*
* The red rises from 0 to 10 in ten steps, the green from
* 0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
*
* In the real program, the color percentages are converted to
* 0-255 values, and there are 1020 steps (255*4).
*
* To figure out how big a step there should be between one up- or
* down-tick of one of the LED values, we call calculateStep(),
* which calculates the absolute gap between the start and end values,
* and then divides that gap by 1020 to determine the size of the step
* between adjustments in the value.
*/
int d1_mqtt_rgbw::calculateStep(int prevValue, int endValue)
{
    int step = endValue - prevValue; // What's the overall gap?
    if (step)
    {                      // If its non-zero,
        step = 1020/step;            //   divide by 1020
    }

    return step;
}


/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1.
*  (R, G, and B are each calculated separately.)
*/
int d1_mqtt_rgbw::calculateVal(int step, int val, int i)
{
    if ((step) && i % step == 0)
    { // If step is non-zero and its time to change a value,
        if (step > 0)
        {              //   increment the value if step is positive...
            val += 1;
        }
        else if (step < 0)
        {         //   ...or decrement it if step is negative
            val -= 1;
        }
    }

    // Defensive driving: make sure val stays in the range 0-255
    if (val > 255)
    {
        val = 255;
    }
    else if (val < 0)
    {
        val = 0;
    }

    return val;
}
