# Atmelstudio 7

Vorraussetzung für die folgenden Schritte ist eine erfolgreiche installation von Atmelstudio 7.

## Template importieren

Das [Atmelstudio 7 Template](https://github.com/teeheee/goldboard4/raw/master/goldboard4-2.1-AS7-template.zip) muss als erstes heruntergeladene werden und in den Template Ordner eurer Atmelstudio installation importiert werden.
![](images/atmelstudio7/projekt_template_importieren.png)
Hier müsst ihr das heruntergeladene Template auswählen.
![](images/atmelstudio7/projekt_template_importieren_2.png)

## Projekt erstellen

Um ein Porjekt zu erstellen geht ihr wie gewohnt vor.
![](images/atmelstudio7/projekt_erstellen.png)
Vergesst nicht den Namen und den Ort (Location) eures Projekts zu ändern damit ihr es später wieder findet.
![](images/atmelstudio7/projekt_erstellen_2.png)

## Fuses setzen und Bootloader hochladen

Unter Tools->Device Programming habt ihr die Möglichkeit, euren Programmer direkt zu steuern.
![](images/atmelstudio7/fuses.png)
Dazu wählt ihr euren Programmer und den verwendeten AVR (Atmega32) aus und drückt Apply.
![](images/atmelstudio7/fuses2.png)
Falls hier ein Fehler auftritt überprüft ob euer Goldboard richtig angeschlossen ist und unter Strom steht. Außerdem kann es vorkommen, dass der Treiber nicht richtig installiert wurde.

Zum setzen der Fuse Bits schreibt folgende Hex Zahlen in die Felder für Low Byte und High Byte und drückt Programm!

* Low Byte:   0xFF
* High Byte:  0xD4

![](images/atmelstudio7/fuses3.png)

Um den Bootloader hochzuladen wählt die in eurem projekt enthaltene datei bootloader_mega32_optiboot.hex aus. Diese findet ihr in dem Ordner utils. Durch drücken von program ladet ihr den Bootloader hoch. Wenn keine Fehler auftreten, sollte die Led am Goldboard blinken.


![](images/atmelstudio7/bootloader.png)

## Programmer für Bootloader konfigurieren

zum Hochladen über usb müsst ihr euren Programmer wie folgt konfigurieren. Kopiert folgende Zeile in das Command Feld:

![](images/atmelstudio7/bootloader_einstellungen.png)

>"$(MSBuildProjectDirectory)\utils\avrdude\avrdude.exe" -c arduino -p m32 -P com5 -b 115200 -U flash:w:"$(OutputDirectory)\$(OutputFileName).hex":i

Je nachdem unter welchem COM-Port euer Goldboard erkannt wird, müsst ihr
"com5" in z.B. "com3" ändern.
