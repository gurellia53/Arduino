
//

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "G:\network_config\network_config_private.h"

#include <door_sensor.h>


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

// door sensor
#define DOOR_NO D6
#define DOOR_NC D7
door_sensor door(DOOR_NO, DOOR_NC);

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
  
  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void wifi_periodic()
{
  server.handleClient();
}


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
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }
}

void door_sensor_init()
{
  
}

void door_sensor_periodic()
{
  switch (door.check_door())
  {
    case DOOR_OPEN:
      Serial.println("Door Open");
      break;
    case DOOR_CLOSED:
      Serial.println("Door Closed");
      break;
    default:
      Serial.println("Door Faulted");
  }
}


void app_init()
{
  Serial.begin(115200);
  
  wifi_init();
  dht_init();
  door_sensor_init();
}

void app_periodic()
{
  wifi_periodic();
  dht_periodic();
  door_sensor_periodic();
}

void setup()
{
  app_init();
}

void loop()
{
  app_periodic();
  delay(2000);
}
