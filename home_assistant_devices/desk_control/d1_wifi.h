#pragma once

#include <string>
// Wifi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include "C:\Users\gurel\Documents\Arduino\network_config_private.h"

#define WIFI_LED D4

//******************************************************************************
class d1_wifi
{
public:
    d1_wifi();
    void init();

private:
    const char* ssid = WIFI_SSID;
    const char* password = WIFI_PASSWORD;
};
