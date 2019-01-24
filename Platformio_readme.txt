Eine kurze Übersicht über alle Befehle:

Fuses setzen:
> pio run -t fuse

Bootloader übertragen:
> pio run -t uploadboot

Programm compilieren
> pio run

Programm übertragen:
> pio run -t upload

Goldboardlib updaten:
> pio run -t update

Simulator starten:
> pio run -t simulate

Für den simulate Befehl muss der Simulator von https://github.com/teeheee/goldboard4-simulator
heruntergeladen werden und die config.json Datei wie folgt angepasst werden:

{
  "firmware" : ".pioenvs/main/firmware.hex",
  ....
}


############ Problem behebung ############

Problem :
  Übertragen mit dem avrispmkii geht nicht.
  > avrdude done.  Thank you.
  > *** Error 1
Lösung:
  1. Lade zadig-x.x.exe von https://zadig.akeo.ie/ herunter
  2. Öffne das Programm und aktiviere "List All Devices" unter "Options"
  3. installiere libusb-win32 für den avrispmkii
  4. merk dir, dass du den Treiber wieder zurückstellen musst, wenn du Atmel Studio mit dem avrispmkii verwenden willst.


Problem:
  Übertragen vom Programm mit dem Bootloader (arduino) hängt.
  >
Lösung:
  1. reset Knopf drücken
  2. weniger Daten mit SERIAL_PRINTLN ausgeben (delay(1000)!!!)
  3. SERIAL_BLOCKER in config.h auskommentieren
