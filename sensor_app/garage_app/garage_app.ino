
#include <app_wifi.h>
#include <app_dht.h>

#define DHTPIN D2    // DHT sensor pin
#define DHTTYPE DHT11 // DHT11 sensor

void app_init()
{
  Serial.begin(115200);
  
  wifi_init();
  dht_init();
  
}

void app_periodic()
{
  wifi_periodic();
  dht_periodic();
}

void setup()
{
  app_init();
}

void loop()
{
  app_periodic();
}
