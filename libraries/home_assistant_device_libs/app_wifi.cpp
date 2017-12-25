
// requires serial

#include <app_wifi.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>


ESP8266WebServer server(80);
#define WIFI_LED D4

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void handleRoot() {
  // digitalWrite(WIFI_LED, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  // digitalWrite(WIFI_LED, 0);
}

void handleNotFound(){
  // digitalWrite(WIFI_LED, 1);
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
  // digitalWrite(WIFI_LED, 0);
}

void wifi_init()
{
    // prepare GPIO2
    pinMode(WIFI_LED, OUTPUT);
    digitalWrite(WIFI_LED, HIGH); // HIGH is LED off

    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    digitalWrite(WIFI_LED, LOW);
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // if (MDNS.begin("esp8266"))
    // {
        // Serial.println("MDNS responder started");
    // }

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