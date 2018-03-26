# Goldboard4 Version 2.0
## TODO
* do full test 
   * ~~motors~~
   * servos
   * ~~pwmpins~~ 
   * ~~digitalpins~~
   * ~~analogpins~~
   * ~~buttons~~
   * ~~leds~~
   * robot->bluetooth->pc
   * robot->bluetooth->robot
   * ~~power pins~~
   * ~~CMPS11~~
   * ~~CMPS03~~
   * SRF08
   * SRF10
   * ~~VL53L0X~~
   * ~~pixy~~
   * usring
   * lcd
* make clean doxygen comments
* add BNO055 sensor
* upgrade servo PWM to every pin
* selftest
* check race condition on interrupts

## Features

* motor acceleration limit
* intelligent motor PWM generation
* single Servo PWM output
* i2c lcd support
* digital read and write on every GB-Pin
* selftest on every start cycle
* Bluetooth support for RN42 module
* I2C scan feature with part identification
* full support for I2C Arduino libs
* EEPROM support
* fast pulsed-IR-sensor lib
* Pixy support
* usring support
* VL53L0X supprt

## Usage

### Atmelstudio 7

### Eclipse IDE for C/C++ Developers


## Memory Usage
minimal Build

```
Program:    4246 bytes (13.0% Full)
(.text + .data + .bootloader)

Data:        262 bytes (12.8% Full)
(.data + .bss + .noinit)
```
PULSE_SENSOR_INPUT, MOTOR_ACCELERATION and SERVO_PWM enabled Build

```
Program:    6850 bytes (20.9% Full)
(.text + .data + .bootloader)

Data:        576 bytes (28.1% Full)
(.data + .bss + .noinit)
```