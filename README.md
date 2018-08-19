# Goldboard4 Version 2.1
## TODO
* do full test 
   * motors
   * servos
   * pwmpins 
   * digitalpins
   * analogpins
   * buttons
   * leds
   * robot->bluetooth->pc
   * robot->bluetooth->robot
   * power pins
   * CMPS11
   * CMPS03
   * SRF08
   * SRF10
   * VL53L0X
   * pixy
   * usring
   * lcd
* make clean doxygen comments
* upgrade servo PWM to every pin
* selftest
* check race condition on interrupts
* reduce code size
* reorganize ERROR Messages

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

### everything enabled
```
Program:    4566 bytes (13.9% Full)
(.text + .data + .bootloader)

Data:        635 bytes (31.0% Full)
(.data + .bss + .noinit)
```
### everything disabled

```
Program:    3372 bytes (10.3% Full)
(.text + .data + .bootloader)

Data:        369 bytes (18.0% Full)
(.data + .bss + .noinit)
```


