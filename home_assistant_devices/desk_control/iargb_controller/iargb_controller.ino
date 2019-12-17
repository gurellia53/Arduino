
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

#define PIN         D3
#define NUMPIXELS   127
Adafruit_NeoPixel pixels_0(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#define DELAYVAL 5
#define BRIGHTNESS_0 64
#define BRIGHTNESS_1 48

//colors
uint32_t YELLOW = 0x30FF00; //0xB2FE00
uint32_t WHITE = 0x30FFFF;

int WIDTH = 12;


void do_something( int pixel, int loop )
{
  int width = 1;
  if ( loop > WIDTH )
  {
    width = WIDTH;
  }
  else
  {
    width = loop;
  }
  
  if ( (pixel < loop + width) && (pixel > loop - width) )
  {
    // moving color
    pixels_0.setPixelColor( pixel, WHITE );
  }
  else
  {
    // background
    pixels_0.setPixelColor( pixel, YELLOW );
  }
  pixels_0.setBrightness( BRIGHTNESS_0 );

}



void setup()
{
  pixels_0.begin(); // INITIALIZE NeoPixel strip object
  pixels_0.clear(); // Set all pixel colors to 'off'
}

void loop()
{
  static int j = 0;
  
  // For each pixel_0
  for( int i=0; i < NUMPIXELS; i++ )
  {
    do_something( i, j );
  }
  
  if ( DELAYVAL )
  {
    delay(DELAYVAL);
  }
  pixels_0.show();

  j++;
  if ( j > NUMPIXELS )
  {
    j = 0;
  }
}
