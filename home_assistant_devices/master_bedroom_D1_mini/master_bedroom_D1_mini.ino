
// Application for garage D1 mini
//  Contains:
//  - DHT11 temperature and humidity sensor
//  - Magnetic door sensor (3-wire)
//  - Wifi
//  - HTTP web server
//  - MQTT

//******************************************************************************
//**** Includes ****************************************************************
//******************************************************************************


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

// Door sensor
#include <door_sensor.h>

//******************************************************************************
//**** App Parameters **********************************************************
//******************************************************************************

// DHT
#define DHTPIN D2    // DHT sensor pin
#define DHTTYPE DHT11 // DHT11 sensor
DHT_Unified dht(DHTPIN, DHTTYPE);

// door sensor
#define DOOR_NO D6
#define DOOR_NC D7
door_sensor door(DOOR_NO, DOOR_NC);

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

#define TASK_RATE_10000_MS 10000
unsigned long prev_run_time_10000_ms;

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

void mqtt_init()
{
  MQTT_client.setServer(MQTT_server, 1883);
  // client.setCallback(callback);
  mqtt_periodic();
}

void mqtt_periodic()
{
  // reset the request flag
  mqtt_update_request = false;
  
  char tempStr[10] = {0};
  char humidStr[10] = {0};
  char doorStr[10] = {0};
  
  if(DOOR_OPEN == Door_open)
    strcpy(doorStr, "Open");
  else if(DOOR_CLOSED == Door_open)
    strcpy(doorStr, "Closed");
  else if(DOOR_FAULTED == Door_open)
    strcpy(doorStr, "Fault");
  
  // digitalWrite ( LED_PIN, LED_ON );
  
  
  if (WiFi.status() == WL_CONNECTED) {
    if (!MQTT_client.connected()) {
      if (MQTT_client.connect("GARAGE")) {
        Serial.println("MQTT Connected");
      } 
    }
    if (MQTT_client.connected()){
      Serial.println("MQTT Publishing");
      MQTT_client.publish("GARAGE/Temp_F",dtostrf(Temp_degF, 3, 2, tempStr));
      MQTT_client.publish("GARAGE/Humidity_Pct",dtostrf(Humidity_pct, 3, 2, humidStr));
      MQTT_client.publish("GARAGE/Door_Open",doorStr);
      MQTT_client.loop();
    }
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
    
  
  if (isnan(_temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Temp_degF = _temperature;
    Serial.print("Temperature: ");
    Serial.print(_temperature);
    Serial.println(" *F");
  }
  
  if (isnan(_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Humidity_pct = _humidity;
    Serial.print("Humidity: ");
    Serial.print(_humidity);
    Serial.println("%");
  }
  
  // if the temperature or humidity has changed significantly, request an update
  if (abs(_temperature_prev - _temperature) > 2.0)
    mqtt_update_request = true;
  if (abs(_humidity_prev - _humidity) > 2.0)
    mqtt_update_request = true;
}

//******************************************************************************
//**** door sensor functions ***************************************************
//******************************************************************************

void door_sensor_init()
{
  door.begin();
}

void door_sensor_periodic()
{
  uint8_t Door_open_prev = Door_open;
  
  switch (door.check_door())
  {
    case DOOR_OPEN:
      Door_open = DOOR_OPEN;
      Serial.println("Door Open");
      break;
    case DOOR_CLOSED:
      Door_open = DOOR_CLOSED;
      Serial.println("Door Closed");
      break;
    default:
      Door_open = DOOR_FAULTED;
      Serial.println("Door Faulted");
  }
  
  // request mqtt update if the door has changed
  if(Door_open_prev != Door_open)
  {
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
  http_server_init();
  mqtt_init();
  
  dht_init();
  door_sensor_init();
}

void task_100_ms()
{
    door_sensor_periodic();
    
    if(true == mqtt_update_request)
    {
        mqtt_periodic(); // update mqtt on request
    }
}

void task_1000_ms()
{
    http_server_periodic();
}

void task_2000_ms()
{
    dht_periodic();
}

void task_10000_ms()
{
    mqtt_periodic();
    wifi_periodic(); // does nothing
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

  if(Prev_loop_start_time_ms > Loop_start_time_ms){
    /* Handle timer overflow - will probably hit this once every other month */
    /* This is so infrequent i'm not going to bother to be accurate. Just reset the task timers */
    /* This will definitely cause a glitch in timing, but this ain't no fancy-pants scheduler */
    prev_run_time_100_ms = 0;
    prev_run_time_1000_ms = 0;
    prev_run_time_2000_ms = 0;
    prev_run_time_10000_ms = 0;
  }

  // Run tasks

  if(prev_run_time_100_ms + TASK_RATE_100_MS < Loop_start_time_ms) {
    // Run the 100 millisecond task
    task_100_ms();
    prev_run_time_100_ms = Loop_start_time_ms;
  }
  
  if(prev_run_time_1000_ms + TASK_RATE_1000_MS < Loop_start_time_ms) {
    // Run the 1 second task
    task_1000_ms();
    prev_run_time_1000_ms = Loop_start_time_ms;
  }
  
  if(prev_run_time_2000_ms + TASK_RATE_2000_MS < Loop_start_time_ms) {
    // Run the 2 second task
    task_2000_ms();
    prev_run_time_2000_ms = Loop_start_time_ms;
  }
  
  if(prev_run_time_10000_ms + TASK_RATE_10000_MS < Loop_start_time_ms) {
    // Run the 10 second task
    task_10000_ms();
    prev_run_time_10000_ms = Loop_start_time_ms;
  }
}
