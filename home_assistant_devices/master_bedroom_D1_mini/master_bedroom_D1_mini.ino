
// Application for bedroom D1 mini
//  Contains:
//  - DHT11 temperature and humidity sensor
//  - Wifi
//  - HTTP web server
//  - MQTT

//******************************************************************************
//**** Includes ****************************************************************
//******************************************************************************
#define UNKNOWN_32 0xFFFFFFFF

// Wifi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include "G:\network_config\network_config_private.h"

//HTTP web server
#include <ESP8266WebServer.h>

// MQTT
#include "PubSubClient.h"

// DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//******************************************************************************
//**** App Parameters **********************************************************
//******************************************************************************

// DHT
#define DHTPIN D2    // DHT sensor pin
#define DHTTYPE DHT11 // DHT11 sensor
DHT_Unified dht(DHTPIN, DHTTYPE);

//Web server
ESP8266WebServer server(80);

// Wifi
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
#define WIFI_LED D4

// MQTT
IPAddress MQTT_server(MQTT_SUBNET_0, MQTT_SUBNET_1, MQTT_SUBNET_2, MQTT_SUBNET_3);
uint16_t port = MQTT_PORT;
WiFiClient wclient;
PubSubClient MQTT_client(wclient);
#define MQTT_DEVICE_ID "BEDROOM"

//******************************************************************************
//**** scheduler config* *******************************************************
//******************************************************************************
// Scheduler constants
unsigned long Loop_start_time_ms;
unsigned long Prev_loop_start_time_ms;

#define TASK_RATE_100_MS 100
unsigned long prev_run_time_100_ms;

#define TASK_RATE_1000_MS 1000
unsigned long prev_run_time_1000_ms;

#define TASK_RATE_2000_MS 2000
unsigned long prev_run_time_2000_ms;

#define TASK_RATE_5000_MS 5000
unsigned long prev_run_time_5000_ms;

//******************************************************************************
//**** global interfaces *******************************************************
//******************************************************************************
float Temp_degF;
float Humidity_pct;
uint8_t Door_open;
bool mqtt_update_request = false;

//******************************************************************************
//**** local functions *********************************************************
//******************************************************************************

void handleRoot() {
  //digitalWrite(WIFI_LED, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  //digitalWrite(WIFI_LED, 0);
}

void handleNotFound(){
  //digitalWrite(WIFI_LED, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(WIFI_LED, 0);
}

float convertCtoF(float c) {
  return c * 1.8 + 32;
}


//******************************************************************************
//**** wifi functions **********************************************************
//******************************************************************************


