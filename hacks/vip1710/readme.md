Setup Web-Server on PC to allow wget 4 file's

myrc

settings2.xml

motorola.sh

vip1701.tar.bz2

telnet <box>

cd /tmp

wget <4 files-see above> from pc-web-werver
------


cp settings2.xml /flash

cp vip1710.tar.bz2 /flash2

cp motorola.sh /flash2

cp myrc /flash2

chmod 755 /flash2/myrc			# don't know if this is really needed..

chmod 755 /flash2/motorola.sh		# don't know if this is really needed..


sync

sync

(wait 1 min)

restart

-----

telnet <box>

ls 					# yes-sir-we-can-boogie file should be there

madstream <url>				# should play radio stream url eg: http://player.ffn.de/ffnstream.mp3

-----

Webbrowse to <box>			# should see homepage of etherplay; add streams etc.; play...

-----

http://player.ffn.de/ffnstream.mp3

http://stream.antenne.com/antenne-nds/mp3-128/stream.antenne.com/
