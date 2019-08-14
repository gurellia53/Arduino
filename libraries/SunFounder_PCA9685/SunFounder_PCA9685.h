
#ifndef _SUNFOUNDER_PCA9685_H_
#define _SUNFOUNDER_PCA9685_H_

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define _MIN_PULSE_WIDTH 500    // the shortest pulse sent to a servo(0 degree)
#define _MAX_PULSE_WIDTH 2500   // the longest pulse sent to a servo (180 degrees)
#define _DEFAULT_PULSE_WIDTH 1500   // default pulse width when servo is attached (90 degrees)

class PCA9685 {
 public:
  PCA9685(uint8_t addr = 0x40);
  PCA9685(uint8_t i2c_addr, float freq);

  void begin(void);
  void reset(void);
  void setFreq(float freq);
  void setServo(uint16_t min_pulse_width, uint16_t max_pulse_width);

  void pwmWrite(uint8_t num, uint16_t val);
  void angleWrite(uint8_t channel, uint8_t angle);

 private:
  uint8_t _i2caddr;
  uint16_t _freq;
  uint16_t _min_pulse_width;
  uint16_t _max_pulse_width;

  uint8_t _readByte(uint8_t addr);
  void _writeByte(uint8_t addr, uint8_t d);
  void _setPWM(uint8_t num, uint16_t on, uint16_t off);

  uint16_t _angle_to_analog(uint8_t angle);
};

#endif
