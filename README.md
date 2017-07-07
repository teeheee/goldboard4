# Goldboard4 libs V1.1

Achtung. Die lib ist noch nicht ausgiebig getestet.

Änderungen:
- Button und Leds gehen gleichzeitig
- Maximale Beschleunigung der Motoren kann festgelegt werde
- Servo und Brushless Support
- VL53L0X Support
- U's Sensorring
- i2c Scanner
- Protexpander integriert (Digitalpins gehen jetzt bis 11)
- schnellere IRSensor Abfrage
- PWM für PowerPorts
- checkAck Funktion für alle i2c libs

## Download

[V 1.1](https://github.com/teeheee/goldboard4/archive/release_1.1.zip)

[V 1.0](https://github.com/teeheee/goldboard4/archive/release_1.0.zip)

## /AS7_Template (Ateml Studio 7 Template)

File->Import->Project Template...

Select a project template [...] -> "<current Path>/AS7_Template/goldboard4_V1.1.zip" -> OK

File->New->Project...

goldboard4_V1.1->OK

## /Makefile_Template (for Linux users and/or Visual Studio haters)

### Dependencies:
avr-libc gcc-avr make avrdude

### Compile:
make

### Upload:
make program

### Fuse:
make fuse

