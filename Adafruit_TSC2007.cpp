/*!
 *  @file Adafruit_TSC2007.cpp
 *
 *  @mainpage Adafruit TSC2007 Resistive Touch Panel library
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Adafruit TSC2007 Resistive Touch Panel Sensor library
 * for Arduino
 *
 * 	This is a library for the Adafruit TSC2007 breakout:
 * 	https://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"

#include "Adafruit_TSC2007.h"

/*!
 *    @brief  Instantiates a new TSC2007 class
 */
Adafruit_TSC2007::Adafruit_TSC2007(void) {}

Adafruit_TSC2007::~Adafruit_TSC2007(void) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  address
 *            The I2C address to use, defaults to 0x48
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TSC2007::begin(uint8_t address, TwoWire *wire) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  command(MEASURE_TEMP0, POWERDOWN_IRQON, ADC_12BIT);
  return true;
}

/*!
 *    @brief  Send a command and read 2 bytes from TSC
 *    @param  func The command function to make
 *    @param  pwr  The power mode to enter after command
 *    @param  res  The ADC resolution
 *    @return 12 bits of data shifted from the 16-bit read value
 */
uint16_t Adafruit_TSC2007::command(adafruit_tsc2007_function func,
                                   adafruit_tsc2007_power pwr,
                                   adafruit_tsc2007_resolution res) {
  uint8_t cmd = (uint8_t)func << 4;
  cmd |= (uint8_t)pwr << 2;
  cmd |= (uint8_t)res << 1;

  uint8_t reply[2];

  i2c_dev->write_then_read(&cmd, 1, reply, 2);

  return ((uint16_t)reply[0] << 4) | (reply[1] >> 4); // 12 bits
}

/*!
 *    @brief  Read touch data from the TSC and then power down
 *    @param  x Pointer to 16-bit value we will store x reading
 *    @param  y Pointer to 16-bit value we will store y reading
 *    @param  z1 Pointer to 16-bit value we will store z1 pressure reading
 *    @param  z2 Pointer to 16-bit value we will store z1 pressure reading
 *    @return True if ADC was able to read the x & y values
 */
bool Adafruit_TSC2007::read_touch(uint16_t *x, uint16_t *y, uint16_t *z1,
                                  uint16_t *z2) {
  *x = command(MEASURE_X, ADON_IRQOFF, ADC_12BIT);
  *y = command(MEASURE_Y, ADON_IRQOFF, ADC_12BIT);
  *z1 = command(MEASURE_Z1, ADON_IRQOFF, ADC_12BIT);
  *z2 = command(MEASURE_Z2, ADON_IRQOFF, ADC_12BIT);
  command(MEASURE_TEMP0, POWERDOWN_IRQON, ADC_12BIT);

  return (*x != 4095) && (*y != 4095);
}
