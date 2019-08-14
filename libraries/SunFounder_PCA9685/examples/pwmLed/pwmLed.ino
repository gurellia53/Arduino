/***************************************************
  This is an example for our Sunfounder 16-channel PWM & Servo driver
  PWM test - this will drive 16 leds, brightness increase and then decrese

 ****************************************************/

#include <SunFounder_PCA9685.h>

PCA9685 pwm1 = PCA9685(0x40);

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel pwmLed test!");

  pwm1.begin();

  pwm1.setFreq(60);

}

void loop() {
  // All channel increased value, value between 0 to 4095
  for(int value=0; value<4096; value+=100){
    for(int channel=0; channel<16; channel++){
       pwm1.pwmWrite(channel, value);
    }
    //delay(150);
    Serial.print("analog value:");
    Serial.println(value);
  }
  // All channel decreased value, value between 4095 to 0
  for(int value=4095; value>0; value-=100){
    for(int channel=0; channel<16; channel++){
       pwm1.pwmWrite(channel, value);
    }
    delay(30);
    Serial.print("pwm value:");
    Serial.println(value);
  }
}