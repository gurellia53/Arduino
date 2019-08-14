#include <SunFounder_PCA9685.h>

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  500 // this is the 'minimum' pulse length count (out of 0 degree)
#define SERVOMAX  2500 // this is the 'maximum' pulse length count (out of 180 degrees)

PCA9685 pwm1 = PCA9685(0x40);

int servo1_chn = 15;
int servo2_chn = 0;

void setup() {
  Serial.begin(9600);

  pwm1.begin();
  Serial.print("begin  ");

  // default frequency is 50
  //pwm1.setFreq(50);

  // setServo(_min_pulse_width, _max_pulse_width), check them on the servo datasheet
  pwm1.setServo(SERVOMIN, SERVOMAX);

  //  servo on servo1_chn, turn to 90 degrees
  pwm1.angleWrite(servo1_chn,90);
  pwm1.angleWrite(servo2_chn,90);
  Serial.println("angle 90");
}

void loop() {
  pwm1.angleWrite(servo1_chn,0);
  pwm1.angleWrite(servo2_chn,0);
  Serial.println("->angle 0   ");
  delay(1000);

  pwm1.angleWrite(servo1_chn,90);
  pwm1.angleWrite(servo2_chn,90);
  Serial.println("->angle 90   ");
  delay(1000);

  pwm1.angleWrite(servo1_chn,180);
  pwm1.angleWrite(servo2_chn,180);
  Serial.println("->angle 180   ");
  delay(1000);

  pwm1.angleWrite(servo1_chn,90);
  pwm1.angleWrite(servo2_chn,90);
  Serial.println("->angle 90   ");
  delay(1000);

  Serial.println("   ");
}
