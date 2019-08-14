//
// https://github.com/corbanmailloux/esp-mqtt-rgb-led/blob/master/mqtt_esp8266_light/mqtt_esp8266_light.ino
//
#include "d1_mqtt.h"



WiFiClient wclient;
IPAddress MQTT_server(MQTT_SUBNET_0, MQTT_SUBNET_1, MQTT_SUBNET_2, MQTT_SUBNET_3);
PubSubClient MQTT_client(wclient);

//******************************************************************************
d1_mqtt::d1_mqtt() : port(MQTT_PORT)
{
    // MQTT_client(wclient)
    // MQTT_server = l_server;
}

//******************************************************************************
void d1_mqtt::init()
{
    Serial.println( "mqtt initializing" );
    MQTT_client.setServer(MQTT_server, port);
    update();
}

//******************************************************************************
bool d1_mqtt::mqtt_ok_to_publish()
{
    bool retval = false;

    // if(WiFi.status() == WL_CONNECTED)
    // {
        if (!MQTT_client.connected())
        {
            if (MQTT_client.connect(MQTT_DEVICE_ID))
            {
                Serial.println("MQTT Connected");
                MQTT_client.subscribe(CONFIG_MQTT_TOPIC_SET);
            }
        }
        if(MQTT_client.connected())
        {
            retval = true;
        }
    // }

    return retval;
}

//******************************************************************************
void d1_mqtt::update()
{
    // reset the request flag
    // mqtt_update_request = false;

    char tempStr[10] = {0};
    char humidStr[10] = {0};
    char doorStr[10] = {0};

    // convert to char*
    // if(UNKNOWN_32 == Temp_degF)
    // {
    //     tempStr[0] = '-';
    //     tempStr[1] = 0x00;
    // }
    // else
    // {
    //     dtostrf(Temp_degF, 3, 2, tempStr);
    // }
    
    // // convert to char*
    // if(UNKNOWN_32 == Humidity_pct)
    // {
    //     humidStr[0] = '-';
    //     humidStr[1] = 0x00;
    // }
    // else
    // {
    //     dtostrf(Humidity_pct, 3, 2, humidStr);
    // }
    
    // // convert to char*
    // if(DOOR_OPEN == Door_open)
    //     strcpy(doorStr, "open");
    // else if(DOOR_CLOSED == Door_open)
    //     strcpy(doorStr, "closed");
    // else if(DOOR_FAULTED == Door_open)
    //     strcpy(doorStr, "fault");
    
    // MQTT publish
    if ( mqtt_ok_to_publish() )
    {
        // Serial.println("MQTT Publishing");
        // MQTT_client.publish("GARAGE/Temp_F", tempStr);
        // MQTT_client.publish("GARAGE/Humidity_Pct", humidStr);
        // MQTT_client.publish("GARAGE/Laundry_Door", doorStr);
        MQTT_client.loop();
    }
    else
    {
        Serial.println("Error: MQTT Publishing not allowed.");
    }
}


//******************************************************************************
// void d1_mqtt::callback(char* topic, byte* payload, unsigned int length)
// {
//     Serial.print("Message arrived [");
//     Serial.print(topic);
//     Serial.print("] ");

//     char message[length + 1];
//     for (int i = 0; i < length; i++)
//     {
//         message[i] = (char)payload[i];
//     }
//     message[length] = '\0';
//     Serial.println(message);

//     // process_message( message );
// }

//******************************************************************************
// bool d1_mqtt::process_message(char* message)
// {
//     Serial.println("Warning: Ignoring received message.");
// }
