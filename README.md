# Goldboard4 Version 2.1

## TODO
* full test
   * ~motors~
   * servos
   * ~pwmpins~
   * ~digitalpins~
   * ~analogpins~
   * ~buttons~
   * ~leds~
   * robot->bluetooth->pc
   * robot->bluetooth->robot
   * ~power pins~
   * CMPS11
   * ~CMPS03~
   * ~SRF08~
   * SRF10
   * VL53L0X
   * ~pixy~
   * ~usring~
   * ~lcd~
* make clean doxygen comments
* upgrade servo PWM to every pin
* check race condition on interrupts
* add i2c hang up recovery

## Hardware

Die Pins des Atmega32 sind am Goldboard4 wie in der folgenden Grafik belegt. PAx, PBx, PCx, und PDx sind die Ports des AVR. ADCx sind die Analogpins des Goldboard. Dx sind die Digitalpins des Goldboard.
Die Nummern stimmen mit der lib überein.

![](images/Pinbelegung.svg.png)

## Entwicklungsumgebung

### Atmelstudio 7

#### Template importieren

Das [Atmelstudio 7 Template](goldboard4-2.1-AS7-template.zip) muss als erstes heruntergeladene werden und in den Template Ordner eurer Atmelstudio installation importiert werden.
![](images/atmelstudio7/projekt_template_importieren.png)
Hier müsst ihr das heruntergeladene Template auswählen.
![](images/atmelstudio7/projekt_template_importieren_2.png)

#### Projekt erstellen

Um ein Porjekt zu erstellen geht ihr wie gewohnt vor.
![](images/atmelstudio7/projekt_erstellen.png)
Vergesst nicht den Namen und den Ort (Location) eures Projekts zu ändern damit ihr es später wieder findet.
![](images/atmelstudio7/projekt_erstellen_2.png)

#### Fuses setzen und Bootloader hochladen

Unter Tools->Device Programming habt ihr die Möglichkeit, euren Programmer direkt zu steuern.
![](images/atmelstudio7/fuses.png)
Dazu wählt ihr euren Programmer und den verwendeten AVR (Atmega32) aus und drückt Apply.
![](images/atmelstudio7/fuses2.png)
Falls hier ein Fehler auftritt überprüft ob euer Goldboard richtig angeschlossen ist und unter Strom steht. Außerdem kann es vorkommen, dass der Treiber nicht richtig installiert wurde.

Zum setzen der Fuse Bits schreibt folgende Hex Zahlen in die Felder für Low Byte und High Byte und drückt Programm!

* Low Byte:   0xFF
* High Byte:  0xD4

> TODO bild fehlt

Um den Bootloader hochzuladen wählt die in eurem projekt enthaltene datei bootloader_mega32_optiboot.hex aus. Diese findet ihr in dem Ordner utils. Durch drücken von program ladet ihr den Bootloader hoch.

#### Programmer für Bootloader konfigurieren

zum Hochladen über usb müsst ihr euren Programmer wie folgt konfigurieren. Kopiert folgende zeile in das Command Feld:

![](images/bootloader_einstellungen.png)
> "$(SolutionDir)\utils\avrdude" -c arduino -p m32 -P com5 -b 115200 -U flash:w:"$(OutputDirectory)\$(OutputFileName).hex":i

Je nachdem unter welchem COM-Port euer Goldboard erkannt wird, müsst ihr
"com5" in z.B. com3 abändern.


## Atom with Platform-IO plugin

> TODO text

## Makefile for users without a GUI

### Dependencies:
avr-libc gcc-avr make avrdude

### Compile:
make

### Upload:
make program