void wifi_init()
{
  // prepare GPIO2
  pinMode(WIFI_LED, OUTPUT);
  digitalWrite(WIFI_LED, HIGH); // HIGH is LED off

  WiFi.begin(ssid, password);
  //WiFi.config(LocalIP, GatewayIP, SubnetMask);
  WiFi.softAPdisconnect(true); //Disable the node as an access point
 
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(WIFI_LED, LOW);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void wifi_periodic()
{
  // do nothing
}

//******************************************************************************
//**** http server functions ***************************************************
//******************************************************************************

void http_server_init()
{

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void http_server_periodic()
{
  server.handleClient();
}


//******************************************************************************
//**** mqtt functions **********************************************************
//******************************************************************************

bool mqtt_ok_to_publish()
{
    bool retval = false;

    if(WiFi.status() == WL_CONNECTED)
    {
        if (!MQTT_client.connected())
        {
            if (MQTT_client.connect(MQTT_DEVICE_ID))
            {
                Serial.println("MQTT Connected");
            }
        }
        if(MQTT_client.connected())
        {
            retval = true;
        }
    }

    return retval;
}

void mqtt_init()
{
    MQTT_client.setServer(MQTT_server, 1883);
    // client.setCallback(callback);
    mqtt_periodic();
}

void mqtt_periodic()
{
    // clear the request flag
    mqtt_update_request = false;
    
    char tempStr[10] = {0};
    char humidStr[10] = {0};
    
    // convert to char*
    if(UNKNOWN_32 == Temp_degF)
    {
        tempStr[0] = '-';
        tempStr[1] = 0x00;
    }
    else
    {
        dtostrf(Temp_degF, 3, 2, tempStr);
    }
    
    // convert to char*
    if(UNKNOWN_32 == Humidity_pct)
    {
        humidStr[0] = '-';
        humidStr[1] = 0x00;
    }
    else
    {
        dtostrf(Humidity_pct, 3, 2, humidStr);
    }
    
    // MQTT publish
    if (mqtt_ok_to_publish())
    {
        Serial.println("MQTT Publishing");
        MQTT_client.publish("BEDROOM/Temp_F", tempStr);
        MQTT_client.publish("BEDROOM/Humidity_Pct", humidStr);
        MQTT_client.loop();
    }
}


//******************************************************************************
//**** dht sensor functions ****************************************************
//******************************************************************************
void dht_init()
{
    // Initialize device.
    dht.begin();
    Serial.println("DHTxx Unified Sensor Example");
    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.println("Temperature");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
    Serial.println("------------------------------------");
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.println("Humidity");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
    Serial.println("------------------------------------");
}

void dht_periodic()
{
    float _temperature_prev = Temp_degF;
    float _humidity_prev = Humidity_pct;
    
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    float _temperature = convertCtoF(event.temperature);
    dht.humidity().getEvent(&event);
    float _humidity = event.relative_humidity;
    
    if(isnan(_temperature))
    {
        Temp_degF = UNKNOWN_32;
        Serial.println("Error reading temperature!");
    }
    else
    {
        Temp_degF = _temperature;
        Serial.print("Temperature: ");
        Serial.print(_temperature);
        Serial.println(" *F");
    }
    
    if(isnan(_humidity))
    {
        Humidity_pct = UNKNOWN_32;
        Serial.println("Error reading humidity!");
    }
    else
    {
        Humidity_pct = _humidity;
        Serial.print("Humidity: ");
        Serial.print(_humidity);
        Serial.println("%");
    }
    
    // if the temperature or humidity has changed, request an update
    if(
    ((_temperature_prev != Temp_degF) && (UNKNOWN_32 != Temp_degF)) || 
    ((_humidity_prev != Humidity_pct) && (UNKNOWN_32 != Humidity_pct))
    )
    {
        Serial.println("Requesting Publish");
        mqtt_update_request = true;
    }
}

//******************************************************************************
//**** OS functions ************************************************************
//******************************************************************************

void app_init()
{
    Serial.begin(115200);

    wifi_init();
    //http_server_init();
    mqtt_init();

    dht_init();
}

void task_5000_ms()
{
    dht_periodic();
    mqtt_periodic();
}


//******************************************************************************
//**** Arduino functions *******************************************************
//******************************************************************************

void setup()
{
    app_init();
}

void loop()
{
    // GerthOS
    
    /* We've implemented a super poor-man's scheduler */
    /*  Yah I could use an RTOS, but this was easier  */
    /*  for the limited scope I have... hopefully.    */
    Prev_loop_start_time_ms = Loop_start_time_ms;
    Loop_start_time_ms = millis();
    
    if(Prev_loop_start_time_ms > Loop_start_time_ms)
    {
        /* Handle timer overflow - will probably hit this once every other month */
        /* This is so infrequent i'm not going to bother to be accurate. Just reset the task timers */
        /* This will definitely cause a glitch in timing, but this ain't no fancy-pants scheduler */
        prev_run_time_100_ms = 0;
        prev_run_time_1000_ms = 0;
        prev_run_time_2000_ms = 0;
        prev_run_time_5000_ms = 0;
    }
    
    // Run tasks
    if(prev_run_time_5000_ms + TASK_RATE_5000_MS < Loop_start_time_ms)
    {
        // Run the 5 second task
        task_5000_ms();
        prev_run_time_5000_ms = Loop_start_time_ms;
    }
}
