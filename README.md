# xlinux
xlinux distribution from 1996 as published in Chip 11/1996.
This is an old, historical distribution with kernal 1.2.13 and X11R5, that runs on top of a dos file system.
Just unrar it into root dir of hdd (will expand into \linux directory).

Links:
https://www.dim13.org/xlinux

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



