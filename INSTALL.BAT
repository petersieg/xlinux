@echo off
echo install.bat muss mit dem Ziellaufwerksbuchstaben aufgerufen werden!
echo Also zum Beispiel:  install c
echo Also nur der Buchstabe ohne : und \ !
echo Wenn dies nicht geschehen sein sollte, bitte jetzt mit CTRL/STRG-C
echo abbrechen...
pause
cd \workshop\xlinux
unrar x xlinux %1:\
