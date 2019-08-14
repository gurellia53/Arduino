#pragma once

#include <string>
#include "PubSubClient.h"
#include <WiFiClient.h>

#include "desk_control.h"

#include "C:\Users\gurel\Documents\Arduino\network_config_private.h"


#define MQTT_DEVICE_ID "desk_glow"


extern WiFiClient wclient;
extern IPAddress MQTT_server;
extern PubSubClient MQTT_client;

//******************************************************************************
class d1_mqtt
{
public:
    d1_mqtt();
    virtual void init();
    virtual void update();

    // static void callback(char* topic, byte* payload, unsigned int length);

protected:
    bool mqtt_ok_to_publish();

    // PubSubClient MQTT_client;
    // IPAddress MQTT_server;
    const uint16_t port;

};
