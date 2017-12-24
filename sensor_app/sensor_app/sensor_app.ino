// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include "Soil_Moisture_Sensor.h"

// ************************ Configs ************************
#define DHTPIN D2    // DHT sensor pin
#define SMSPIN A0   // SMS sensor pin

#define DHTTYPE DHT11 // DHT11 sensor
#define SMSTYPE SENSOR_MODE_ANALOG // Analog mode

// ************************ Objects ************************
DHT                  dht(DHTPIN, DHTTYPE);
Soil_Moisture_Sensor sms(SMSPIN, SMSTYPE);

// ************************ Scheduler Globals ************************
unsigned long loop_counter_t1 = 0;
bool task_01x_trigger = false;
bool task_10x_trigger = false;
bool task_20x_trigger = false;


// ************************ App Functions ************************

// ****** DHT ******
// temperature & humidity sensor
static void dht_periodic ()
{ 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    
    // Print to serial
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
  }
  
}

// ****** SMS ******
// soil moisture sensor
static void sms_periodic ()
{ 
  int moisture_sensor_val = sms.readMoisture();
  float moisture_percent = sms.convertPercent(moisture_sensor_val);
  
  if (isnan(moisture_sensor_val) || isnan(moisture_percent))
  {
    Serial.println("Failed to read from moisture sensor!");
  }
  else
  {
    Serial.print("Soil Moisture: ");
    Serial.print(moisture_sensor_val);
    Serial.print(" %\t");
    Serial.print("Percent: ");
    Serial.println(moisture_percent);
  }
}



// ************************ Application Init ************************
void app_init()
{
  // initialize the serial connection
  Serial.begin(9600);
  Serial.println("Initializing Application");
  
  // feature inits
  dht.begin();
  sms.begin();
}

// 1x task
void task_01x()
{
  // ****** SMS ******
  sms_periodic();
  
  // ****** DHT ******
  dht_periodic();
}

// ************************ ISR Init ************************
void isr_init()
{
  // Timer1 init - for task timing
}


// ************************ Arduino Setup ************************
void setup()
{
  // Arduino Setup
  isr_init();
  app_init();
}

// ************************ Arduino Loop ************************
void loop()
{
  delay(2000); 
  // Arduino Main Loop
  task_01x();
}
