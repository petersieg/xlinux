# xlinux

![Bild](https://github.com/petersieg/xlinux/blob/master/xlinux.jpg)

xlinux distribution from 1996 as published in Chip 11/1996.
This is an old, historical distribution with kernal 1.2.13 and X11R5, that runs on top of a dos file system.
Just unrar it into root dir of hdd (will expand into \linux directory).

Links:
https://www.dim13.org/xlinux

Was ist in xlinux - Textbasierende Programme:

● Alles für DOS User...

● Mouseless Commander - Der NC Clone

● SC - Das Spreadsheet

● Vi - Editor

● Joe - Editor

● edit - deutscher Editor mit voller Tastatur und Menue-Steuerung

● TeX/LaTeX - Das Satzsystem

● Ghostscript - Druck von Postscriptdateien auf Standarddruckern

● Fax - „Faxen“ von Text und Postscriptdateien

● ssfile - Einfache B-Tree Isam Dateiverwaltung

● MSQL/PQL - Die SQL Datenbanken

● Lynx - Der Text-HTML Browser

● Perl/Awk - Die textorientierte Programmiersprache

● basic, bwbasic - Basic Interpreter

● Linuxdoc-SGML - Konverter für unterschiedliche Textformate

● UDO - der vielseitigere Text Konverter - deutsche Shareware!

● Groff - Der Textkonvertierer

● Minicom, ecu, xc, kermit, rx/y/z+sx/y/z - Terminalprogramme

● Gnuplot - Plotten von Graphen

● Ldlas - Statitische Auswertungen - nutzt gnuplot

● casio - Datentransfer zum und von Casio SF-4/8xxx Organizern

● dbview - Listen und Konvertieren von Dbase III Dateien

● tde - Editor für Binaerdateien

● pgp - Pretty Good Privacy - Daten verschluesseln etc.

● recode - GNU Zeichensatz Konverter

● pcmcia - Utilities fuer PCMCIA Interface

● Chess, Tetris, Sasteroids, lizards - Spiele


Was ist in xlinux - X11 Programme:

● X11R5 Server

● Axe - Editor

● Xcoral - Editor

● Doc - Editor auch für LaTeX...

● ez - Andrew WYSIWYG Frontend fuer LaTeX...

● lyx - Deutscher Frontend fuer LaTeX...

● Xcalc - Taschenrechner

● Calctool - Besserer Taschenrechner

● Xspread - Tabellenkalkulation

● Oleo - Die GNU Tabellenkalkulation

● Nexs - Demo dieser Tabellenkalkulation

● xmgr - Plotting Tool ala Gnuplot

● Xlispstat - Statistikpaket in Lisp

● Xpaint - Bitmap Zeichenprogramm

● Xfig - Vektor Zeichenprogramm

● Idraw - Vektor Zeichenprogramm auch für doc...

● Xv - Bildbetrachter und Konverter

● Xgrab - Screenshots

● Ghostview - Ansehen von Postscript Dateien

● xhtml - HTML-Editor

● Xmosaic - HTML Browser

● Cbb - Kontoführung

● addressbook - Auch deutsches Adressverwaltungsprogramm, mit Tcl/Tk erstellt

● Xinvest - Anlageverwaltung

● plan - Terminkalendar - Planer

● Tcl/Tk - Script X11 Programmentwicklung

● Wine - Windows Emulator

● xfileman und uc - Dateimanager

● uc - Unix Cockpit - Dateimanager und mehr...

● Seyon - X11 Terminalprogramm

● xanim - X11 MPEG Player

● xearth - Rotierende Weltkugel auf dem X11 Desktop

● pcemu - DOS Emulator unter X11 für Textprogramme


---------------
testlin.exe

Zum testen, ob das auf dem Zielsystem läuft, kann man testlin.exe nehmen. Das enthaelt ein minimales System mit
Setup+X11 (VGA16 - startx). 

Man kann auch mit cd / - tar zxvf /DOS/c245.tgz - sh install/doinst.sh - einen C-Compiler installieren.

Und mit cd / - tar zxvf /DOS/sast.tgz - das tolle sasteroids Spiel.

Zum Installieren:

c:  (c kann durch jeden DOS Laufwerksbuchstaben einer HD Partition ersetzt werden)

testlin.exe
 
---------------
zum Testen:

cd \linux

linux.bat c

login: root

setup

startx

CTRL-ALT-BS

CTRL-ALT-DEL

---------------
--- C-Compiler ---

c245.tgz im / root-Verzeichnis entpacken mit tar zxvf c245.tgz

Dann sh install/doinst.sh ausführen.


---------------
--- Deutsch ---

Hallo, bitte hiernach erst einmal \xlinux.txt lesen!

Dann Xlinux mit "install <x>" installieren. <x> = Ziellaufwerksbuchstabe ohne :\

Wenn sie kein IDE-System haben, MUESSEN sie NACH der Installation (s.o.) erst
einen passenden Kernel auswaehlen (kernels\README.DOS und kernels\CHIP.txt) und mit:

copy kernels\xxxx x:\linux\vmlinuz

kopieren und damit auch "installieren". xxxx = ihr gewaehlter Kernel muss also
nach <x=Ziellaufwerk>:\linux\vmlinuz kopiert werden. Sollte das System nicht
booten, so kann ein anderer Kernel mit obiger Prozedur probiert werden...

Beachten sie bitte!!! Die Kernel wurden freundlicher weise von S.u.S.E GmbH
zur Verfuegung gestellt! S.u.S.E uebernimmt natuerlich fuer diese Distribution
KEINERLEI Support!!! Bitte nicht bei Problemen dort anrufen...

Dann nach x:\linux gehen.

linux.bat x

login: root

setup

startx - startet X-Windows

CTRL-ALT-BS - beendet X-Windows

:q! - beendet den vi editor

...


---------------
--- English ---

Start install.bat with just the destination drive letter. Eg: install c
(NO :\)

Go into directory linux

linux.bat c

login: root

setup

startx - starts X-Windows

CTRL-ALT-BS - quits X-Windows

:q! - quits vi editor

...



