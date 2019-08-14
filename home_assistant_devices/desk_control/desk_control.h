#pragma once
/*
 * This is a sample configuration file for the "mqtt_esp8266" light.
 *
 * Change the settings below and save the file as "config.h"
 * You can then upload the code using the Arduino IDE.
 */

#include "C:\Users\gurel\Documents\Arduino\network_config_private.h"



// Pins
// In case of BRIGHTNESS: only WHITE is used
// In case of RGB(W): red, green, blue(, white) is used
// All values need to be present, if they are not needed, set to -1,
// it will be ignored.
#define CONFIG_PIN_RED   0  // For RGB(W)
#define CONFIG_PIN_GREEN 2  // For RGB(W)
#define CONFIG_PIN_BLUE  3  // For RGB(W)
#define CONFIG_PIN_WHITE -1 // For BRIGHTNESS and RGBW

// WiFi
#define CONFIG_WIFI_SSID WIFI_SSID
#define CONFIG_WIFI_PASS WIFI_PASSWORD

// MQTT
// #define CONFIG_MQTT_HOST "{MQTT-SERVER}"
#define CONFIG_MQTT_PORT 1883 // Usually 1883
// #define CONFIG_MQTT_USER "{MQTT-USERNAME}"
// #define CONFIG_MQTT_PASS "{MQTT-PASSWORD}"
#define CONFIG_MQTT_CLIENT_ID "desk_glow" // Must be unique on the MQTT network


// MQTT Topics
#define CONFIG_MQTT_TOPIC_STATE "DESK/desk_glow"
#define CONFIG_MQTT_TOPIC_SET "DESK/desk_glow/set"


// Miscellaneous



// Reverse the LED logic
// false: 0 (off) - 255 (bright)
// true: 255 (off) - 0 (bright)
#define CONFIG_INVERT_LED_LOGIC false

// Set the mode for the built-in LED on some boards.
// -1 = Do nothing. Leave the pin in its default state.
//  0 = Explicitly set the LED_BUILTIN to LOW.
//  1 = Explicitly set the LED_BUILTIN to HIGH. (Off for Wemos D1 Mini)
#define CONFIG_LED_BUILTIN_MODE -1

// Enables Serial and print statements
#define CONFIG_DEBUG false