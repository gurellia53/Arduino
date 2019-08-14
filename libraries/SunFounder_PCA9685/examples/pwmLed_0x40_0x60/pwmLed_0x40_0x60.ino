/***************************************************
  This is an example for our Sunfounder 16-channel PWM & Servo driver
  PWM led_0x40_0x60 - this will drive 16 leds, both on two boards

 ****************************************************/

#include <SunFounder_PCA9685.h>

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

PCA9685 pwm1 = PCA9685(0x40);
PCA9685 pwm2 = PCA9685(0x60);

void setup() {
  Serial.begin(9600);
  Serial.println("two board 16 channel pwm led test!");
}

void loop() {
  pwm1.begin();    // select the 0x40 board
  pwm1_channel_all();

  pwm2.begin();   // select the 0x40 board
  pwm2_channel_all();
}

void pwm1_channel_all(){
   // All channel increased pwm value
  for(int value=0; value<4096; value++){
    for(int channel=0; channel<16; channel++){
       pwm1.pwmWrite(channel, value);
    }
  }
  for(int value=4095; value>0; value--){
    for(int channel=0; channel<16; channel++){
       pwm1.pwmWrite(channel, value);
    }
  }
}

void pwm2_channel_all(){
   // All channel decreased pwm value
  for(int value=4095; value>0; value--){
    for(int channel=0; channel<16; channel++){
       pwm2.pwmWrite(channel, value);
    }
  }
  for(int value=0; value<4096; value++){
    for(int channel=0; channel<16; channel++){
       pwm2.pwmWrite(channel, value);
    }
  }
}