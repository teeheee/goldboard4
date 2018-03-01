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
   * CMPS03
   * SRF08
   * SRF10
   * ~~VL53L0X~~
   * pixy
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
Program:   16548 bytes (50.5% Full)
(.text + .data + .bootloader)

Data:        370 bytes (18.1% Full)
(.data + .bss + .noinit)
```
everything enabled Build

```
Program:   17042 bytes (52.0% Full)
(.text + .data + .bootloader)

Data:        636 bytes (31.1% Full)
(.data + .bss + .noinit)
```