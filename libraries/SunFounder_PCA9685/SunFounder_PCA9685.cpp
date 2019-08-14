
#include "SunFounder_PCA9685.h"
#include <Wire.h>


PCA9685::PCA9685(uint8_t addr) {
  _min_pulse_width = _MIN_PULSE_WIDTH;
  _max_pulse_width = _MAX_PULSE_WIDTH;
  _i2caddr         = addr;
  _freq            = 50;
}

PCA9685::PCA9685(uint8_t addr, float freq){
    _min_pulse_width = _MIN_PULSE_WIDTH;
    _max_pulse_width = _MAX_PULSE_WIDTH;
    _i2caddr         = addr;
    _freq            = freq;
}

void PCA9685::begin(void) {
 Wire.begin();
 reset();
 setFreq(_freq);
}

void PCA9685::reset(void) {
 _writeByte(PCA9685_MODE1, 0x0);
}

void PCA9685::setFreq(float freq) {
  //Serial.print("Attempting to set freq ");
  //Serial.println(freq);
  //freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;

  uint8_t prescale = floor(prescaleval + 0.5);

  uint8_t oldmode = _readByte(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  _writeByte(PCA9685_MODE1, newmode); // go to sleep
  _writeByte(PCA9685_PRESCALE, prescale); // set the prescaler
  _writeByte(PCA9685_MODE1, oldmode);
  delay(5);
  _writeByte(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
                                          // This is why the beginTransmission below was not working.
  //  Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
}

void PCA9685::setServo(uint16_t min_pulse_width, uint16_t max_pulse_width){
  _min_pulse_width = min_pulse_width;
  _max_pulse_width = max_pulse_width;
}

// Sets pin without having to deal with on/off tick placement and properly handles
// a zero value as completely off.  Optional invert parameter supports inverting
// the pulse for sinking to ground.  Val should be a value from 0 to 4095 inclusive.
void PCA9685::pwmWrite(uint8_t num, uint16_t val){
  if (val == 4095) {
    // Special value for signal fully on.
    _setPWM(num, 4096, 0);
  }
  else if (val == 0) {
    // Special value for signal fully off.
    _setPWM(num, 0, 4096);
  }
  else {
    _setPWM(num, 0, val);
    }
}

void PCA9685::angleWrite(uint8_t channel, uint8_t angle){
  if (channel > 15)  { channel = 15; }
  if (channel < 0)   { channel = 0;  }
  if (angle > 180)   { angle = 180;  }
  if (angle < 0)     { angle = 0;    }
  uint16_t analog_value = _angle_to_analog(angle);
  //Serial.print("analog_value:  ");Serial.println(analog_value);
  pwmWrite(channel, analog_value);
}

uint8_t PCA9685::_readByte(uint8_t addr) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(addr);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  return Wire.read();
}

void PCA9685::_writeByte(uint8_t addr, uint8_t d) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(addr);
  Wire.write(d);
  Wire.endTransmission();
}

void PCA9685::_setPWM(uint8_t num, uint16_t on, uint16_t off) {
  //Serial.print("Setting PWM "); Serial.print(num); Serial.print(": "); Serial.print(on); Serial.print("->"); Serial.println(off);

  Wire.beginTransmission(_i2caddr);
  Wire.write(LED0_ON_L+4*num);
  Wire.write(on);
  Wire.write(on>>8);
  Wire.write(off);
  Wire.write(off>>8);
  Wire.endTransmission();
}

uint16_t PCA9685::_angle_to_analog(uint8_t angle){
  // Calculate 12-bit analog value from giving angle
  //Serial.println("_angle_to_analog:  ");
  uint16_t pulse_width   = map(angle, 0, 180, _min_pulse_width, _max_pulse_width);
  uint16_t analog_value = int(float(pulse_width) / 1000000 * _freq * 4096);
  return analog_value;
}