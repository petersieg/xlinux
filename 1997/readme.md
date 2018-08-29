This is a xlinux from 1997 - probably last known/found version.

Check install.bat - untgz xlinux.tgz inside /xlinux directory.

---

As always. If you make a qemu / real install, please let me know / show a picture ;-)

---

Xlinux mit "install <x> <y>" installieren.
<y> = Quell-Laufwerksbuchstabe
<y> = Ziel-Laufwerksbuchstabe
Also etwa mit "install d: c:", wenn die CD in D: liegt
Kernal ist für IDE-System.
Dann nach x:\linux gehen.
linux.bat x
login: root - Passwort: leer oder geheim
  
Das PCMCIA Modem muss, wenn keine PCMCIA-Card Services unter Linux
aktiviert
wurde, unter DOS initialisiert werden!! Also die entsprechenden DOS Aufrufe
in Config.sys und autoexec von dem linux.bat start drin lassen! Mit Minicom
dann
ersteinmal probieren, ob das Modem antwortet. Evtl. auch den link in /dev
pruefen
/dev/modem -> /dec/cua2 (bei com3)

Das Problem mit zu vielen Drives tritt anscheinend bei mehr als zwei Platten auf.
Das Programm loadlinx, welches dann loadlin aufruft, uebersetzt die DOS LW
c:, d:, e: etc. nach UNIX Konvention /dev/xxx. Dabei gibt es wohl dann bei der Vielzahl Schwierigkeiten. Eine Loesung kann die d
irekte Angabe von /dev/xxx anstatt
c:, d: etc. sein:
Erste IDE       Primaer Partition       = /dev/hda1
Erstes log. LW in Ext.Part.     = /dev/hda5
Zweites log. LW in Ext.Part.    = /dev/hda6
Zeite IDE Platte                = /dev/hdb1
Evtl.einfach /dev/hda1 angeben, und bei den Kernelmeldungen auf die erkannten
Partitionen achten! Wenn es nicht zufaellig die richtige Angabe wahr, bleibt
UMSDOS nach! dem Kernel-Boot haengen, mit VFS unable to mount root...
Dann nacheinander die anderen erkannten Partitionen probieren...

BTW - xlinux ist entstanden und wird gepflegt auf einen Sattelite T2155CDS Laptop
von Toshiba... Mit hoeher aufloesenden Grafiken habe ich da so meine Probleme...
Ich hoffe, das das Modul-Konzept mit der initrd (Ram-Disk beim Booten) in naher
Zukunft, die Kernel-Vielfalt entkrampfen wird... XFREE als X11 Server wird wohl
weiterhin ein Kapitel fuer sich bleiben...
Auf das wir weiterhin Interessantes in der Computer-Technik erleben duerfen...

Da wa noch ein Fehler in den /dev/.. Eintraegen. In der Busmouse/Howto
wird die Logitech NICHT als serielle Maus, sondern als BUSMOUSE gefuehrt!!!
Damit das funktioniert bitte folgendes ausfuehren:
cd /dev
mknod logibm c 10 0

