## Atom mit Platform-IO Plugin

Vorraussetzung für die folgenden Schritte ist eine erfolgreiche installation der Atom-IDE.

### Installation des Platform-IO Plugin
Das Plugin wird über den internen plugin manager installiert. Eine internetverbindung ist nötig.
![](images/platformio/install_1.png)
![](images/platformio/install_2.png)
![](images/platformio/install_3.png)
Die installation von Clang ist optional.
![](images/platformio/install_4.png)


### Projekt erstellen

Um ein neues Projekt zu erstellen muss das [Platform IO Template](https://github.com/teeheee/goldboard4/raw/master/goldboard4-AtomPio-template.zip) heruntergeladen werden und an dem bevorzugten Speicherort extrahiert werden. Es bietet sich an das Template Archiv für später zwischen zu speichern.
Mit dem Menüpunkt Add Projekt Folder kann der Ordner geöffnet werden.
![](images/platformio/usage_1.png)
Die main.cpp findet ihr im src Ordner. Mit dem Haken am linken Rand könnt ihr Compilieren. Der Pfeil ist fürs übertragen. Der Stecker links unten ist für den Seriel Monitor.
![](images/platformio/usage_2.png)
Damit der Serialmonitor funktioniert muss die Baudrate 115200 eingestellt werden.
![](images/platformio/usage_3.png)

### Fuses und Bootloader übertragen

Um die Fuses und den Bootloader übertagen zu können muss in platformio.ini ein Programmer ausgewählt werden der keinen Bootloader benötigt.
Also entweder stk500 oder avrispmkii.

![](images/platformio/bootloader_fuses_3.png)
Nun könnt ihr links unten über das Terminal Symbol eine Konsole öffnen und folgende Befehle eingeben.
Für den Bootloader:

> pio run -t uploadboot

Für die Fuses:

> pio run -t fuse

Es werden nun automatisch die richtigen Fuses oder der Bootloader übertragen.
Beim übertragen des Bootloaders sollte die Led am Goldboard blinken.


### Andere Befehle

Um die neusten Goldboardlibs herunterzuladen:
> pio run -t update

Um den simulator zu starten falls er mal irgendwann funktioniert :-D
> pio run -t simulate

Nur compilieren
> pio run

Übertragen und compilieren
> pio run -t upload
